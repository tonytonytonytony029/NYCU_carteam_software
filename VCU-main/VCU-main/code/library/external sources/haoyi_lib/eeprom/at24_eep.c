#include "ch.h"
#include "hal.h"

#include "at24_eep.h"
#include "string.h"


#define PAGE_SIZE 16
#define EEP_ADDR        0x50
#define SLA 0x00

#define EEP_BUFFER_SIZE 64
static uint8_t page_size = 16;
static uint8_t page_count = 128;
static uint8_t self_addr = 0x50;
static uint8_t length_bytes = 1;
static I2CDriver *devp = 0;

static const I2CConfig i2ccfg = {
  OPMODE_I2C,
  50000,
  STD_DUTY_CYCLE,
};


static uint8_t txBuf[EEP_BUFFER_SIZE];
static uint8_t rxBuf[EEP_BUFFER_SIZE];

void at24eep_init(I2CDriver *i2cp,uint8_t p_size, uint32_t p_count, uint8_t address, uint8_t len)
{
  devp = i2cp;
  page_size = p_size;
  page_count = p_count;
  self_addr = address;
  length_bytes = len;
}

int eepromRead(uint16_t addr, size_t s, void *buf)
{
  uint16_t bufRead=0;
  uint16_t bufToRead;
  uint16_t address = addr;
  uint8_t addr_h = (addr >> 8);
  uint8_t addr_l = addr & 0xff;
  uint8_t *bufptr = buf;
  static msg_t ret;
  
  if(!devp) return -1;  
  i2cAcquireBus(devp);
  i2cStart(devp,&i2ccfg);
  // check if address is aligned
  int r = addr - (addr / page_size)*page_size;
//  if(r)
//    r = page_size - r;
  while(bufRead < s-1){
    if(r){
      bufToRead = r;
      r = 0;
    }else{
      bufToRead = ((s - bufRead)>page_size)?(page_size):(s-bufRead);
    }
    addr_h = (address >> 8);
    addr_l = address & 0xff;
    
    if(length_bytes == 1){
      txBuf[0] = addr_l;
      ret = i2cMasterTransmitTimeout(devp,
                               self_addr | addr_h,
                               txBuf,
                               length_bytes,
                               rxBuf,
                               bufToRead,
                               MS2ST(100));
      }
    else{
      txBuf[0] = addr_h;
      txBuf[1] = addr_l;
      ret = i2cMasterTransmitTimeout(devp,
                               self_addr,
                               txBuf,
                               length_bytes,
                               rxBuf,
                               bufToRead,
                               MS2ST(100));
      
    }
    address += bufToRead;
    bufRead += bufToRead;
    memcpy(bufptr,rxBuf,bufToRead);
    bufptr+=bufToRead;
    chThdSleepMilliseconds(2);
  }
  i2cStop(devp);
  i2cReleaseBus(devp);
  return ret;
}

int eepromWrite(uint16_t addr, size_t s, void *buf)
{
  uint16_t bufWrote = 0;
  uint16_t bufToWrite;
  uint16_t address = addr;
  uint8_t *bufptr = buf;
  uint8_t addr_h = (addr >> 8);
  uint8_t addr_l = addr & 0xff;
  msg_t ret;
  
  if(!devp) return -1;  
  i2cAcquireBus(devp);
  i2cStart(devp,&i2ccfg);
  // check if address is aligned
  int r = addr - (addr / page_size)*page_size;
//  if(r)
//    r = page_size - r;

  while(bufWrote < s-1){
    if(r){
      bufToWrite = r;
      r = 0;
    }else{
      bufToWrite = ((s - bufWrote)>page_size)?(page_size):(s-bufWrote);
    }

    addr_h = (address >> 8);
    addr_l = address & 0xff;
      txBuf[0] = addr_l;
    if(length_bytes == 1){      
      memcpy(&txBuf[1],bufptr,bufToWrite);
      ret = i2cMasterTransmitTimeout(devp,
                               self_addr | addr_h,
                               txBuf,
                               bufToWrite+length_bytes,
                               rxBuf,
                               0,
                               MS2ST(10));
      }
    else{
      txBuf[0] = addr_h;
      txBuf[1] = addr_l;
      memcpy(&txBuf[2],bufptr,bufToWrite);
      ret = i2cMasterTransmitTimeout(devp,
                               self_addr,
                               txBuf,
                               bufToWrite+length_bytes,
                               rxBuf,
                               0,
                               MS2ST(10));
      
    }
    address += bufToWrite;
    bufWrote += bufToWrite;
    bufptr += bufToWrite;
    chThdSleepMilliseconds(5);
  }
  i2cStop(devp);
  i2cReleaseBus(devp);
  return ret;
}



void eepTest(void)
{
  uint8_t data[64];
  static int8_t ret[64];
  int8_t i;
  //uint16_t addr = 0x00;
  
  for(i=0;i<64;i++) data[i] = i*8;
  
  eepromWrite(0x0,64,data);
  eepromRead(0x0,64,ret);
  
  bool fail = false;
  for(i=0;i<64;i++){
    if(ret[i] != data[i]){
      fail = true;
    }
  }
  
}
