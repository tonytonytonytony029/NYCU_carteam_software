#include <string.h>
#include <stdlib.h>

#include "ch.h"
#include "hal.h"

#include "port.h"
#include "mbconfig.h"
#include "user_mb_app.h"

#include "ThreadBank.h"

#define MB_PORT			1				// NU
#define MB_MODE			MB_RTU			// RTU mode only
#define MB_BITRATE		115200			// bitrate
#define MB_PARITY		MB_PAR_EVEN		// parity
#define WAIT_FOREVER	(-1)
#define MB_PRIO		(NORMALPRIO+1)	// Modbus process priority

extern USHORT   usMDiscInStart;
#if      M_DISCRETE_INPUT_NDISCRETES%8
extern UCHAR    ucMDiscInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_DISCRETE_INPUT_NDISCRETES/8+1];
#else
extern UCHAR    ucMDiscInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_DISCRETE_INPUT_NDISCRETES/8];
#endif
extern USHORT   usMCoilStart;
#if      M_COIL_NCOILS%8
extern UCHAR    ucMCoilBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8+1];
#else
extern UCHAR    ucMCoilBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8];
#endif
extern USHORT   usMRegInStart;
extern USHORT   usMRegInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS];
extern USHORT   usMRegHoldStart;
extern USHORT   usMRegHoldBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS];
USHORT  usModbusUserData[MB_PDU_SIZE_MAX];
UCHAR   ucModbusUserData[MB_PDU_SIZE_MAX];


/*
==============================================
Modbus Test thread
==============================================
*/

static THD_WORKING_AREA(waMBTest, 256);
static THD_FUNCTION(thdMBTest, arg) {
  (void)arg;
  chprintf((BaseSequentialStream*)&SD1,">>Start MB_FunctionCode test\r\n");
  chThdSleepMilliseconds(20);
  
  eMBMasterReqErrCode    errorCode = MB_MRE_NO_ERR;
  uint16_t errorCount = 0;

  chRegSetThreadName("MBTest");

  while (true)
  {
	  //Test Modbus Master
	  usModbusUserData[0] = (USHORT) (chVTGetSystemTimeX() / 10);
	  usModbusUserData[1] = (USHORT) (chVTGetSystemTimeX() % 10);
          
          chThdSleepSeconds(1);
          chprintf((BaseSequentialStream*)&SD1,"eMBMasterReqReadDiscreteInputs\r\n");
          chThdSleepMilliseconds(20);
	  errorCode = eMBMasterReqReadDiscreteInputs(1,0,8,WAIT_FOREVER);
	  (void) ucMDiscInBuf[0];

          chThdSleepSeconds(1);
          chprintf((BaseSequentialStream*)&SD1,"eMBMasterReqWriteCoil\r\n");
          chThdSleepMilliseconds(20);
          errorCode = eMBMasterReqWriteCoil(1,0,0xFF00,WAIT_FOREVER);
          
          chThdSleepSeconds(1);
          chprintf((BaseSequentialStream*)&SD1,"eMBMasterReqReadCoil\r\n");
          chThdSleepMilliseconds(20);
	  errorCode = eMBMasterReqReadCoils(1,0,1,WAIT_FOREVER);
	  (void) ucMCoilBuf[0];

          chThdSleepSeconds(1);
          chprintf((BaseSequentialStream*)&SD1,"eMBMasterReqReadInputRegister\r\n");
          chThdSleepMilliseconds(20);
	  errorCode = eMBMasterReqReadInputRegister(1,0,2,WAIT_FOREVER);
	  (void) usMRegInBuf[0];

          chThdSleepSeconds(1);
          chprintf((BaseSequentialStream*)&SD1,"eMBMasterReqWriteHoldingRegister\r\n");
	  errorCode = eMBMasterReqWriteHoldingRegister(1,0,usModbusUserData[0],WAIT_FOREVER);
	  errorCode = eMBMasterReqWriteHoldingRegister(1,1,usModbusUserData[1],WAIT_FOREVER);

          chThdSleepSeconds(1);
          chprintf((BaseSequentialStream*)&SD1,"eMBMasterReqReadHoldingRegister\r\n");
	  errorCode = eMBMasterReqReadHoldingRegister(1,0,2,WAIT_FOREVER);
	  (void) usMRegHoldBuf[0];

//      errorCode = eMBMasterReqWriteMultipleCoils(1,3,5,ucModbusUserData,WAIT_FOREVER);
//		errorCode = eMBMasterReqWriteMultipleHoldingRegister(1,3,2,usModbusUserData,WAIT_FOREVER);
//		errorCode = eMBMasterReqReadWriteMultipleHoldingRegister(1,3,2,usModbusUserData,5,2,WAIT_FOREVER);
	  if (errorCode != MB_MRE_NO_ERR) {
		  errorCount++;
	  }
	  chThdSleepSeconds(30);
          chThdSleepMilliseconds(1000);
  }
}

