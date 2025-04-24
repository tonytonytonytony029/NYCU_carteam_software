#include "ch.h"
#include "hal.h"

#define MCU_heartbeatID 0x01


typedef struct{
    /* state */
    uint8_t VM_TurnON:1;
    /* contorll flag */
    /* data */
    uint8_t motorDirection:1;
    int16_t torque;
    int16_t LASTtorque;
    uint16_t FBtorque;
} MotorController_VM_t;



extern MotorController_VM_t mc_dev;

bool MC_send_Torque(void);
bool MC_send_FaultClear(void);

bool MC_changeDirection(void);

bool MC_VirtualMachine_init(CANRxFrame*);

bool MC_check0xAA(void); // should put in a general CAN-receiving thread
bool MC_checkFaultCode(void);// should put in a general CAN-receiving thread

bool MC_HeartBeat(void);


