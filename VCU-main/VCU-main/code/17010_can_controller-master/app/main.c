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
#include "ThreadBank.h"
#include "digital_io.h"
#include "motor_controller.h"
#include <time.h>
#include "subsystem1.h"
#include "SubsystemKit.h"
#include "VCU.h"


uint8_t reset_flags = 0;
extern thread_t *thdAD7124; 
uint8_t toRead[8] = {0};
int page =0;

//extern int16_t torque, torqueRatio;/*for accel debug*/
//extern void f_torque(void);

stm32_gpio_t * gpioD;
stm32_gpio_t * gpioC;
stm32_gpio_t * gpioB;


int array[10] = {0};
void test();
void test(){
  if(VCU_DEBUG != true) return;
  
  while(false){
    console_print(">>ADC:");
    for(int i=0; i<8;i++){
      console_print(" %d", ad0Result[i]);
      array[i] = ad0Result[i];
    }
    console_print("\r\n");
    chThdSleepMilliseconds(500);
  }
  
  
  while(true){
    console_print("Digital input:\r\n");
    for(int i = 0; i < 8; i++){
      console_print("CH%d: %d ", i+1, dio_read(i));
            array[i] = dio_read(i);
    }
    console_print("\r\n");
    
    chThdSleepMilliseconds(500);
  } 
}



int main(int argc, char* argv[]) {
  
  // There are 4 states for VCU: INIT, TEST, READY, DRIVE
  // The line of code below will start the State Machine and
  // bring the state into INIT.
  vcuStartFSM(); 
  halInit();
  chSysInit();
  vmuInit();
  DriverInit();
  
  vcuEnterTest();
  vcuEnterReady();
  vcuEnterDrive();
  
  
  test();
  
  
  while (1) {
    if(true){
      chThdSleepMilliseconds(5000);
      console_print(">>Echo\r\n");
    } 
    
  } 
   
}

