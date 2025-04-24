#include "ch.h"
#include "hal.h"
#include "vmu.h"
#include "digital_io.h"
#include "ntc.h"
#include "mr25_config.h"
//#include "flash.h"
#include "shell.h"
#include "at24_eep.h"

#ifndef _AD5593R_H
#include "ad5593r.h"
#endif


#include "stdbool.h"
#define BufferSize 32

#define MB_PRIO		(NORMALPRIO+1)	// Modbus process priority

char Buffer[BufferSize+1] = {0};
char* WritePtr = Buffer;
char* ReadPtr = Buffer;
bool bBufferFullFlag = false;
bool bBufferEmptyFlag = true;

bool pipeWrite(char* toWrite){
  /*the WritePtr should always point to a vacant slot before Write*/
  
  /*check the buffer hasn't fully filled*/
  if(bBufferFullFlag == true){
    return false;
  }
    
  memcpy(WritePtr, toWrite, 1);
  bBufferEmptyFlag = false;
  
  /*fill the buffer, and move forward the Ptr*/
  WritePtr++;
  if (WritePtr == (Buffer + BufferSize)){
    WritePtr = Buffer;
  }
  
  /*check whether the buffer is full or not*/
  if (WritePtr == ReadPtr){
    bBufferFullFlag = true;
  }
  
  return true;
}

bool pipeRead(char* toRead){
  if(bBufferEmptyFlag == true){
    return false;
  }
  
  memcpy(toRead, ReadPtr, 1);
  bBufferFullFlag = false;
  
  
  ReadPtr++;
  if(ReadPtr == (Buffer + BufferSize)){
    ReadPtr = Buffer;
  }
  
  if (ReadPtr == WritePtr){
    bBufferEmptyFlag = true;
  }
  return true;
}

bool pipeIsEmpty(void){
  return bBufferEmptyFlag;
}

char* pipeBufferPtr(void){
  return Buffer;
}

/*
================================================
test Thread
================================================
*/
uint8_t Msg = 0;

static THD_WORKING_AREA(waBufferTest, 640);
static THD_FUNCTION(thdBufferTest, arg) {
  while(true){
    sdPut(&SD3,Msg);
    chThdSleepMilliseconds(150);
    chprintf((BaseSequentialStream*)&SD1,"Sent uint8_t: %x \r\n", Msg);
    Msg++;
    Msg %= (255) ;
    chThdSleepMilliseconds(10);
    //chThdSleepSeconds(5);
    if (Msg == 50){
      chprintf((BaseSequentialStream*)&SD1,"Buffer Test Done \r\n", Msg);
      //break;
    }
  }
}
/*
================================================
mRAM test Thread
================================================
*/
bool factor = TRUE;
uint32_t offsetIndex = 0;
static THD_WORKING_AREA(waMR25Test, 320);
static THD_FUNCTION(thdMR25Test, arg) {
  
  uint8_t data = 1;
  uint8_t dataReceived = 0;
  int statue = 0;
  
  
  while(true){
    statue = mr25_data_write(offsetIndex, &data, 1, &mr25);
    chThdSleepMilliseconds(15);
    //chprintf((BaseSequentialStream*)&SD1,"Sent uint8_t: %x \r\n", Msg);
    statue = mr25_data_read(offsetIndex, &dataReceived, 1, &mr25);
    
    if(dataReceived != data) factor = FALSE;
    if(statue != 0 ) factor = FALSE;
    else ;//factor = TRUE;
    
    
    offsetIndex += 1; 
    data = 0;
    data%= (0x11111111) ;
    
    chThdSleepMilliseconds(10);
    
    if(factor == FALSE){
      break;
    }
    
    
    //chThdSleepSeconds(5);
    if (data == 50){
      chprintf((BaseSequentialStream*)&SD1,"Buffer Test Done \r\n", Msg);
      //break;
    }
  }
}



/*
================================================
API
================================================
*/
void BufferTest_init(void){
  chprintf((BaseSequentialStream*)&SD1,"Start buffer test\r\n");
  //chThdCreateStatic(waBufferTest, sizeof(waBufferTest), MB_PRIO, thdBufferTest, NULL);
  chThdCreateStatic(waMR25Test, sizeof(waMR25Test), MB_PRIO, thdMR25Test, NULL);
}
