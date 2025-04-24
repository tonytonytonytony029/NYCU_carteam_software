#include "VCU.h"
#include "VCU_th.h"

#include "ch.h"
#include "hal.h"

#include "chprintf.h"

VCU_t vcu_dev = {0};
uint8_t mask = 0;

uint8_t* const pmask = &mask;

void DriverInit(){
  


#if DRIVER_DIGIT_IO_ENABLE == true
    digital_io_init();
    console_print(">>DigitIO Driver Initialization finished\r\n");
    chThdSleepMilliseconds(5);
#endif
}

int console_print(const char *fmt, ...){
  #if SYS_USE_CONSOLE  == true
  va_list ap;
  int formatted_bytes;
  BaseSequentialStream *chp = (BaseSequentialStream*)&(SYS_CONSOLE_CH);
  
  
  va_start(ap, fmt);
  formatted_bytes = chvprintf(chp, fmt, ap);
  va_end(ap);

  return formatted_bytes;
  #endif
}



void vcuStartFSM(){
  vcu_dev.state = VCU_INIT;
}
void vcuEnterTest(){
  vcu_dev.state = VCU_TEST;
  console_print(">> Enter <TEST> state\r\n");

  mask = 0; // clear mask
  thdCreateADCtest();
  
  while(mask != 0x01){
    chThdSleepMilliseconds(10);
  }
  
  console_print(">> Leave <TEST> state\r\n");
  mask = 0; // clear mask
  
}
void vcuEnterReady(){
  vcu_dev.state = VCU_READY;
  console_print(">> Enter <READY> state\r\n");
  
  mask = 0; // clear mask
  thdCreateRTDlock();
  
  while(mask != 0x01){
    chThdSleepMilliseconds(10);
  }
  
  console_print(">> Leave <READY> state\r\n");
  mask = 0; // clear mask
}
void vcuEnterDrive(){
  vcu_dev.state = VCU_DRIVE;
  console_print(">> Enter <DRIVE> state\r\n");
 
  thdCreateDashboard();
  thdCreateADC();
  thdCreateBMS();
  thdCreateMCU();
}


