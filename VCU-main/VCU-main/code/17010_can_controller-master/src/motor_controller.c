#include "stdlib.h"
#include "motor_controller.h"

uint32_t RcvErrorMask = 0xFFFFFFFF;
MotorController_VM_t mc_dev = {0};
#define rxmsg (*pRxmsg)

static bool inverter;
static CANRxFrame* pRxmsg;
static bool suspendTorqueCMD;



bool MC_send_FaultClear(void){
    if(mc_dev.VM_TurnON != TRUE) return FALSE;
    
    CANTxFrame txmsg;

    txmsg.IDE = CAN_IDE_STD;
    txmsg.SID = 0xC1;
    txmsg.RTR = CAN_RTR_DATA;
    txmsg.DLC = 8;

    txmsg.data8[0] = 0x0014 >>8;
    txmsg.data8[1] = 0x0014;
    txmsg.data8[2] = 1;
    txmsg.data8[3] = 0;
    txmsg.data8[4] = 0;
    txmsg.data8[5] = 0;
    txmsg.data8[6] = 0;
    txmsg.data8[7] = 0;
    canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));

    return TRUE;
}

bool MC_send_Torque(void){
    if(mc_dev.VM_TurnON != TRUE) return FALSE;
    //if(suspendTorqueCMD == TRUE) return FALSE;
    
    CANTxFrame txmsg;

    txmsg.IDE = CAN_IDE_STD;
    txmsg.SID = 0xC0;
    txmsg.RTR = CAN_RTR_DATA;
    txmsg.DLC = 8;
    
    txmsg.data8[0] = mc_dev.torque ;
    txmsg.data8[1] = mc_dev.torque >> 8;
    txmsg.data8[2] = 0;
    txmsg.data8[3] = 0;
    txmsg.data8[4] = 0;
    txmsg.data8[5] = 0;
    txmsg.data8[6] = 0;
    txmsg.data8[7] = 0;
    canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
    
    txmsg.data8[0] = mc_dev.torque ;
    txmsg.data8[1] = mc_dev.torque >> 8;
    txmsg.data8[2] = 0;
    txmsg.data8[3] = 0;
    txmsg.data8[4] = 1;
    txmsg.data8[5] = 1;
    txmsg.data8[6] = 0;
    txmsg.data8[7] = 0;
    canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
    
    return TRUE;
}

bool MC_changeDirection(void){
    
    mc_dev.motorDirection = ~mc_dev.motorDirection;
    MC_send_Torque();

    return TRUE;
}


bool MC_check0xAA(void){
    if(mc_dev.VM_TurnON != TRUE) return FALSE;
    
    if (!(rxmsg.data8[6]==128 || rxmsg.data8[6]==129)) {
        suspendTorqueCMD = TRUE;
        return FALSE;
    }

    if ((rxmsg.data8[5]&0x06) > 0) { // 0x0006 = 0b0000 0110  
        suspendTorqueCMD = TRUE;
        return FALSE;
    }
    
    suspendTorqueCMD = FALSE;
    
    return TRUE;
}

bool MC_checkFaultCode(void){
    if(mc_dev.VM_TurnON != TRUE) return FALSE;
    
    if(rxmsg.SID != 0xAB) return TRUE;
    if((rxmsg.data32[1]&RcvErrorMask) == 0) return TRUE;

    MC_send_FaultClear();
    
    return FALSE;
}

bool MC_VirtualMachine_init(CANRxFrame* pCRF){
    mc_dev.motorDirection =0;
    mc_dev.torque =0;
    inverter = 1;
    suspendTorqueCMD = FALSE;
    pRxmsg = pCRF;
    
    mc_dev.VM_TurnON = TRUE;
}

bool MC_HeartBeat(void){
    if(mc_dev.VM_TurnON != TRUE) return FALSE;
    
    CANTxFrame txmsg;

    txmsg.IDE = CAN_IDE_STD;
    txmsg.SID = MCU_heartbeatID;
    txmsg.RTR = CAN_RTR_DATA;
    txmsg.DLC = 8;

    txmsg.data8[0] = 1;
    txmsg.data8[1] = 0;
    txmsg.data8[2] = 0;
    txmsg.data8[3] = 0;
    txmsg.data8[4] = 0;
    txmsg.data8[5] = 0;
    txmsg.data8[6] = 0;
    txmsg.data8[7] = 0;
    canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));

    return TRUE;
}