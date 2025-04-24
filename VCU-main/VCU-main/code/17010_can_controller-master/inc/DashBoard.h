//#define CodingWihtoutIAR
#ifdef CodingWihtoutIAR
typedef unsigned char           uint8_t;  
typedef unsigned short int      uint16_t;
#endif
#define WheelTemperatureLimit 0xFF
#define WheelPressureLimit 0xFF
#define WheelSuspensionLimit 0xFF
#define Page_Driving_ItemAmount 14
#define Page_Pit1_ItemAmount 17
#define Page_Pit2_ItemAmount 11
#define Page_Pit3_ItemAmount 6
#define Rcv_OK 0x06
#define Rcv_error 0x15

#define DB_DEBUG false
#define DB_NEWLINE false

#include "hal.h"


typedef enum{
    driving =0,
    pit1,
    pit2,
    pit3,
} DB_Page_t;

typedef struct DashBoard
{
    uint16_t data[49];
    uint16_t motorSpeed;
    uint16_t wheelDirection;
    DB_Page_t page;
    uint16_t BigLED_Flag;
    uint16_t SmallLED_Flag;
} DashBoard_t ;


// array of data
extern DashBoard_t dashboard_dev;

//macro function
void setBit(uint16_t* flag, uint8_t n, uint8_t bit);

uint8_t getBit(uint16_t* flag,uint8_t n);



// DB: Dashboard
// CMD: command
bool DB_CMD_Show_Page(uint8_t id);
bool DB_CMD_Show_ItemValue(uint8_t id);
bool DB_CMD_Show_MotorSpeed(void);
bool DB_CMD_Show_WheelDirection(void);

bool DB_CMD_SetAndShow_BigLED(uint8_t id, uint8_t data);
bool DB_CMD_SetAndShow_SmallLED(uint8_t id, uint8_t data);

// other stuff
void DB_Check_Lap(void);
void DB_Check_Wheel(void);
void DB_Check_LED(void);
bool DB_PageViewRefresh(void);