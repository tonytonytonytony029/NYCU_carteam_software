#include "ch.h"
#include "hal.h"
#include "vmu.h"
#include "digital_io.h"
//#include "ad5593r.h"
#include "ntc.h"
#include "mr25_config.h"
//#include "flash.h"
#include "shell.h"
#include "at24_eep.h"


static THD_WORKING_AREA(waRS485Test, 512);
static THD_FUNCTION(thRS485BTest, arg){
  bool done = false;
  char msg_out[] = "This is a test message!";
  char msg_in[32];
  uint32_t sz;
  uint32_t test_cycle = 0;
  uint32_t errCode = 0;
  size_t sSize = 32;
 
  
  const int sleepTime = 1;
  chprintf((BaseSequentialStream*)&SD1,">>> SD1 test sending with Sleep Time = %dms\r\n\n",sleepTime);
  //sdWriteTimeout(&SD1, ">>> SD1 test sending with Sleep Time = 10ms\r\n\n", strlen(">>> SD1 test sending with Sleep Time = 5ms\r\n\n"), MS2ST(10));
  int counter =0;
  while(true){
    //sdWrite(&SD3,msg_out,strlen(msg_out));
    //sdWrite(&SD2, msg_out, strlen(msg_out));
    chThdSleepMilliseconds(MS2ST(sleepTime));

    //sdWriteTimeout(&SD1, "A test message\r\n", strlen("A test message\r\n"), MS2ST(10));
    chprintf((BaseSequentialStream*)&SD1,"the (%d)th message\r\n", counter++);
  }

  
  chprintf((BaseSequentialStream*)&SD1,"RS485 Starts message sending test\r\n");
  chThdSleepMilliseconds(MS2ST(20));
  
  chprintf((BaseSequentialStream*)&SD1,"RS485 Starts message sending test 2\r\n");
  for(int counter = 0; counter<10; counter++){
    chprintf((BaseSequentialStream*)&SD1,">>> SerialDriver3 Sending %d th message \n\n", counter);
    sdWrite(&SD3,msg_out,strlen(msg_out));
    chThdSleepMilliseconds(MS2ST(20));
  }
  
  for(int counter = 0; counter<10; counter++){
    chprintf((BaseSequentialStream*)&SD1,">>> SD2 Sending %dth message \n\n", counter);
    sdWrite(&SD2, msg_out, strlen(msg_out));
    chThdSleepMilliseconds(MS2ST(20));
  }
  
  chprintf((BaseSequentialStream*)&SD1,"RS485 Starts message receiving test\n\n");

  while(!done){
    
    chprintf((BaseSequentialStream*)&SD1,"UARTD2 sends & SD3 receives\n\n");
    sdWrite(&SD2, msg_out, strlen(msg_out));
    //chThdSleepMilliseconds(MS2ST(50));
    sz = sdReadTimeout(&SD3,msg_in,32,MS2ST(100));
    
    if(sz > 0){
      chprintf((BaseSequentialStream*)&SD1,"SD3 received %d bytes of message", sz);
      if(strncmp(msg_in,msg_out,sz) != 0){
        errCode = 0x1;
        RS485_state = errCode;
        chprintf((BaseSequentialStream*)&SD1,", which differ from origin message\n");
        chprintf((BaseSequentialStream*)&SD1,">>> %S\n", msg_in);
        
        //done = true;
        //break;
      }
      else{
        chprintf((BaseSequentialStream*)&SD1,", which fit origin message\n");
        errCode = 0x0;
        RS485_state = errCode;        
      }
    }
    else if(sz == 0){
        chprintf((BaseSequentialStream*)&SD1,", nothing has been received.\n");
      errCode = 41;
      RS485_state = errCode;
      RxSizeZeroFlag = test_cycle;
      //break;
    }
    
    chThdSleepMilliseconds(MS2ST(200));
    
    chprintf((BaseSequentialStream*)&SD1,"SD3 sends & UARTD2 receives\n");
    chprintf((BaseSequentialStream*)&SD1,"SD3 sends %d bytes of message", strlen(msg_out));
    sdWrite(&SD2, msg_out, strlen(msg_out));
    //chThdSleepMilliseconds(MS2ST(50));
    sdRead(&SD2, msg_in, strlen(msg_in));
    sz = strlen(msg_in);
    if(sz > 0){
      chprintf((BaseSequentialStream*)&SD1,"UARTD3 received %d bytes of message", sz);
      if(strncmp(msg_in,msg_out,sz) != 0){
        errCode = 0x1;
        RS485_state = errCode;
        chprintf((BaseSequentialStream*)&SD1,", which differ from origin message\n");
        chprintf((BaseSequentialStream*)&SD1,">>> %S\n", msg_in);
        
        //done = true;
        //break;
      }
      else{
        chprintf((BaseSequentialStream*)&SD1,", which fit origin message\n");
        errCode = 0x0;
        RS485_state = errCode;        
      }
    }
    else if(sz == 0){
        chprintf((BaseSequentialStream*)&SD1,", nothing has been received.\n");
      errCode = 41;
      RS485_state = errCode;
      RxSizeZeroFlag = test_cycle;
      //break;
    }

    chThdSleepMilliseconds(MS2ST(200));   
  }

  chThdExit((msg_t)(errCode));
}

void RS485_test(void){
    chThdCreateStatic(waRS485Test,sizeof(waRS485Test),NORMALPRIO+5,thRS485BTest,NULL);
}