#define HOLDINGREG_NUM 30
#define SLAVE_NUM 10





typedef enum{   
    NO_ERROR,
    RCV_Empty,
    CRC_error,
    RCV_address,
    NOT_init,
}   modbus_error_t;

typedef struct modbus
{
    /*private data*/
    uint8_t turn_on_flag:1;
    modbus_error_t state;
    /*public data*/
    uint16_t usMRegHoldBuf[SLAVE_NUM][HOLDINGREG_NUM];
} modbus_t;

extern modbus_t modbus_dev;

/**
 * num: the register number to read
 * 
*/
void MB_readHoldingRegister(int8_t id, int16_t start_pos, int16_t num);
bool MB_init(void);
void MB_print_ALL();

