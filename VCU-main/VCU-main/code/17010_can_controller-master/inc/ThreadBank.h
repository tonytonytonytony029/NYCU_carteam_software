#include "ch.h"
#include "hal.h"

/*
==============================================
CAN thread
==============================================
*/

void can_tx_thd_init(void);
void can_rx_thd_init(void);
void can_test(void);

extern CANRxFrame CANRxmsg;


/*
==============================================
Serial thread
==============================================
*/
void RS485_test(void);

/*
==============================================
Buffer
==============================================
*/

bool pipeWrite(char* toWrite);
bool pipeRead(char* toRead);
bool pipeIsEmpty(void);
char* pipeBufferPtr(void);
void BufferTest_init(void);

/*
==============================================
ADC
==============================================
*/
void adc_init(void);

/*
==============================================
dashboard
==============================================
*/

void dashboard_init(void);

/*
==============================================
motor controller
==============================================
*/

void motorController_init(CANRxFrame* p);


/*
==============================================
BMS
==============================================
*/

void BMS_VM_init(CANRxFrame* p);