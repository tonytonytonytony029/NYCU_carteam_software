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

#include "VCU.h"

appParam_t appParam;
uint32_t RS485_loop_counter = 0;
uint32_t RS485_state = 0;
uint32_t RxSizeZeroFlag = 0;
char Rx_msg_in[32] = 0;

#define LED_ON()        palSetPad(GPIOC,12)
#define LED_OFF()       palClearPad(GPIOC,12)

#define VOUT_12V_ON()        palClearPad(GPIOD,1)
#define VOUT_12V_OFF()       palSetPad(GPIOD,1)
#define VOUT_5V_ON()        palClearPad(GPIOD,2)
#define VOUT_5V_OFF()       palSetPad(GPIOD,2)


#define NOF_DIG_IN      10
#define NOF_DIG_OUT     11

static digital_io_type_t dig_in[] = {
  {DIO_DIR_INPUT,DIO_TYPE_GPIO,0,0,0,GPIOD,12},
  {DIO_DIR_INPUT,DIO_TYPE_GPIO,0,0,0,GPIOD,13},
  {DIO_DIR_INPUT,DIO_TYPE_GPIO,0,0,0,GPIOD,14},
  {DIO_DIR_INPUT,DIO_TYPE_GPIO,0,0,0,GPIOD,15},
  {DIO_DIR_INPUT,DIO_TYPE_GPIO,0,0,0,GPIOC,6},
  {DIO_DIR_INPUT,DIO_TYPE_GPIO,0,0,0,GPIOC,7},
  {DIO_DIR_INPUT,DIO_TYPE_GPIO,0,0,0,GPIOC,8},
  {DIO_DIR_INPUT,DIO_TYPE_GPIO,0,0,0,GPIOC,9},
  {DIO_DIR_INPUT,DIO_TYPE_GPIO,0,0,0,GPIOB,7},
  {DIO_DIR_INPUT,DIO_TYPE_GPIO,0,0,0,GPIOD,3},
};

static digital_io_type_t dig_out[] = {
  {DIO_DIR_OUTPUT,DIO_TYPE_GPIO,0,0,0,GPIOE,8},
  {DIO_DIR_OUTPUT,DIO_TYPE_GPIO,0,0,0,GPIOE,9},
  {DIO_DIR_OUTPUT,DIO_TYPE_GPIO,0,0,0,GPIOE,10},
  {DIO_DIR_OUTPUT,DIO_TYPE_GPIO,0,0,0,GPIOE,11},
  {DIO_DIR_OUTPUT,DIO_TYPE_GPIO,0,0,0,GPIOE,12},
  {DIO_DIR_OUTPUT,DIO_TYPE_GPIO,0,0,0,GPIOE,13},
  {DIO_DIR_OUTPUT,DIO_TYPE_GPIO,0,0,0,GPIOE,14},
  {DIO_DIR_OUTPUT,DIO_TYPE_GPIO,0,0,0,GPIOE,15},  
  {DIO_DIR_OUTPUT,DIO_TYPE_GPIO,0,0,0,GPIOE,1},  
  {DIO_DIR_OUTPUT,DIO_TYPE_GPIO,0,0,0,GPIOE,0},  
  {DIO_DIR_OUTPUT,DIO_TYPE_GPIO,0,0,0,GPIOD,2},  
};

void wukp_int_handler(EXTDriver *extp, expchannel_t channel);
void accsens_int_handler(EXTDriver *extp, expchannel_t channel);

static const EXTConfig extcfg = {
  {
    {EXT_CH_MODE_DISABLED, NULL}, 
    {EXT_CH_MODE_DISABLED, NULL}, 
    {EXT_CH_MODE_DISABLED, NULL}, 
    {EXT_CH_MODE_FALLING_EDGE | EXT_MODE_GPIOD, accsens_int_handler},
    {EXT_CH_MODE_DISABLED, NULL}, 
    {EXT_CH_MODE_DISABLED, NULL}, 
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_FALLING_EDGE | EXT_MODE_GPIOB, wukp_int_handler},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
  }
};

// CAN config for 500K baud

static CANConfig canCfg500K = {
  CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  CAN_BTR_BRP(6) | CAN_BTR_TS1(8) | CAN_BTR_TS2(1) | CAN_BTR_SJW(0)
};

