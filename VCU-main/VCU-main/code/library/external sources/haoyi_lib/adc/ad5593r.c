#include "hal.h"
#include "ad5593r.h"

#define I2C_READ_MASK   0x01
#define I2C_TIMEOUT     MS2ST(10)                   
                   
//static void ad5593rWritereg(I2CDriver *p, uint8_t sla,size_t n, uint8_t *d)
static void ad5593rWritereg(AD5593RDriver *dev,size_t n, uint8_t *d)
{
  i2cAcquireBus(dev->config->i2cp);
  i2cStart(dev->config->i2cp,dev->config->i2ccfg);
  msg_t ret;
                                 
  ret = i2cMasterTransmitTimeout(dev->config->i2cp,
                                 dev->address,
                                 d,
                                 n,
                                 NULL,
                                 0,
                                 I2C_TIMEOUT);
//  chThdSleepMilliseconds(5);
  if(ret == MSG_RESET){
    i2cflags_t flag = i2cGetErrors(dev->config->i2cp);
  }
 
  i2cStop(dev->config->i2cp);
  i2cReleaseBus(dev->config->i2cp);
}

//static void ad5593rReadReg(I2CDriver *p,uint8_t sla, uint8_t reg,size_t n, uint8_t *d)
static void ad5593rReadReg(AD5593RDriver *dev, uint8_t reg,size_t n, uint8_t *d)
{
//  i2cAcquireBus(p);
  i2cAcquireBus(dev->config->i2cp);
  i2cStart(dev->config->i2cp,dev->config->i2ccfg);
  msg_t ret;
  
  ret = i2cMasterTransmitTimeout(dev->config->i2cp,
                                 dev->address,
                                 &reg,
                                 1,
                                 d,
                                 n,
                                 I2C_TIMEOUT);
 
  
//  chThdSleepMilliseconds(5);
//  i2cReleaseBus(p);
  i2cStop(dev->config->i2cp);
  i2cReleaseBus(dev->config->i2cp);
}


static msg_t start(void *instance)
{
#define g ((AD5593RDriver*)instance)
  osalDbgAssert(g->state == AD5593R_STOP,"start(), invalid state");
  //osalDbgAssert(g->config->i2cp->state == I2C_READY,"start(), invalid state");
  
  uint8_t tx[4];
  uint8_t rx[4];
  
  // write gpcr register
  tx[0] = AD5593_MODE_CONFIG | AD5593_REG_GPCR;
  tx[1] = (g->config->gpcr >> 8);
  tx[2] = (g->config->gpcr & 0xff);
  
  chThdSleepMilliseconds(20);
  ad5593rWritereg(g,3,tx);
  chThdSleepMilliseconds(20);
  ad5593rReadReg(g,AD5593_REG_GPCR | AD5593_MODE_REGR,2,rx);
  chThdSleepMilliseconds(20);
  
  // write ADC pin configuration register, set all pin to ADC by default
  tx[0] = AD5593_MODE_CONFIG | AD5593_REG_ADCP;
  tx[1] = 0;
  tx[2] = g->config->adc_pin_mask;
  ad5593rWritereg(g,3,tx);
  ad5593rReadReg(g,AD5593_REG_ADCP | AD5593_MODE_REGR,2,rx);
  
  // write ADC pin configuration register, set all pin to ADC by default
  tx[0] = AD5593_MODE_CONFIG | AD5593_REG_SEQ;
  tx[1] = 0x2;
  tx[2] = 0xff;
  ad5593rWritereg(g,3,tx);
  ad5593rReadReg(g,AD5593_REG_SEQ | AD5593_MODE_REGR,2,rx);
  
  // enable Vref appear on pin
  tx[0] = AD5593_MODE_CONFIG | AD5593_REG_PDRC;
  tx[1] = 0x2;
  tx[2] = 0x0;
  ad5593rWritereg(g,3,tx);
  ad5593rReadReg(g,AD5593_REG_PDRC | AD5593_MODE_REGR,2,rx);
  g->state = AD5593R_READY;

#undef g
  return MSG_OK;
}

static msg_t stop(void *instance)
{
#define g ((AD5593RDriver*)instance)
  
  g->state = AD5593R_STOP;
  
  return MSG_OK;
#undef g
}

static msg_t read_adc(void *instance)
{
#define g ((AD5593RDriver*)instance)
  uint8_t c = AD5593_MODE_ADCR;
  uint8_t d[16];
  ad5593rWritereg(g,1,&c);
  chThdSleepMilliseconds(5);
  ad5593rReadReg(g,AD5593_MODE_ADCR,16,(uint8_t*)d);
  for(int8_t i=0;i<8;i++){
    g->results[i] = (d[i*2+1] << 0) | (d[i*2]<<8);
    g->results[i] &= 0xfff;
    
  }
  return MSG_OK;
#undef g
}

static msg_t write_dac(void *instance, int16_t *result)
{
  
  return MSG_OK;
}

static const struct AD5593RVMT vmt_ad5593r = {
  start,
  stop,
  read_adc,
  write_dac,  
};


void ad5593rObjectInit(AD5593RDriver *devp)
{
#define g ((AD5593RDriver*)devp)
  
  g->vmt_ad5593r = &vmt_ad5593r;
  g->config = NULL;
  g->state = AD5593R_STOP;

#undef g  
}

void ad5593rStart(AD5593RDriver *devp, const AD5593RConfig *config)
{
#define g ((AD5593RDriver*)devp)
  osalDbgCheck(devp != NULL);
  osalDbgAssert(g->state == AD5593R_STOP || g->state == AD5593R_READY,"ad5593rObjectInit");
  
  g->config = config;
  if(g->config->i2cp->state != I2C_READY)
    //i2cStart(g->config->i2cp,g->config->i2ccfg);
  g->address = AD5593R_ADDR_BASE | g->config->devSel;
  g->vmt_ad5593r->start(g);
  g->state = AD5593R_READY;
#undef g  
}

void ad5593rStop(AD5593RDriver *devp)
{
#define g ((AD5593RDriver*)devp)
  i2cStop(devp->config->i2cp);
  devp->state = AD5593R_STOP;
#undef g  
}

void ad5593rScanStart(AD5593RDriver *devp)
{
#define g ((AD5593RDriver*)devp)
  
#undef g  
}

void ad5593rScanStop(AD5593RDriver *devp)
{
#define g ((AD5593RDriver*)devp)
  
#undef g  
}

void ad5593rRead(AD5593RDriver *devp)
{
  read_adc(devp);
}
