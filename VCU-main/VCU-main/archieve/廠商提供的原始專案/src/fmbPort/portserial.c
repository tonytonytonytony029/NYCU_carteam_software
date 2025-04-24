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
 * File: $Id: portserial.c,v 1.2 2010/06/06 13:46:42 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include <stdlib.h>
#include "ch.h"
#include "hal.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "port.h"
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define USART_INVALID_PORT      ( 0xFF )
#define USART_NOT_RE_IDX        ( 3 )
#define USART_DE_IDX            ( 4 )
#define USART_IDX_LAST          ( 1 )

#define RX_EN 0x01
#define TX_EN 0x02

extern _mb_instance_t *mbRtu;

/* ----------------------- Static variables ---------------------------------*/
static void txDriverHasRead(UARTDriver *uartp) ;
static void txBufferEmpty(UARTDriver *uartp) ;
static void rxErr(UARTDriver *uartp, uartflags_t e);
static void rxChar(UARTDriver *uartp, uint16_t c);
static void rxEnd(UARTDriver *uartp);

static UCHAR    ucUsedPort = USART_INVALID_PORT;
static UCHAR    oneByteAccum = 0; // should we use a circular buffer ?

#define UARTDRIVER  UARTD1

#ifdef DEBUG_MB
static CHAR lastCharReceived;
CHAR getLastCharReceived (void){
  return lastCharReceived;
}
#endif

void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{ 
  
  USART_TypeDef *u = UARTDRIVER.usart;
  
  if( xRxEnable )  {
    (u->CR1) |=  USART_CR1_RXNEIE;
  }  
  else {
      (u->CR1) &= ~USART_CR1_RXNEIE;
  }
  
  if( xTxEnable )  {
   (u->CR1) |= USART_CR1_TCIE;
   if(mbRtu)
     mbRtu->pxMBFrameCBTransmitterEmpty ();
  } 
  else {
    (u->CR1) &= ~USART_CR1_TCIE;
  }
  
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity  )
{
  BOOL   bStatus = TRUE;
  static UARTConfig uartCfg = {
    txDriverHasRead,
    txBufferEmpty,
    rxEnd,
    rxChar,
    rxErr,
    BAUDRATE,
    0,
    USART_CR2_LINEN,
    0
  };

  uartCfg.speed = ulBaudRate;

  switch(eParity){
  case MB_PAR_NONE:
    break;
  case MB_PAR_ODD:
    uartCfg.cr1 |= (USART_CR1_PCE | USART_CR1_PS);
    break;
  case MB_PAR_EVEN:
    uartCfg.cr1 |= USART_CR1_PCE;
    break;
  default:
    bStatus = FALSE;
  }

  switch(ucDataBits){
  case 8:
    if(eParity != MB_PAR_NONE){
      uartCfg.cr1 |= USART_CR1_M;
    }
    break;
  case 7:
    break;
  default:
    bStatus = FALSE;
  }

  if(bStatus == TRUE){
    uartStart(&UARTDRIVER,&uartCfg);
  }
  ucUsedPort = ucPORT;
  return bStatus;
}

void
vMBPortSerialClose( void )
{
  if (ucUsedPort != USART_INVALID_PORT)   {
    uartStop (&UARTD1);
    ucUsedPort = USART_INVALID_PORT;
  }
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
  CHAR toSend = ucByte;
  
  if (bMBPortIsWithinException() == TRUE) {
    uartStartSendI (&UARTD1, 1, &toSend);      
  } else {
    uartStartSend (&UARTD1, 1, &toSend);
  }
  return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
  *pucByte = oneByteAccum;
  //*pucByte = UARTD1.usart->DR;
  //*pucByte = sdGet(&SD1);
    return TRUE;
}

/* End of transmission buffer callback.   */
static void txDriverHasRead(UARTDriver *uartp){
  (void) uartp;
  
}

/* Physical end of transmission callback.  */
void txBufferEmpty(UARTDriver *uartp){
  (void) uartp;

  chSysLockFromISR();
  vMBPortSetWithinException (TRUE);
  
  
  #ifndef DEBUG_MB
  
    if (mbRtu && mbRtu->pxMBFrameCBTransmitterEmpty () == TRUE)
      rescheduleJbus485FromIsr ();
  #else
      static char charCount = 'a';
    if (charCount < 'z') {
      xMBPortSerialPutByte (charCount++);
    } 
    else {
      charCount = 'a';
      vMBPortSerialEnable (FALSE, FALSE);
    }
  #endif
    
  vMBPortSetWithinException (FALSE);
  chSysUnlockFromISR();
}


/* Receive buffer filled callback.  */
void rxEnd(UARTDriver *uartp){
  (void) uartp;
  
}


/* Character received while out if the UART_RECEIVE state.  */
void rxChar(UARTDriver *uartp, uint16_t c){
  
  (void) uartp;
  (void) c;

  oneByteAccum = (UCHAR) c;

  vMBPortSetWithinException (TRUE);
  chSysLockFromISR(); {
#ifdef DEBUG_MB
    xMBPortSerialGetByte (&lastCharReceived);
#else
    if(mbRtu)
      mbRtu->pxMBFrameCBByteReceived ();
    rescheduleJbus485FromIsr ();
#endif   
  } 
  chSysUnlockFromISR();
  vMBPortSetWithinException (FALSE);
  

  
}


/* Receive error callback.  */
void rxErr(UARTDriver *uartp, uartflags_t e){
  (void) uartp;
  (void) e;
  
  chSysLockFromISR(); 
  if (e & USART_SR_PE) {
    //syslogErrorFromISR ("parity err");
  } else if (e & USART_SR_FE) {
    //syslogErrorFromISR ("framing err");
  } if (e & USART_SR_NE) {
    //syslogErrorFromISR ("noise err");
  } if (e & USART_SR_ORE) {
    //syslogErrorFromISR ("overrun err");
  }  if (e & USART_SR_IDLE) {
    //syslogErrorFromISR ("idle line err");
  } else {
    //syslogErrorFromISR ("uart rx err");
  }
  chSysUnlockFromISR();
  
  
}