/*
================================================
Modbus main Thread
================================================
*/

bool initModbus(void) {
  eMBErrorCode eStatus;
  eStatus = eMBMasterInit(MB_MODE, MB_PORT, MB_BITRATE, MB_PARITY);
  if (eStatus != MB_ENOERR) {
	  return false;
  }

  eStatus = eMBMasterEnable();
  if (eStatus != MB_ENOERR) {
	  return false;
  }

  return true;
}



static THD_WORKING_AREA(waThdModbus, 256);
static THD_FUNCTION(thdModbus, arg) {
  (void)arg;

  chprintf((BaseSequentialStream*)&SD1,">>Start MB_main\r\n");
  chThdSleepMilliseconds(20);
  
  chRegSetThreadName("MODBUS");

  while (initModbus() != TRUE) {
	chThdSleepMilliseconds(1000);

	if (chThdShouldTerminateX())
	  goto cleanAndExit;
  }

  do {
    eMBMasterPoll();
  } while (!chThdShouldTerminateX());

cleanAndExit:
  eMBMasterDisable();
  eMBMasterClose();
}

/*
================================================
Modbus byte-wise data process Thread
================================================
*/
char oneByteBuffer_1 = 0;
static THD_WORKING_AREA(waMB_Process, 128);
static THD_FUNCTION(thdMB_Process, arg) {
  (void)arg;
  chprintf((BaseSequentialStream*)&SD1,">>Start MB_processw\r\n");
  chThdSleepMilliseconds(20);
  
  while(true){
    //chThdSleepSeconds(5);
    chThdSleepMilliseconds(50);
    if (pipeRead(&oneByteBuffer_1) == true){
      mrxChar(oneByteBuffer_1);
    }
  }
}

char oneByteBuffer_2 = 0;
static THD_WORKING_AREA(waMB_Read, 128);
static THD_FUNCTION(thdMB_Read, arg) {
  (void)arg;
  chprintf((BaseSequentialStream*)&SD1,">>Start MB_read\r\n");
  chThdSleepMilliseconds(20);
  while(true){
    sdRead(&SD2, &oneByteBuffer_2, 1);
    //pipeWrite(&oneByteBuffer_2);
    
    mrxChar(oneByteBuffer_2);
  }
}

/*
================================================
Modbus API
================================================
*/
void modbus_master_init(void) {
  
  chThdCreateStatic(waThdModbus, sizeof(waThdModbus), MB_PRIO, thdModbus, NULL);
  
  //chThdCreateStatic(waMB_Process, sizeof(waMB_Process), MB_PRIO, thdMB_Process, NULL);
  
  chThdCreateStatic(waMB_Read, sizeof(waMB_Read), MB_PRIO, thdMB_Read, NULL);
}

void modbus_MasterFunctionCode_test(void) {
  chThdCreateStatic(waMBTest, sizeof(waMBTest), MB_PRIO, thdMBTest, NULL);
}

/*
===========================================
Othe Stuff
===========================================
*/

// called on kernel panic
void halt(void){
	port_disable();
	while(TRUE)
	{
	}
}
