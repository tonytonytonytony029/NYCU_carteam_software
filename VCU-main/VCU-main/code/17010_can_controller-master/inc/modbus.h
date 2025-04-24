#define HOLDINGREG_NUM 30
#define SLAVE_NUM 20





typedef enum{   
    NO_ERROR,
    RCV_Empty,
    CRC_error,
    RCV_address,
    NOT_init,
}   modbus_error_t;

typedef enum
{
  cell_1 = 0,
  cell_2,
  cell_3,
  cell_4,
  cell_5,
  cell_6,
  cell_7,
  cell_8,
  cell_9,
  cell_10,
  cell_11,
  cell_12,
  cell_total,
  cell_lowest,
  cell_highest,
  lowestCell,
  highestCell,
  ntc_1,
  ntc_2,
  ntc_3,
  ntc_4,
  ntc_5,
  ntc_lowest,
  ntc_highest,
  lowestNTC,
  highestNTC,
} bmu_field;

typedef struct modbus
{
    /*private data*/
    uint8_t OnSendId;
    uint8_t turn_on_flag:1;
    modbus_error_t state;
    /*public data*/
    uint16_t usMRegHoldBuf[SLAVE_NUM][HOLDINGREG_NUM];
} modbus_t;

typedef struct {
  int id;
  int field;
} modbus_block_t;

extern modbus_t modbus_dev;

/**
 * num: the register number to read
 * 
*/
void MB_readHoldingRegister(int8_t id, int16_t start_pos, int16_t num);
bool MB_init(void);
void MB_print_ALL();
void MB_print_state();

