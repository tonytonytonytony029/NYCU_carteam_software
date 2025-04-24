#include "SubsystemKit.h"
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "VCU.h"



/*
void Init(){
  bool acce = true; // Accelerator
  
  bool can = false;
  bool mcu = true;
  
  bool db = false; //dashboard
  bool shell = false;
  
  bool modbus = false;
  
  bool digit_io = false;
  
#if 
      adc_init();
      chThdSleepMilliseconds(5);
      console_print(">>Accelerator Initialization finished\r\n");
      chThdSleepMilliseconds(5);
#endif
  if(can || mcu){
    can_rx_thd_init();
    chThdSleepMilliseconds(5);
    console_print(">>MCU Initialization finished\r\n");
    chThdSleepMilliseconds(5);
  }
  
  if(modbus){
    MB_init();
    chThdSleepMilliseconds(5);
    console_print(">>Modbus Initialization finished\r\n");
    chThdSleepMilliseconds(5);
  }
  
  if(shell && db){
    console_print(">>Error: Dashboard and Shell fighting for the same TTL channel.\r\n");
    chThdSleepMilliseconds(10);
    console_print(">>Error: Exit\r\n");
    exit(0);
  }
  
  if(shell){
    shellInit();
    THD_FUNCTION(shellThread, SD1);
    console_print(">>Shell Initialization finished\r\n");
    chThdSleepMilliseconds(5);
  }
  
  if(db){
    dashboard_init();
    console_print(">>Dashboard Initialization finished\r\n");
    chThdSleepMilliseconds(5);
  }
  
  if(digit_io){
    digital_io_init();
    console_print(">>DigitIO Initialization finished\r\n");
    chThdSleepMilliseconds(5);
  }
}
*/
