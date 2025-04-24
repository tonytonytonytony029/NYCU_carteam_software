#include "ch.h"
#include "hal.h"
#include "mrxx.h"



int8_t mr25_cmd_strobe(uint8_t cmd,const MR25XX_dev_t *dev)
{
  int result = MR25_OK;
  dev->chipsel(0);
  dev->write(&cmd,1);
  dev->chipsel(1);
  return result;
}

int8_t mr25_read_reg(uint8_t reg_addr, uint8_t *data, uint16_t len, const MR25XX_dev_t *dev)
{
  int result = MR25_OK;
  dev->chipsel(0);
  dev->write(&reg_addr,1);
  dev->read(data,len);
  dev->chipsel(1);
  return result;
}

int8_t mr25_write_regs(uint8_t reg_addr, uint8_t *data, uint16_t len, const MR25XX_dev_t *dev)
{
  int result = MR25_OK;
  dev->chipsel(0);
  dev->write(&reg_addr,1);
  dev->write(data,len);
  dev->chipsel(1);
  return result;
}

int8_t mr25_data_read(uint32_t offset, uint8_t *data, uint32_t sz, const MR25XX_dev_t *dev)
{
  int result = MR25_OK;
  if((offset + sz) > dev->capacity) return MR25_WRONG_ADDR;

  uint8_t tx[8];
  tx[0] = MR25_CMD_READ;
  tx[1] = (offset >> 16)&0xff;
  tx[2] = (offset >> 8)& 0xff;
  tx[3] = (offset) & 0xff;
  
  dev->chipsel(0);
  dev->write(tx,4);
  dev->read(data,sz);
  dev->chipsel(1);
  
  return result;
}

int8_t mr25_data_write(uint32_t offset, uint8_t *data, uint32_t sz, const MR25XX_dev_t *dev)
{
  int result = MR25_OK;
  uint8_t tx[8];

  if((offset + sz) > dev->capacity) return MR25_WRONG_ADDR;
  
  tx[0] = MR25_CMD_WRITE;
  tx[1] = (offset >> 16)&0xff;
  tx[2] = (offset >> 8)& 0xff;
  tx[3] = (offset) & 0xff;
  
  dev->chipsel(0);
  dev->write(tx,4);
  dev->write(data,sz);
  dev->chipsel(1);
  
  return result;
}

int8_t mr25_init(MR25XX_dev_t *dev)
{
  int result = MR25_OK;
  uint8_t sta = 0x2;
  // enable write
  mr25_cmd_strobe(MR25_CMD_WREN,dev);
  mr25_read_reg(MR25_CMD_RDSR,&dev->status,1,dev);
  
  return result;
}

void mr25_read_write_test(uint32_t offset, MR25XX_dev_t *dev)
{
  uint8_t tx[64],rx[64];
  for(uint8_t i=0;i<64;i++){
    tx[i] = i;
    rx[i] = 0;
  }
  mr25_data_write(offset,tx,64,dev);
  mr25_data_read(offset,rx,64,dev);
  
  
}