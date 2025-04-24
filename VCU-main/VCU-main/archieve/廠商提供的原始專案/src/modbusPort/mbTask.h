#ifndef _MBTASK_H
#define _MBTASK_H
#include "ch.h"
#include "hal.h"
#include "port.h"

extern thread_t *thdModbus;
extern thread_t *thdMasterModbus;

#define EV_MBM_READ     EVENT_MASK(1)



typedef int8_t(*modbus_reg_decoder)(uint8_t, uint8_t*);

typedef struct modbus_decoder_s{
//  struct modbus_decoder_s *next;
  uint8_t loBound;
  uint8_t hiBound;
  modbus_reg_decoder reader;
  modbus_reg_decoder writer;
}modbus_decoder_t;

int mbPortInit(mbRTUOpts_t *opts);

typedef struct _mbMaster_request_s{
  uint8_t port;
  uint8_t slaveAddr;
  uint8_t funcCode;
  uint16_t startAddr;
  uint8_t nofReg;
}mbMaster_request_t;



#endif
