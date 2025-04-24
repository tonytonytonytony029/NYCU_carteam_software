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


systime_t startTime,endTime;
/* ----------------------- Static variables ---------------------------------*/

//static WORKING_AREA(waTimerIrq,128);
//static msg_t threadTimer(void *arg);
//static Thread *timerThread;
 

static void timeout_cb(void *arg)
{
  mb_struct_t *curmb = (mb_struct_t*)arg;
  mb_struct_t *mb = mbRoot;
  endTime = chVTGetSystemTimeX();
  while(mb){
    if(curmb == mb){
      switch(curmb->mode){
      case MB_RTU:
        //if(curmb->role == MB_SLAVE)
          xMBRTUTimerT35Expired(mb);
        //else
          //xMBMasterRTUTimerT35Expired(mb);
        break;
      case MB_ASCII:
        xMBASCIITimerT1SExpired(mb);
        break;
      }
      break;
    }
    mb = mb->next;
  }
  
}

/* ----------------------- Start implementation -----------------------------*/


BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us , mb_struct_t *mb)
{
  chVTObjectInit(&mb->vt);
  mb->ulTimeoutInUs = usTim1Timerout50us * 100;
  
  return TRUE;
}


void 
vMBPortTimerClose( void )
{
  
}

void vMBPortTimerPoll(mb_struct_t *mb)
{
  uint32_t ulDeltaMS;
  
  
}


void
vMBPortTimersEnable(mb_struct_t *mb )
{
  // reset timer first to avoid 
  chVTReset(&mb->vt);
  //startTime = chVTGetSystemTimeX();
  if(mb->role == MB_SLAVE)
    chVTSet(&mb->vt,US2ST(mb->ulTimeoutInUs),timeout_cb,mb);
  else{
    if(mb->masterState.waitResponse)
      chVTSet(&mb->vt,MS2ST(mb->masterState.timeout),timeout_cb,mb);
    else
      chVTSet(&mb->vt,US2ST(mb->ulTimeoutInUs),timeout_cb,mb);
  }
}


void
vMBPortTimersDisable( mb_struct_t *mb )
{
  chVTReset(&mb->vt);
}

void
vMBPortTimersDisableI( mb_struct_t *mb )
{
  chVTResetI(&mb->vt);
}


void
vMBPortTimersDelay( USHORT usTimeOutMS )
{
  chThdSleepMilliseconds(1);
}