// can config for 250K baud
static CANConfig canCfg250K = {
  CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  CAN_BTR_BRP(20) | CAN_BTR_TS1(5) | CAN_BTR_TS2(0) | CAN_BTR_SJW(0)
};

static const I2CConfig i2ccfg_ad5593 = {
  OPMODE_I2C,
  100000,
  STD_DUTY_CYCLE,
};

int16_t ad0Result[8];
int16_t ad1Result[8];

static AD5593RConfig adc_cfg0 = {
  &I2CD1,
  &i2ccfg_ad5593,
  AD5593_DEV1,
  0x0320,
  0xff,
  0x00,
  NULL,
  VIO_VREF
};

static AD5593RConfig adc_cfg1 = {
  &I2CD1,
  &i2ccfg_ad5593,
  AD5593_DEV1,
  0x0320,
  0xff,
  0x00,
  NULL,
  VIO_2VREF
};

// shell command on UART1
static THD_WORKING_AREA(waShell,1024);
static void cmdRead(BaseSequentialStream *chp, int argc, char *argv[]);
static void cmdWrite(BaseSequentialStream *chp, int argc, char *argv[]);
static void cmdView(BaseSequentialStream *chp, int argc, char *argv[]);
static void cmdSend(BaseSequentialStream *chp, int argc, char *argv[]);
static void cmdEcho(BaseSequentialStream *chp, int argc, char *argv[]);
static void cmdRTC(BaseSequentialStream *chp, int argc, char *argv[]);
static void cmdDC(BaseSequentialStream *chp, int argc, char *argv[]);
static void cmdDigit(BaseSequentialStream *chp, int argc, char *argv[]);
static void rs485_loop_test(BaseSequentialStream *chp, int argc, char *argv[]);
// static void can_loop_test(BaseSequentialStream *chp, int argc, char *argv[]);
static void adc_test(BaseSequentialStream *chp, int argc, char *argv[]);


static const ShellCommand commands[] = {
  {"di_read",cmdRead},
  {"do_write",cmdWrite},
  {"ai_read",cmdView},
  {"send",cmdSend},
  {"ech",cmdEcho},
  {"rtc",cmdRTC},
  {"dc",cmdDC},
  {"digit",cmdDigit},
  {"loop_rs485", rs485_loop_test},
//  {"loop_can",can_loop_test},
  {"adc",adc_test},
  {NULL,NULL}
};

static const ShellConfig shell_cfg = {
  (BaseSequentialStream*)&SD1,
  commands
};
// end shell command


AD5593RDriver ad_dev0,ad_isens;
thread_t *thdVMU;
thread_t *can1Recv, *can2Recv;
thread_reference_t vmu_ref;

static msg_t canSend(CANDriver *p, j1939_msg_t *m);

static THD_WORKING_AREA(waRs485Loop,2048);

