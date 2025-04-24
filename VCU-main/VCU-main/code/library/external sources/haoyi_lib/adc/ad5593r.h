
/**
* @file  ad5593r.h
* @brief ADI ADXL355 interface module header
*
* @addtogroup -- 
* @ingroup --

*/
#ifndef _AD5593R_H
#define _AD5593R_H
#endif

#define EX_AD5593R_VERSION      "1.0.0"
#define EX_AD5593R_MAJOR        1
#define EX_AD5593R_MINOR        0
#define EX_AD5593R_PATCH        0

#define AD5593R_NUMBER_OF_CHANNEL       8

/**
/* @name        AD5593R register addresses
 * @{
 */

#define AD5593_NOP      0
#define AD5593_REG_SEQ  0x2
#define AD5593_REG_GPCR 0x3
#define AD5593_REG_ADCP 0x4
#define AD5593_REG_DACP 0x5
#define AD5593_REG_PDC  0x6
#define AD5593_REG_LDAC 0x7
#define AD5593_REG_GWCF 0x8
#define AD5593_REG_GWDA 0x9
#define AD5593_REG_GRC  0xa
#define AD5593_REG_PDRC 0xb
#define AD5593_REG_ODCG 0xc
#define AD5593_REG_RST  0xf

// mode register definition
#define AD5593_MODE_CONFIG      0x0
#define AD5593_MODE_DACW        0x10
#define AD5593_MODE_ADCR        0x40
#define AD5593_MODE_DACR        0x50
#define AD5593_MODE_GPIOR       0x60
#define AD5593_MODE_REGR        0x70

#if !HAL_USE_I2C
#error "AD5593R require HAL_USE_I2C"
#endif
   
#define AD5593R_ADDR_BASE       0x10
#define AD5593R_CHIP_ADDR_0     AD5593R_ADDR_BASE+0
#define AD5593R_CHIP_ADDR_1     AD5593R_ADDR_BASE+1
#define AD5593R_LSB     10000/4096
#define REG_CFG        0x00
#define REG_DACWR       0x10
#define REG_ADCRD       0x40
#define REG_DACRD       0x50
#define REG_GIORD       0x60
#define REG_REGRD       0x70

#define AD5593_GPCR_ADC_BUF_PRECHARGE   0x200
#define AD5593_GPCR_ADC_BUF_ENABLE      0x100
#define AD5593_GPCR_PIN_LOCK            0x80
#define AD5593_GPCR_DAC_WRITE_ALL       0x40
#define AD5593_GPCR_ADC_VREF_2X         0x20
#define AD5593_GPCR_DAC_VREF_2X         0x10

typedef enum
{
  AD5593R_UNINIT = 0,
  AD5593R_STOP = 1,
  AD5593R_READY = 2,
  AD5593R_MEASURE = 3,
}ad5593_state_t;

typedef enum{
  PIN_ADC,
  PIN_DAC,
  PIN_GPIO,
}ad5593r_pintype_t;

typedef enum{
  VIO_VREF,
  VIO_2VREF
}ad5593r_vio_t;

typedef enum{
  AD5593_DEV0,
  AD5593_DEV1
}ad5593r_dev_sel;

/**
  @brief AD5593R configuration structure

*/

typedef struct{
  I2CDriver *i2cp;
  const I2CConfig *i2ccfg;
  ad5593r_dev_sel devSel; 
  uint16_t gpcr;
  uint8_t adc_pin_mask;
  uint8_t dac_pin_mask;
  uint8_t *pinConfig;
  ad5593r_vio_t vio;
}AD5593RConfig;

typedef struct AD5593RDriver AD5593RDriver;

#define _ad5593r_methods \
  msg_t (*start)(void *instance); \
  msg_t (*stop)(void *instance); \
  msg_t (*read_adc)(void *instance); \
  msg_t (*write_dac)(void *instance, int16_t *data); \
  

struct AD5593RVMT{
  _ad5593r_methods
};

#define _ad5593r_data \
  ad5593_state_t state; \
  const AD5593RConfig *config; \
  uint8_t address; \
  int16_t *results; \

struct AD5593RDriver{
  const struct AD5593RVMT *vmt_ad5593r;
  _ad5593r_data
};


void ad5593rObjectInit(AD5593RDriver *devp);
void ad5593rStart(AD5593RDriver *devp, const AD5593RConfig *config);
void ad5593rStop(AD5593RDriver *devp);
void ad5593rScanStart(AD5593RDriver *devp);
void ad5593rScanStop(AD5593RDriver *devp);
void ad5593rRead(AD5593RDriver *devp);


