#include <stdio.h>
#include <string.h>
#include "ch.h"
#include "hal.h"
#include "shell.h"
#include "evtimer.h"
#include "chprintf.h"
#include "stdlib.h"
#include "hal_ad7124.h"
#include "sysParam.h"
//#include "ff.h"
#include "vmu.h"
#include "motor_controller.h"

/*============
MCU recieve handler
==============*/
uint16_t MCU_handler(CANRxFrame *pFrame){
  uint16_t FB_torque = 0;
  FB_torque = pFrame->data8[2];
  FB_torque += (pFrame->data8[3] << 8);
  return FB_torque;
}

/*============
CAN rx Thread
==============*/
CANRxFrame CANRxmsg;
uint16_t RCVed_statistic;
uint16_t former_time, current_time;
static THD_WORKING_AREA(can_rx_wa, 512);
static THD_FUNCTION(can_rx, p) {

  uint8_t RCVed_counter = 0;
  
  current_time = ST2S(chVTGetSystemTime());
  former_time = ST2S(chVTGetSystemTime());
  
  event_listener_t el;
  //CANRxFrame CANRxmsg;
  
  motorController_init(&CANRxmsg);
  BMS_VM_init(&CANRxmsg);
  
  (void)p;
  chRegSetThreadName("receiver");
  chEvtRegister(&CAND1.rxfull_event, &el, 0);
  while (true) {
    current_time = ST2S(chVTGetSystemTime());
    if((current_time-former_time)> 1){
      RCVed_statistic = RCVed_counter;
      RCVed_counter = 0;
      former_time = current_time;
    } 
    
    if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(10)) == 0){
      continue;
    }
    
    while (canReceive(&CAND1, CAN_ANY_MAILBOX, &CANRxmsg, TIME_IMMEDIATE) == MSG_OK) {
      /* Process message.*/
//       chprintf((BaseSequentialStream*)&SD1, ">>>RTR: %d, IDE: %d, EID: 0x%08x, DLC: %d: 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n",\
//               CANRxmsg.RTR,\
//               CANRxmsg.IDE, CANRxmsg.EID, CANRxmsg.DLC,\
//               CANRxmsg.data8[0], CANRxmsg.data8[1], CANRxmsg.data8[2], CANRxmsg.data8[3],\
//               CANRxmsg.data8[4], CANRxmsg.data8[5], CANRxmsg.data8[6], CANRxmsg.data8[7]\
//      );
      
      
      switch(CANRxmsg.SID){
        case 0xAC : // torqu
          mc_dev.FBtorque =  MCU_handler(&CANRxmsg);
          break;
        default:
          break;
      }
    }
     
  }
  
  
  chEvtUnregister(&CAND1.rxfull_event, &el);
}

void can_rx_thd_init(void){
    chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO + 7, can_rx, NULL);
}


/*============
CAN tx Thread
==============*/
static THD_WORKING_AREA(can_tx_wa, 256);
static THD_FUNCTION(can_tx, p) {
  CANTxFrame txmsg;

  (void)p;
  chRegSetThreadName("transmitter");
  txmsg.IDE = CAN_IDE_EXT;
  txmsg.EID = 0x01234567;
  txmsg.RTR = CAN_RTR_DATA;
  txmsg.DLC = 8;
  txmsg.data32[0] = ST2S(chVTGetSystemTime());
  txmsg.data32[1] = 0x12345678;

  while (true) {
    txmsg.data32[0] = ST2S(chVTGetSystemTime());
    txmsg.data32[1] = 0x12345678;
    txmsg.EID = 0x01234567;
    canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
    chThdSleepMilliseconds(1000);
  }
}

void can_tx_thd_init(void){
  chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 7, can_tx, NULL);    
}

/*============
CAN test
==============*/
void can_test(void){
    //CAN send test
    chprintf((BaseSequentialStream*)&SD1, ">>> CAN transmit test\n\n");
    chThdSleepMilliseconds(10);
    if (canTryTransmitI) chprintf((BaseSequentialStream*)&SD1, "canTryTransmit success\n");
    else chprintf((BaseSequentialStream*)&SD1, "CANNOT transmit can message (canTryTransmit failed\n");
}