//static THD_WORKING_AREA(waCANRX1,512);
//static THD_WORKING_AREA(waCANRX2,512);
//static THD_WORKING_AREA(waRS485_C1,512);
//static THD_WORKING_AREA(waRS485_C2,512);
static THD_FUNCTION(thCanRx,p)
{
  (void)p;
  CANDriver *ip = p;
  CANRxFrame rxmsg;
  CANTxFrame txmsg;
  event_listener_t el;
  j1939_msg_t m;
  uint8_t i;
  //chRegSetThreadName("CANRx_01");
  
  chEvtRegister(&ip->rxfull_event,&el,1);
  while(true){
    if(chEvtWaitAnyTimeout(ALL_EVENTS,MS2ST(100)) == 0){
      continue;
    }
    
    while(canReceive(ip,CAN_ANY_MAILBOX,&rxmsg,TIME_IMMEDIATE) == MSG_OK){
      txmsg.DLC = rxmsg.DLC;
      txmsg.EID = rxmsg.EID;
      txmsg.IDE = rxmsg.IDE;
      txmsg.RTR = CAN_RTR_DATA;
      
      for(uint8_t i=0;i<rxmsg.DLC;i++){
        txmsg.data8[i] = rxmsg.data8[i] ;//+ 0x11;
      }
      
      canTransmit(ip,CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
      
//      m.pgn = (rxmsg.EID >> 8) & 0x1ffff;
//      m.src = (rxmsg.EID & 0xff);
//      m.len = rxmsg.DLC;
//      memcpy(m.buffer,rxmsg.data8,rxmsg.DLC);
      
//      if(ip == &CAND1){
//        appParam.msg1.pgn = (rxmsg.EID >> 8) & 0x1ffff;
//        appParam.msg1.src = (rxmsg.EID & 0xff);
//        appParam.msg1.len = rxmsg.DLC;
//        memcpy(appParam.msg1.buffer,rxmsg.data8,rxmsg.DLC);
//        if(appParam.can_tx_thread)
//          chEvtSignal(appParam.can_tx_thread,EVT_CAN1_RECEIVE);
//      }
//      if(ip == &CAND2){
//        appParam.msg2.pgn = (rxmsg.EID >> 8) & 0x1ffff;
//        appParam.msg2.src = (rxmsg.EID & 0xff);
//        appParam.msg2.len = rxmsg.DLC;
//        memcpy(appParam.msg2.buffer,rxmsg.data8,rxmsg.DLC);
//        if(appParam.can_tx_thread)
//          chEvtSignal(appParam.can_tx_thread,EVT_CAN2_RECEIVE);
//      }
//      if(ip == &CAND1 && (appParam.testState.echo_canbus & 0x1)){
//        char msg[64];
//        char *ptr = msg;
//        char *q = (char*)&m;
//        for(uint8_t i=0;i<16;i++){
//          ptr += chsnprintf(ptr,64,"%02x:",*q++);
//        }
//        chprintf(&SD1,"CAN 1:%s\n",msg);
////        sdWrite(&SD1,"CAN1:",4);
////        sdWrite(&SD1,(uint8_t*)&m,16);
//      }
//      else if(ip == &CAND2 && (appParam.testState.echo_canbus & 0x2)){
//        char msg[64];
//        char *ptr = msg;
//        char *q = (char*)&m;
//        for(uint8_t i=0;i<16;i++){
//          ptr += chsnprintf(ptr,64,"%x ",*q++);
//        }
//        chprintf(&SD1,"CAN 2:%s\n",msg);
//      }
      
    }
  }
  chEvtUnregister(&ip->rxfull_event,&el);
}

double ntcTemp[4];
int IDR_buffer = 0;

static THD_WORKING_AREA(waVMU,1024);
static THD_FUNCTION(thVMU ,p)
{
  uint8_t led = 0;
  
  j1939_msg_t msg;
  msg.dst = 0x1234;
  msg.pgn = 100;
  msg.src =  0x5678;
  msg.buffer[0] = 0x11;
  msg.buffer[1] = 0x22;
  msg.buffer[2] = 0x33;
  msg.buffer[3] = 0x44;
  msg.buffer[4] = 0x55;
  msg.buffer[5] = 0x66;
  msg.buffer[6] = 0x77;
  msg.buffer[7] = 0x88;
  msg.len = 8;
  // canSend(&CAND2,&msg);
  
  while(true)
  {
    if(led == 0){
      LED_ON();
      led = 1;
    }
    else{
      LED_OFF();
      led = 0;
    }
    ad5593rRead(&ad_dev0);
    ad5593rRead(&ad_isens);

    for(uint8_t i=0;i<4;i++){
      ntcTemp[i] = ntcGetTempFromRatio(ad_dev0.results[i]/4096.);
    }
    
//    canSend(&CAND1,&msg);
//    canSend(&CAND2,&msg);
    chThdSleepMilliseconds(1000);
    IDR_buffer = dio_read(0);
  }
}

static SerialConfig serialcfg = {
  SERIAL_BAUD_2_3,
};

static SerialConfig serialcfg_console = {
  SERIAL_BAUD_1,
};
// spi and mram test
static SPIConfig spicfg = {
  NULL,
  NULL,
  NULL,
  SPI_CR1_BR_2 | SPI_CR1_BR_1// | SPI_CR1_CPOL | SPI_CR1_CPHA
};
#define MRAM_SELECT     palClearPad(GPIOB,12)
#define MRAM_DESELECT   palSetPad(GPIOB,12)
#define FLASH_SELECT     palClearPad(GPIOB,10)
#define FLASH_DESELECT   palSetPad(GPIOB,10)

static THD_FUNCTION(thd_rs485_loop ,p);
//static THD_FUNCTION(thd_can_loop ,p);

static THD_FUNCTION(thd_rs485_echo ,p)
{
  bool done = false;
  SerialDriver *dev = (SerialDriver*)p;
  char msg_out[] = "This is a test message!";
  char msg_in[32];
  char *wptr = msg_in;
  uint32_t sz;
  sz = sdReadTimeout(dev,msg_in,32,MS2ST(100));
  uint32_t test_cycle = 0;
  uint32_t errCode = 0;
  char c;
  
  while(!done){
    
    if (streamRead(dev, (uint8_t *)&c, 1) == 0)
      continue;
    
    *wptr++ = c;
    if(c == '\n'){
      sdWrite(dev,msg_in,wptr - msg_in);
      wptr = msg_in;
    }   
  }
  chThdExit((msg_t)(errCode));
}


static THD_FUNCTION(thd_rs485_loop ,p)
{
  bool done = false;
  char msg_out[] = "This is a test message!";
  char msg_in[32];
  uint32_t sz;
  sz = sdReadTimeout(&SD2,msg_in,32,MS2ST(100));
  sz = sdReadTimeout(&SD3,msg_in,32,MS2ST(100));
  uint32_t test_cycle = 0;
  uint32_t errCode = 0;
  chprintf((BaseSequentialStream*)&SD1,"RS485 Testing\r\n");
  while(!done){
    sdWrite(&SD2,msg_out,strlen(msg_out));
    //chThdSleepMilliseconds(MS2ST(50));
    sz = sdReadTimeout(&SD3,msg_in,32,MS2ST(50));
    //chprintf((BaseSequentialStream*)&SD1,"SD3 read %d bytes of message %d/10 \r\n",sz,test_cycle);
    if(sz > 0){
      if(strncmp(msg_in,msg_out,sz) != 0){
        errCode = 0x1;
        done = true;
      }
    }
    
    
    memset(msg_in,32,0x0);
    sdWrite(&SD3,msg_out,strlen(msg_out));
    //chThdSleepMilliseconds(MS2ST(50));
    sz = sdReadTimeout(&SD2,msg_in,32,MS2ST(50));
    //chprintf((BaseSequentialStream*)&SD1,"SD2 read %d bytes of message %d/10 \r\n",sz,test_cycle);
    if(sz > 0){
      if(strncmp(msg_in,msg_out,sz) != 0){
        errCode = 0x2;
        done = true;
      }
    }
    
    test_cycle++;
    if(test_cycle > 10){
      done = true;
    }
    
  }
  if(errCode == 0x00){
    chprintf((BaseSequentialStream*)&SD1,"RS485 Test OK\r\n");
  }else{
    chprintf((BaseSequentialStream*)&SD1,"RS485 Test NG:%d\r\n",errCode);
  }
  chThdExit((msg_t)(errCode));
}


enum {
  CAN_TX1,
  CAN_RX1,
  CAN_TX2,
  CAN_RX2
};

/*
static THD_FUNCTION(thd_can_loop ,p)
{
  bool done = false;
  uint8_t canState = CAN_TX1;
  uint32_t test_cycle = 0;
  uint32_t errCode = 0;
  msg_t ret,ret_tx;
  uint8_t stage = 0;
  
  j1939_msg_t msg_out,msg_in;
  CANRxFrame rxmsg;
  msg_out.dst = 0x34;
  msg_out.pgn = 100;
  msg_out.src =  0x78;
  msg_out.buffer[0] = 0x11;
  msg_out.buffer[1] = 0x22;
  msg_out.buffer[2] = 0x33;
  msg_out.buffer[3] = 0x44;
  msg_out.buffer[4] = 0x55;
  msg_out.buffer[5] = 0x66;
  msg_out.buffer[6] = 0x77;
  msg_out.buffer[7] = 0x88;
  msg_out.len = 8;
  
  CANDriver *c1 = &CAND1;
  CANDriver *c2 = &CAND2;
  event_listener_t el1,el2;
//  chEvtRegister(&c1->rxfull_event,&el1,(0));
//  chEvtRegister(&c2->rxfull_event,&el2,(1));
  eventmask_t evt;
  chprintf((BaseSequentialStream*)&SD1,"CAN Testing\n");
  while(!done){
   switch(stage){
    case 0:
      ret_tx = canSend(c1,&msg_out);
      evt  = chEvtWaitAnyTimeout(ALL_EVENTS,MS2ST(50));
      if(evt & EVT_CAN2_RECEIVE){
        for(uint8_t j=0;j<8;j++){
          if(appParam.msg2.buffer[j] != msg_out.buffer[j]){
            errCode = 0x1;
            done = true;
          }
        }
        stage = 1;
      }
      break;
    case 1:
      ret_tx = canSend(c2,&msg_out);
      evt  = chEvtWaitAnyTimeout(ALL_EVENTS,MS2ST(50));
      if(evt & EVT_CAN1_RECEIVE){
        for(uint8_t j=0;j<8;j++){
          if(appParam.msg1.buffer[j] != msg_out.buffer[j]){
            errCode = 0x1;
            done = true;
          }
        }
        test_cycle++;
        if(test_cycle == 10){
          done = true;
        }
        stage = 0;
      }
      break;
    }    
  }
  if(errCode == 0x00){
    chprintf((BaseSequentialStream*)&SD1,"CAN Test OK\n");
  }else{
    chprintf((BaseSequentialStream*)&SD1,"CAN Test NG:%d\n",errCode);
  }
  chThdExit((msg_t)(errCode));
}
*/

void wukp_int_handler(EXTDriver *extp, expchannel_t channel)
{
  
}
void accsens_int_handler(EXTDriver *extp, expchannel_t channel)
{
  
}

static msg_t canSend(CANDriver *p, j1939_msg_t *m)
{
  CANTxFrame ptx;
  uint8_t i;
  
  ptx.EID = (m->prio << 26) | (m->pgn << 8) | m->src;
  ptx.IDE = CAN_IDE_EXT;
  ptx.RTR = CAN_RTR_DATA;
  ptx.DLC = m->len;
  memcpy(ptx.data8,m->buffer,m->len);
  
  return canTransmit(p,CAN_ANY_MAILBOX,&ptx,MS2ST(100));
}

static void cmdRead(BaseSequentialStream *chp, int argc, char *argv[])
{
  char msg[64];
  char *ptr = msg;
  uint8_t sz = 0;
  //chprintf(chp,"Digital In:\n");
  for(uint8_t i=0;i<8;i++){
    //chprintf(chp,"CH:%d = %d\n",i,dio_read(i));
    ptr += sprintf(ptr,"%d ",dio_read(i));
  }
  ptr += sprintf(ptr,"\n");
  chprintf(chp,"%s",msg);
}

static void cmdWrite(BaseSequentialStream *chp, int argc, char *argv[])
{
  uint32_t c = strtol(argv[0],NULL,16);
  //chprintf(chp,"Digital In:\n");
  for(uint8_t i=0;i<8;i++){
    if(c & (1 << i)){
      dio_write(i,1);
    }else{
      dio_write(i,0);
    }
  }
  chprintf(chp,"OK\n");
//  chprintf(chp,"Digital Out:\n");
//  for(uint8_t i=0;i<8;i++){
//    chprintf(chp,"CH:%d = %d\n",i,(c & (1<<i))==0?0:1);
//  }
}

static void cmdView(BaseSequentialStream *chp, int argc, char *argv[])
{
  char msg[128];
  char *ptr = msg;
//  chprintf(chp, "ADC 0:\n");
  for(uint8_t i=0;i<8;i++){
    ptr += sprintf(ptr,"%04d ",ad_dev0.results[i]);
//    chprintf(chp,"CH.%d = %04d\n",i,ad_dev0.results[i]);
  }
  sprintf(ptr,"\n");
//  chprintf(chp, "ADC 1:\n");
//  for(uint8_t i=0;i<8;i++){
//    chprintf(chp,"CH.%d = %04d\n",i,ad_isens.results[i]);
//  }
  chprintf(chp,msg);
}

static void cmdSend(BaseSequentialStream *chp, int argc, char *argv[])
{
  if((strncmp(argv[0],"r1",2) == 0)){
    //sdWrite(&UARTD2,argv[1],strlen(argv[1]));
    chprintf(&SD2,"%s\n",argv[1]);
  }
  else if((strncmp(argv[0],"r2",2) == 0)){
//    sdWrite(&SD3,argv[1],strlen(argv[1]));
    chprintf(&SD3,"%s\n",argv[1]);
  }
  else if((strncmp(argv[0],"c1",2) == 0)){
    j1939_msg_t msg;
    msg.dst = 0x1234;
    msg.pgn = 100;
    msg.src =  0x5678;
    msg.buffer[0] = 0x11;
    msg.buffer[1] = 0x22;
    msg.buffer[2] = 0x33;
    msg.buffer[3] = 0x44;
    msg.buffer[4] = 0x55;
    msg.buffer[5] = 0x66;
    msg.buffer[6] = 0x77;
    msg.buffer[7] = 0x88;
    msg.len = 8;
    canSend(&CAND1,&msg);
  }
  else if((strncmp(argv[0],"c2",2) == 0)){
    j1939_msg_t msg;
    msg.dst = 0x1234;
    msg.pgn = 100;
    msg.src =  0x5678;
    msg.buffer[0] = 0x11;
    msg.buffer[1] = 0x22;
    msg.buffer[2] = 0x33;
    msg.buffer[3] = 0x44;
    msg.buffer[4] = 0x55;
    msg.buffer[5] = 0x66;
    msg.buffer[6] = 0x77;
    msg.buffer[7] = 0x88;
    msg.len = 8;
    canSend(&CAND2,&msg);
  }
  
}

static void cmdEcho(BaseSequentialStream *chp, int argc, char *argv[])
{
  if((strncmp(argv[0],"r1",2) == 0)){
    uint8_t tout =  *argv[1] - 0x30;
    char msg[32];
    uint32_t sz;
    //sz = sdReadTimeout(&UARTD2,msg,32,S2ST(tout));
    msg[sz] = 0;
    if(sz)
      chprintf(chp,"> %s\n",msg);
    else
      chprintf(chp,"timeout\n");
//    if(*argv[1] == '0')
//      appParam.testState.echo_rs485 &= ~(0x01);
//    else
//      appParam.testState.echo_rs485 |= 0x01;
  }
  else if((strncmp(argv[0],"r2",2) == 0)){
    uint8_t tout =  *argv[1] - 0x30;
    char msg[32];
    uint32_t sz;
    sz = sdReadTimeout(&SD3,msg,32,S2ST(tout));
    msg[sz] = 0;
    if(sz)
      chprintf(chp,"> %s\n",msg);
    else
      chprintf(chp,"timeout\n");
  }
  else if((strncmp(argv[0],"c1",2) == 0)){
    if(*argv[1] == '0')
      appParam.testState.echo_canbus &= ~(0x01);
    else
      appParam.testState.echo_canbus |= 0x01;
  }
  else if((strncmp(argv[0],"c2",2) == 0)){
    if(*argv[1] == '0')
      appParam.testState.echo_canbus &= ~(0x02);
    else
      appParam.testState.echo_canbus |= 0x02;
  }
}

static void cmdRTC(BaseSequentialStream *chp, int argc, char *argv[])
{
  RTCDateTime timespec;
  rtcGetTime(&RTCD1,&timespec);
  rtcConvertDateTimeToStructTm(&timespec,&appParam.tim_now,NULL);
  if(argc == 0){
    chprintf(chp,"%04d/%02d/%02d-%02d:%02d:%02d\n",
               appParam.tim_now.tm_year+1900,
               appParam.tim_now.tm_mon,
               appParam.tim_now.tm_mday,
               appParam.tim_now.tm_hour,
               appParam.tim_now.tm_min,
               appParam.tim_now.tm_sec);
  }
  else if(argc == 6){
    struct tm tim;
    tim.tm_year = strtol(argv[0],NULL,10)-1900;
    tim.tm_mon = strtol(argv[1],NULL,10);
    tim.tm_mday = strtol(argv[2],NULL,10);
    tim.tm_hour = strtol(argv[3],NULL,10);
    tim.tm_min = strtol(argv[4],NULL,10);
    tim.tm_sec = strtol(argv[5],NULL,10);
    rtcConvertStructTmToDateTime(&tim,0,&timespec);
    rtcSetTime(&RTCD1,&timespec);
  }

}

static void cmdDC(BaseSequentialStream *chp, int argc, char *argv[])
{
  if(argc == 2){
    switch(*argv[0]){
    case '0':
      if(*argv[1] == '0')
        VOUT_5V_OFF();
      else
        VOUT_5V_ON();
      break;
    case '1':
      if(*argv[1] == '0')
        VOUT_12V_OFF();
      else
        VOUT_12V_ON();
      break;
    }
  }
  chprintf(chp,"OK\r\n");
}

static void cmdDigit(BaseSequentialStream *chp, int argc, char *argv[])
{

}

static void rs485_loop_test(BaseSequentialStream *chp, int argc, char *argv[])
{
  chThdCreateFromHeap(NULL,THD_WORKING_AREA_SIZE(1024),"RS485_LOOP",NORMALPRIO,thd_rs485_loop,NULL);
}
/*
static void can_loop_test(BaseSequentialStream *chp, int argc, char *argv[])
{
  appParam.can_tx_thread  = chThdCreateFromHeap(NULL,THD_WORKING_AREA_SIZE(1024),"CAN_LOOP",NORMALPRIO,thd_can_loop,NULL);
}
*/
static void adc_test(BaseSequentialStream *chp, int argc, char *argv[])
{

}

void ADC_device_read(int d){
  if (d == 0) ad5593rRead(&ad_dev0);
  else ad5593rRead(&ad_isens);//
}

void vmuInit(void)
{
  //at24eep_init(&I2CD1, 16,128,0x50,1);
  //eepTest();
  
  // initial digital I/O
  dio_set_in_def(dig_in,NOF_DIG_IN);
  dio_set_out_def(dig_out,NOF_DIG_OUT);
  
  extStart(&EXTD1, &extcfg);
  
  dio_write(AD1_RST,0);
  dio_write(AD2_RST,0);
  
  VOUT_5V_ON();
  
  // test all digital output
//  for(uint8_t i=0;i<NOF_DIG_OUT;i++){
//    dio_write(i,1);
//  }

  sdStart(&SD1,&serialcfg_console);
  //chprintf((BaseSequentialStream*)&SD1,"SD1 Configured\r\n");
  sdStart(&SD2,&serialcfg);
  //chprintf((BaseSequentialStream*)&SD1,"SD2 Configured\r\n");  
  sdStart(&SD3,&serialcfg);
  
#ifdef Debug  
  chprintf((BaseSequentialStream*)&SD1,"SD1,SD2, SD3 Configured\r\n");
#endif
  

  // test for aux power (5V) output
//  dio_write(AUX_PWR_EN,AUX_POWER_ON);
//  dio_write(AUX_PWR_EN,AUX_POWER_OFF);
  
  canStart(&CAND1,&canCfg500K);
  //canStart(&CAND2,&canCfg250K);
  canStart(&CAND2,&canCfg500K);
  
  ad_dev0.results = ad0Result;
  ad_isens.results = ad1Result;
  
  ad5593rObjectInit(&ad_dev0);
  ad5593rStart(&ad_dev0,&adc_cfg0);
  
  ad5593rObjectInit(&ad_isens);
  ad5593rStart(&ad_isens,&adc_cfg1);
  
  ntcInit(2.5, 10000,10000,3950, 25);
  
  
  spiStart(&SPID2,&spicfg);
  mr25_init(&mr25);
//  mr25_read_write_test(0,&mr25);
//  mr25_read_write_test(0x100,&mr25);
//  int16_t result[8];
//  for(uint8_t i=0;i<100;i++){
//    ad5593rRead(&ad_dev0,result);
//    chThdSleepMilliseconds(200);
//  }
  
  //chThdCreateStatic(waShell,sizeof(waShell),NORMALPRIO + 1,shellThread,(void*)&shell_cfg);

  
  //can1Recv = chThdCreateStatic(waCANRX1,sizeof(waCANRX1),NORMALPRIO,thCanRx,&CAND1);
  //can2Recv = chThdCreateStatic(waCANRX2,sizeof(waCANRX2),NORMALPRIO,thCanRx,&CAND2);
  //chThdCreateStatic(waRS485_C1,sizeof(waRS485_C1),NORMALPRIO,thd_rs485_echo,&UARTD2);
  //chThdCreateStatic(waRS485_C2,sizeof(waRS485_C2),NORMALPRIO,thd_rs485_echo,&SD3);
  thdVMU = chThdCreateStatic(waVMU,sizeof(waVMU),NORMALPRIO+1,thVMU,NULL);
  
  //chThdCreateStatic(waRs485Loop,sizeof(waRs485Loop),NORMALPRIO+1,thd_rs485_loop,NULL);
  
  //chThdCreateFromHeap(NULL,THD_WORKING_AREA_SIZE(1024),"RS485_LOOP",NORMALPRIO,thd_rs485_loop,NULL);
  //chThdCreateFromHeap(NULL,THD_WORKING_AREA_SIZE(1024),"CAN_LOOP",NORMALPRIO,thd_can_loop,NULL);
  
  
}