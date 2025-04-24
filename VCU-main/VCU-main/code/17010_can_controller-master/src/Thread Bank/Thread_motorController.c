#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#include "motor_controller.h"



static THD_WORKING_AREA(mc_wa, 256);
static THD_FUNCTION(thread_mc, p) {
    static int time = 0;
  
    
    while(true){
      time++;
      if(time == 250){
        ////chprintf((BaseSequentialStream*)&SD1,">>MCU Echo\r\n");
        MC_HeartBeat();
        time = 0;
      }
      
      
      chThdSleepMilliseconds(20);
        if(mc_dev.LASTtorque != mc_dev.torque){
          mc_dev.LASTtorque = mc_dev.torque;
          //chprintf((BaseSequentialStream*)&SD1,">>MCU Torque Cmd: %d \r\n", mc_dev.torque);
          MC_send_Torque(); 
        }
        
       
        
        
        MC_check0xAA();
        MC_checkFaultCode();
        
        
        
    }

}

void motorController_init(CANRxFrame* p){
    MC_VirtualMachine_init(p);
    chThdCreateStatic(mc_wa, sizeof(mc_wa), NORMALPRIO + 7, thread_mc, NULL);
}