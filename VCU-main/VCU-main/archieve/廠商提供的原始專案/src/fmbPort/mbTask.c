#include "ch.h"
#include "hal.h"
#include "mb.h"
#include "mbTask.h"
#include "port.h"
#include "lwip/api.h"
#include "lwip/tcp.h"
#include "lwip/sockets.h"
#include "sysParam.h"

#define MB_TCP_UID      6
#define MB_TCP_LEN      4
#define MB_TCP_FUNC      7

#define MB_TCP_DEFAULT_PORT     502
#define MB_TCP_BUF_SIZE (256+7)

thread_t *thdModbus;
thread_reference_t modbus_trp = NULL;

#define MB_RTU_EN   1
#define MB_TCP_EN   1

uint16_t byteSwap(uint16_t v)
{
  uint16_t vret;
  vret = ((v&0xff)<<8) | (v >> 8);

  return vret;
}


eMBErrorCode eMBRegInputCB(UCHAR *pucRegBuffer,USHORT usAddress,USHORT usNRegs)
{
  return MB_ENOREG;
}

eMBErrorCode eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress,USHORT usNRegs, eMBRegisterMode eMode)
{
  eMBErrorCode eStatus = MB_ENOERR;
  uint8_t i;
  uint16_t regVal;
  USHORT usAddr = usAddress - 1;
  uint8_t ch;
  uint8_t index;
  uint8_t szRet;
  double dv;
  if(eMode == MB_REG_READ){
    for(i=0;i<usNRegs;i++){
      szRet=sysReadMBMap(usAddr,pucRegBuffer);
      usAddr+=szRet;
      if(szRet == 2) i++;
      pucRegBuffer += (szRet<<1);
    }
  }else if(eMode == MB_REG_WRITE){
    for(i=0;i<usNRegs;i++){
      szRet =sysWriteMBMap(usAddr,pucRegBuffer);
      usAddr+=szRet;
      if(szRet == 2) i++;
      pucRegBuffer += (szRet<<1);
    }
    // issue write operation
    //chEvtSignal(thdSpeedIO,
    sysSaveParam();
  }
  
  return eStatus;
}
extern struct netconn *serverconn;

static THD_WORKING_AREA(waMBTask,2048);
static THD_FUNCTION(thMBTask ,p)
{
  mbRTUOpts_t *opt = (mbRTUOpts_t*)p;
  chRegSetThreadName("MODBUS_Thread");
  
  if(MB_RTU_EN){
    if(!opt){
      if(eMBInit(MB_RTU,1,0,9600,MB_PAR_NONE) == MB_ENOERR){
        eMBEnable(0);
      }
    }else{
      if(eMBInit(MB_RTU,opt->sla,0,opt->baudrate,opt->parity) == MB_ENOERR){
        eMBEnable(0);
      }
    }
  }
  
  if(MB_TCP_EN){
    if(eMBTCPInit(502) == MB_ENOERR){
      eMBEnable(1);
    }
  }
  /* Resumes the caller and goes to the final priority.*/
  chThdResume(&modbus_trp, MSG_OK);
  while(true){
    if(MB_RTU_EN){
      //chSysLock();
      eMBPoll(0);
      //chSysUnlock();
      //chThdSleepMilliseconds(10);
    }
    if(MB_TCP_EN){
      //chSysLock();
      eMBPoll(1);
      //chSysUnlock();
      //chThdSleepMilliseconds(10);
    }
    chThdSleepMilliseconds(5);
  }
  
}

int mbPortInit(mbRTUOpts_t *opts)
{
  
  thdModbus = chThdCreateStatic(waMBTask,sizeof(waMBTask),NORMALPRIO,thMBTask,opts);
  // lock until thread finishing initial task
  chSysLock();
  chThdSuspendS(&modbus_trp);
  chSysUnlock();

  return 0;
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}
