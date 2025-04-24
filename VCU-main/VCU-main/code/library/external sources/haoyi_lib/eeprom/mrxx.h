#ifndef _MRXX_
#define _MRXX_

#define MR25_CMD_WREN   0x06
#define MR25_CMD_WRDI   0x04
#define MR25_CMD_RDSR   0x05
#define MR25_CMD_WRSR   0x01
#define MR25_CMD_READ   0x03
#define MR25_CMD_WRITE  0x02
#define MR25_CMD_SLEEP  0xB9
#define MR25_CMD_WAKE   0xAB

typedef enum{
  MR25_OK,
  MR25_ERR,
  MR25_WRONG_ADDR,
}MR25_STATUS_E;

typedef int8_t (*mr25_com_fptr_t)(uint8_t *b,uint16_t n);
typedef int8_t (*mr25_chipsel_fptr_t)(uint8_t st);
typedef void (*mr25_delay_ms)(uint32_t period);

typedef struct MR25XX_dev{
  uint32_t capacity;
  uint8_t status;
  mr25_chipsel_fptr_t chipsel;
  mr25_com_fptr_t read;
  mr25_com_fptr_t write;
  mr25_delay_ms delay;
}MR25XX_dev_t;

int8_t mr25_data_read(uint32_t offset, uint8_t *data, uint32_t sz, const MR25XX_dev_t *dev);
int8_t mr25_data_write(uint32_t offset, uint8_t *data, uint32_t sz, const MR25XX_dev_t *dev);
int8_t mr25_init(MR25XX_dev_t *dev);


#endif