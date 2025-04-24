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


/* ----------------------- Static variables ---------------------------------*/

USART_TypeDef *getDriver(uint8_t id)
{
  switch(id){
#if STM32_UART_USE_USART1
  case 1:
    return UARTD1.usart;
#endif
#if STM32_UART_USE_USART2
  case 2:
    return UARTD2.usart;
#endif
#if STM32_UART_USE_USART3
  case 3:
    return UARTD3.usart;
#endif
#if STM32_UART_USE_USART4
  case 4:
    return UARTD4.usart;
#endif
  }
}

static BOOL prvbMBPortSerialRead(uint8_t *pucBuffer,uint16_t usNBytes,uint16_t *usNBytesRead, mb_struct_t *mb)
{
  BOOL bStatus = TRUE;
  char *dptr = pucBuffer;
  char c;
  uint16_t res;
                      
  if((res = sdAsynchronousRead(mb->driver,(uint8_t*)pucBuffer,usNBytes)) == 0){
    *usNBytesRead = 0;
    bStatus = FALSE;
  }else{
    *usNBytesRead = res;
  }

  return bStatus;
}

static BOOL prvbMBPortSerialWrite(uint8_t *pucBuffer, uint16_t usNBytes, mb_struct_t *mb)
{
  BOOL bStatus = TRUE;
  char *dptr = pucBuffer;
  uint16_t left = usNBytes;

  sdAsynchronousWrite(mb->driver,pucBuffer,usNBytes);
  //  while(left>0){
//
//  }

  return bStatus;
}

void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable,mb_struct_t *mb )
{ 
  
  USART_TypeDef *u;// = UARTDRIVER.usart;

  //u = getDriver(mb->port);
  u = mb->driver->usart;
  if( xRxEnable )  {

    //(u->CR1) |=  USART_CR1_RXNEIE;
    mb->bEnableRx = true;
  }  
  else {
      //(u->CR1) &= ~USART_CR1_RXNEIE;
      mb->bEnableRx = false;
  }
  
  if( xTxEnable )  {
    mb->bEnableTx = true;
   //(u->CR1) |= USART_CR1_TCIE;
//   switch(mb->mode){
//   case MB_RTU:
//     (void)xMBRTUTransmitFSM(mb);
//     break;
//   }
  } 
  else {
    //(u->CR1) &= ~USART_CR1_TCIE;
    mb->bEnableTx = false;
  }
  
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity, mb_struct_t *mb )
{
  BOOL   bStatus = TRUE;
  SerialConfig scfg;
  scfg.speed = ulBaudRate;

  USART_TypeDef *u;
//  uartCfg.speed = ulBaudRate;


  switch(eParity){
  case MB_PAR_NONE:
    break;
  case MB_PAR_ODD:
    scfg.cr1 |= (USART_CR1_PCE | USART_CR1_PS);
    break;
  case MB_PAR_EVEN:
    scfg.cr1 |= USART_CR1_PCE;
    break;
  default:
    bStatus = FALSE;
  }

  switch(ucDataBits){
  case 8:
    if(eParity != MB_PAR_NONE){
      scfg.cr1 |= USART_CR1_M;
    }
    break;
  case 7:
    break;
  default:
    bStatus = FALSE;
  }

  if(bStatus == TRUE){
    sdStart(mb->driver,&scfg);
  }
  return bStatus;
}

void
vMBPortSerialClose( mb_struct_t *mb )
{
  sdStop(mb->driver);
}

BOOL xMBPortSerialSetTimeout(uint32_t timeoutMS, mb_struct_t *mb)
{
  if(timeoutMS > 0){
    mb->ulTimeoutMs = timeoutMS;
  }else{
    mb->ulTimeoutMs = 1;
  }
  return TRUE;
}


BOOL
xMBPortSerialPutByte( CHAR ucByte, mb_struct_t *mb)
{
//  mb->serial->rxBuf[mb->serial->usSndBufferPos++] = ucByte;
  mb->c = ucByte;
  return TRUE;
}

BOOL
xMBPortSerialWrite(mb_struct_t *mb)
{
          if(mb->usSndBufferCount > 0){
            sdWrite(mb->driver,mb->buffer,mb->usSndBufferCount);
            mb->usSndBufferCount = 0;
          }
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte, mb_struct_t *mb )
{
  *pucByte = mb->c;
    return TRUE;
}



BOOL
xMBPortSerialPoll(mb_struct_t *mb)
{
  BOOL bStatus = FALSE;
  uint16_t usBytesRead;
  int i;
  if(mb->bEnableRx){
    while(sdAsynchronousRead(mb->driver,(uint8_t*)&mb->c,1) == 1){
//    if(prvbMBPortSerialRead(mb->serial->rxBuf,256,&usBytesRead,mb)){
      switch(mb->mode){
      case MB_RTU:
        (void)xMBRTUReceiveFSM(mb);
        break;
      case MB_ASCII:
        (void)xMBASCIIReceiveFSM(mb);
      }
    }
  }
  if(mb->bEnableTx){
    if(mb->usSndBufferCount > 0){
      switch(mb->mode){
      case MB_RTU:
        while(!xMBRTUTransmitFSM(mb)){
          sdPut(mb->driver,mb->c);
        }
        break;
      case MB_ASCII:
        while(!xMBASCIITransmitFSM(mb)){
          sdPut(mb->driver,mb->c);
        }
        break;
      }
    }
//    if(mb->usSndBufferCount > 0){
//      while(mb->usSndBufferCount>0){
//        switch(mb->mode){
//        case MB_RTU:
//          (void)xMBRTUTransmitFSM(mb);
//          break;
//        case MB_ASCII:
//          (void)xMBASCIITransmitFSM(mb);
//          break;
//        }
//        sdPut(mb->driver,mb->c);
//      }
//    }
//    else{
//      switch(mb->mode){
//      case MB_RTU:
//        (void)xMBRTUTransmitFSM(mb);
//        break;
//      case MB_ASCII:
//        while(!xMBASCIITransmitFSM(mb)){
//          sdPut(mb->driver,mb->c);
//        }
////        if(!mb->masterState.waitResponse)
//        break;
//      }
//    }
  }
}

