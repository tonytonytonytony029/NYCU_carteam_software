/*
 * FreeModbus Libary: Atmel STM32 Demo Application
 * Copyright (C) 2010 Christian Walter <cwalter@embedded-solutions.at>
 *
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * IF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: porttimer.c,v 1.2 2010/06/06 13:46:42 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "ch.h"
#include "hal.h"
#include <stdlib.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "port.h"
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define GPT_BASE_CLOCK 1000000

#ifdef DEBUG_MB
#include "tm.h"
static TimeMeasurement tm;
#endif

static void gpt_callback(GPTDriver *gptp);     //timerHandler
extern _mb_instance_t *mbRtu;
/* ----------------------- Static variables ---------------------------------*/

//static WORKING_AREA(waTimerIrq,128);
//static msg_t threadTimer(void *arg);
//static Thread *timerThread;
 
static GPTConfig gptcfg={
  GPT_BASE_CLOCK,                // GPT_BASE_CLOCK 18000000
  gpt_callback
};

uint32_t gptInterval;   //timerout


/* ----------------------- Start implementation -----------------------------*/
static void gpt_callback(GPTDriver *gptp)          
{
  (void)gptp;
  
  #ifdef DEBUG_MB
    tmStopMeasurement (&tm);
  #endif
  
  chSysLockFromISR(); 
  vMBPortSetWithinException (TRUE) ;
   
  if (mbRtu && mbRtu->pxMBPortCBTimerExpired () == TRUE)     //// pxMBPortCBTimerExpired = xMBRTUTimerT35Expired;
      rescheduleJbus485FromIsr();

  vMBPortSetWithinException (FALSE) ;
  chSysUnlockFromISR();
}



BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
  gptInterval = usTim1Timerout50us*50;          //MHz*50uS=900
  gptInit();
  gptStart(&GPTD1,&gptcfg);                      //General Purpose Timer,GPTD1:TIM1 
  
  //gptStartContinuous(&GPTD1,gptInterval);      
  //gptStartOneShot(&GPTD1,gptInterval);     
  //timerThread = chThdCreateStatic(waTimerIrq,sizeof(waTimerIrq),NORMALPRIO,threadTimer,0);
  
  #ifdef DEBUG_MB
  tmObjectInit (&tm);
  #endif
  
  return TRUE;
}


void 
vMBPortTimerClose( void )
{
  gptStop(&GPTD1);
}


void
vMBPortTimersEnable(  )
{
 //if(dbg_isr_cnt !=0){chEvtSignalI(timerThread,(eventmask_t)2);   }
 //else{chEvtSignal(timerThread,(eventmask_t)2);    }
 
#ifdef DEBUG_MB
  tmStartMeasurement (&tm);
#endif
  
  //if(dbg_isr_cnt !=0){
  if (bMBPortIsWithinException() == TRUE) {
    gptStopTimerI (&GPTD1);
    gptStartOneShotI(&GPTD1,gptInterval);      
  }
  else{
    gptStopTimer (&GPTD1);
    gptStartOneShot(&GPTD1,gptInterval);
  }
 
}


void
vMBPortTimersDisable(  )
{
  //if(dbg_isr_cnt !=0){chEvtSignalI(timerThread,(eventmask_t)4);     }
  //else{chEvtSignal(timerThread,(eventmask_t)4);     }

  //if(dbg_isr_cnt !=0){
  if (bMBPortIsWithinException() == TRUE){
    gptStopTimerI (&GPTD1);  
  }
  else{
    gptStopTimer (&GPTD1);
  }
}



void
vMBPortTimersDelay( USHORT usTimeOutMS )
{
  chThdSleepMilliseconds(1);
}





/*
static msg_t threadTimer(void *arg)
{
  int j;
  eventmask_t ret;
  while(1){
    ret = chEvtWaitAny(ALL_EVENTS);
    
    switch(ret){
    case 1:
      j = 1;
      chEvtGetAndClearEvents((eventmask_t)1);
      ( void )pxMBPortCBTimerExpired(  );  
      break;
    case 2:
      j = 1;
      chEvtGetAndClearEvents((eventmask_t)2);
      gptStartOneShot(&GPTD1,gptInterval);
      break;
    case 3:
      j = 1;
      chEvtGetAndClearEvents((eventmask_t)4);
      gptStopTimer(&GPTD1);
      break;
    }
    //chThdSleepMilliseconds(1);
  }
}
*/
