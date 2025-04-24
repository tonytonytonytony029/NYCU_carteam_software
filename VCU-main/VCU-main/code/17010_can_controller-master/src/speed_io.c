/*
 * speed_io.c
 *
 *  Created on: 2017¦~8¤ë2¤é
 *      Author: Jason
 */

#include "ch.h"
#include "hal.h"
#include "sysParam.h"
#include "hal_ad7124.h"
#include "mbTask.h"
#include "lwipthread.h"
#include "lwip/sockets.h"
#include "lwip/tcpip.h"
#include "thermocouple.h"
#include "sen_htu2xx.h"
#include "filesys.h"
//#include "udpserver.h"
//#include "port.h"

thread_t *thdSpeedIO;
thread_reference_t speedio_trp = NULL;

uint8_t localMACAddr[] = {0xcc,0xdd,0xee,0x55,0x66,0x77};
lwipthread_opts_t lwipOpts = {
  (uint8_t*)localMACAddr,
  0x5901a8c0,
  0x00ffffff,
  0x0101a8c0
};




static THD_WORKING_AREA(waSpeedIO,2048);
static THD_FUNCTION(thSpeedIO ,p)
{
  // start RTC
  static uint16_t nofRecords = 0;
  // mount file system
//  filSysInit();
  /* Resumes the caller and goes to the final priority.*/
  chThdResume(&speedio_trp, MSG_OK);
  systime_t time = chVTGetSystemTimeX();

  if(moduleParam.csvCreationInterval > 0){
    changeLogFile();
  }

  RTCDateTime timespec;
  struct tm tim;
  rtcGetTime(&RTCD1,&timespec);
  rtcConvertDateTimeToStructTm(&timespec,&tim,NULL);
  moduleParam.lastLogTime = mktime(&tim);

  double dv;
  while(1){
    time += MS2ST(1000);
//    dv = sen_htu2xx_read_temp();
//    updateColdT(dv);
//    dv = sen_htu2xx_read_humidity();
//    updateHumidity(dv);

    rtcGetTime(&RTCD1,&timespec);
    rtcConvertDateTimeToStructTm(&timespec,&tim,NULL);
    if(moduleParam.recInterval > 0 && nofRecords < moduleParam.nofLogRecords){
      if((mktime(&tim) - moduleParam.lastLogTime) >= moduleParam.recInterval){
        char str[256];
        int len;
        int i;
        len = sprintf(str,"%04d-%02d-%02d,%02d-%02d-%02d",tim.tm_year+1900,tim.tm_mon, tim.tm_mday, tim.tm_hour,tim.tm_min,tim.tm_sec);
        for(i=0;i<8;i++){
          len += sprintf(&str[len],",%8.3f",anaInData[i].engPV);
        }
        sprintf(&str[len],"\r\n");
        writeRecord(str);
        nofRecords++;
        moduleParam.lastLogTime = mktime(&tim);
      }
    }
    chThdSleepUntil(time);
  }

}


void speedio_init()
{
  mbRTUOpts_t rtuOpts;

  // initial serial port for image download
  sysParamInit();
  //initial tcp/ip stack
  lwipOpts.address = (moduleParam.ip[3]<<24) | (moduleParam.ip[2]<<16) | (moduleParam.ip[1]<<8) | moduleParam.ip[0];
  lwipOpts.gateway = (moduleParam.gateway[3]<<24) | (moduleParam.gateway[2]<<16) | (moduleParam.gateway[1]<<8) | moduleParam.gateway[0];
  lwipOpts.netmask = (moduleParam.mask[3]<<24) | (moduleParam.mask[2]<<16) | (moduleParam.mask[1]<<8) | moduleParam.mask[0];

  lwipOpts.address = 0x0a00a8c0;
  lwipOpts.gateway = 0x0100a8c0;
  lwipOpts.netmask = 0xffffff;
  lwipInit(&lwipOpts);

  switch(moduleParam.baudrate){
  case BAU_9600:
    rtuOpts.baudrate = 9600;
    break;
  case BAU_38400:
    rtuOpts.baudrate = 38400;
    break;
  case BAU_57600:
    rtuOpts.baudrate = 57600;
    break;
  case BAU_115200:
    rtuOpts.baudrate = 115200;
    break;
  }
  // read dip switch (PE8~PE15)
  uint8_t i;
  uint16_t id;
  id = palReadPort(GPIOE);
  id >>= 8;
  id = ~id;

  id = ((id >> 4) & 0xf)*10 + (id & 0xf);

  rtuOpts.dataBits = moduleParam.databits;
  rtuOpts.parity = moduleParam.parity;
  rtuOpts.sla = id;
  rtuOpts.stopBits = moduleParam.stopbits;

  // start modbus task
  mbPortInit(&rtuOpts);

  // start adc task
  halAd7124Init(&SPID1);

  // start speedio task
  thdSpeedIO = chThdCreateStatic(waSpeedIO,sizeof(waSpeedIO),NORMALPRIO,thSpeedIO,NULL);

  // lock until thread finishing initial task
  chSysLock();
  chThdSuspendS(&speedio_trp);
  chSysUnlock();

  // start UDP Server
  updServerInit();


}
