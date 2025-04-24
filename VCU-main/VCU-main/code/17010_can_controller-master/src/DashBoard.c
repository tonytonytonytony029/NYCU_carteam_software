#include "DashBoard.h"
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
/**
 * 
    uint8_t msg[6] = {0};
    
    if( ) return FALSE;

    msg[0] = 0x;
    msg[1] = 0x;
    msg[2] = 0x;
    msg[3] = 0x;
    msg[4] = 0x;
    msg[5] = CHKSUM(msg);
    send(msg,6,100);

    return TRUE;
 * 
*/
/*
chprintf((BaseSequentialStream*)&SD1,"RS485 Testing\r\n");
chThdSleepMilliseconds(1000);
*/

/* ------------------ Local Funtion Declaration------------------*/

uint8_t CHKSUM(uint8_t* data);
void send(uint8_t* data, uint8_t length,uint16_t waitTime);
void debug(uint8_t* msg, uint16_t waitTime);

/* ------------------ Variables ------------------*/

DashBoard_t dashboard_dev;

int Page0Handler[Page_Driving_ItemAmount] = {0,1,2,3,4,5,6,7,8,9,42,43,44,45};
int Page1Handler[Page_Pit1_ItemAmount] = {10,11,12,13,14,\
                                        15,16,17,18,19,\
                                        20,21,22,23,24,\
                                        25,26};
int Page2Handler[Page_Pit2_ItemAmount] = {27,28,29,30,31,\
                                        32,33,34,35,47,48};
int Page3Handler[Page_Pit3_ItemAmount] = {36,37,38,39,40,41};

static bool allowSend = true;

/* ------------------ Funtion Definition ------------------*/

void setBit(uint16_t* flag, uint8_t n, uint8_t bit) {
    *flag &= (~(0x01 << (n-1)));
    *flag |= ((uint16_t)bit <<(n-1));
}

uint8_t getBit(uint16_t* flag,uint8_t n) {\
    uint8_t buffer;
    if( ((*flag) & (0x01 << (n-1))) >0 ) buffer = 1 ;\
    else buffer = 0;
    return buffer;
}

uint8_t CHKSUM(uint8_t* data){
    uint8_t sum = 0;
    for(int i = 0; i < 5; i++){
        sum ^= data[i];
    }
    return sum;
}

void debug(uint8_t* msg, uint16_t waitTime){
  if(DB_DEBUG == false) return;
  
  switch(*msg){
    default:
      for(int i =0; i < 6; i++){
        
        uint8_t rcv = 0;
        uint8_t sendMsg = 0x00;
       

        //sdPutTimeout(&SD1, sendMsg,MS2ST(10));
        //rcv = sdGetTimeout(&SD1,MS2ST(10));
        
        sdWriteTimeout(&SD1,&sendMsg,1,MS2ST(waitTime));
        chThdSleepMilliseconds(5);
        rcv = sdGetTimeout(&SD1, MS2ST(waitTime));
        
        
        
        if(rcv == 0x06) return;
        
      }
  }
  return;
}
  
void send(uint8_t* data, uint8_t length,uint16_t waitTime){;
  uint8_t rcvMsg;
  
  if(allowSend == false) return;
    
  allowSend = false;
  
  for(int i = 0; i <1 ; i++){
    sdWriteTimeout(&SD1,data,6,MS2ST(waitTime));
    sdReadTimeout(&SD1, &rcvMsg, 1, MS2ST(waitTime));
    
    if(DB_NEWLINE == true) chprintf((BaseSequentialStream*)&SD1,"\r\n");
    
    if(rcvMsg == Rcv_OK){
      allowSend = true;
      return;
    }else{
      debug(&rcvMsg, waitTime);
      allowSend = true;
    }
    
  }
  return;
}


bool DB_CMD_Show_Page(uint8_t id){
    uint8_t msg[6] = {0};

    if(id > 3) return FALSE;

    msg[0] = 0x01;
    msg[1] = 0x0a;
    msg[2] = id;
    msg[3] = 0x00;
    msg[4] = 0x00;
    msg[5] = CHKSUM(msg);
    //send(msg,6,100);
    send(msg,6,100);

    dashboard_dev.page = (DB_Page_t)id;

    return TRUE;
}

// ID: 0~48
// ID 46 are not availalbe
bool DB_CMD_Show_ItemValue(uint8_t id){
    uint8_t msg[6] = {0};
    
    if( id > 48) return FALSE;
    if( id == 46) return FALSE;
    //if( id == 44) return FALSE;

    msg[0] = 0x01;
    msg[1] = 0x0f;
    msg[2] = id;
    msg[3] = dashboard_dev.data[id] >> 8 ;
    msg[4] = dashboard_dev.data[id] & 0x00FF;
    msg[5] = CHKSUM(msg);
    send(msg,6,100);

    return TRUE;
}

bool DB_CMD_Show_MotorSpeed(void){
    uint8_t msg[6] = {0};

    msg[0] = 0x01;
    msg[1] = 0x07;
    msg[2] = 0x00;
    msg[3] = dashboard_dev.motorSpeed >> 8;
    msg[4] = dashboard_dev.motorSpeed & 0x00FF;
    msg[5] = CHKSUM(msg);
    send(msg,6,100);

    return TRUE;
}

bool DB_CMD_Show_WheelDirection(void){
    uint8_t msg[6] = {0};

    if( dashboard_dev.wheelDirection > 90) return FALSE;

    msg[0] = 0x01;
    msg[1] = 0x10;
    msg[2] = 0x00;
    msg[3] = dashboard_dev.wheelDirection >> 8;
    msg[4] = dashboard_dev.wheelDirection & 0x00FF;
    msg[5] = CHKSUM(msg);
    send(msg,6,100);

    return TRUE;
}

bool DB_CMD_SetAndShow_BigLED(uint8_t id, uint8_t data){
    uint8_t msg[6] = {0};
    
    if(id > 5) return FALSE;

    setBit(&(dashboard_dev.BigLED_Flag), id, data);

    msg[0] = 0x01;
    msg[1] = 0x13;
    msg[2] = id;
    msg[3] = 0x00;
    msg[4] = data;
    msg[5] = CHKSUM(msg);
    send(msg,6,100);

    return TRUE;
}

bool DB_CMD_SetAndShow_SmallLED(uint8_t id, uint8_t data){
    uint8_t msg[6] = {0};


    if( id>13 ) return FALSE;

    setBit(&(dashboard_dev.SmallLED_Flag), id, data);

    msg[0] = 0x01;
    msg[1] = 0x13;
    msg[2] = id;
    msg[3] = 0x00;
    msg[4] = data;
    msg[5] = CHKSUM(msg);
    send(msg,6,100);
    
    return TRUE;
}

// fastest: 2,3,4
// current: 5,6,7
void DB_Check_Lap(void){
    uint32_t fastest = 0 ;
    uint32_t current = 0 ;
    uint32_t temp =0;

    fastest = dashboard_dev.data[2]*60*1000 + dashboard_dev.data[3]*1000 + dashboard_dev.data[4];
    current = dashboard_dev.data[2]*60*1000 + dashboard_dev.data[3]*1000 + dashboard_dev.data[4];
    temp = current - fastest;
    
    if (temp >0){
        setBit(&(dashboard_dev.BigLED_Flag), 4,0);
        DB_CMD_SetAndShow_BigLED(4,0);

        dashboard_dev.data[45] = temp / 1000;
        dashboard_dev.data[46] = temp % 1000;
    } 
    else if(temp<1){
        setBit(&(dashboard_dev.BigLED_Flag), 4,1);
        DB_CMD_SetAndShow_BigLED(4,1);
    }
}

// lap data: ID [10, 11, 12] ~ [19, 20, 21]
void DB_Check_Wheel(void){
    int wheelOrder = 0; 

    // left-front
    if ((dashboard_dev.data[10 +wheelOrder*3 +0])>WheelTemperatureLimit || \
        (dashboard_dev.data[10 +wheelOrder*3 +1])>WheelPressureLimit || \
        (dashboard_dev.data[10 +wheelOrder*3 +2])>WheelSuspensionLimit )
    {
        setBit(&(dashboard_dev.BigLED_Flag), 0, 0);
        DB_CMD_SetAndShow_BigLED(0, 0);
    }
    else{
        setBit(&(dashboard_dev.BigLED_Flag), 0, 1);
        DB_CMD_SetAndShow_BigLED(0, 1);
    }
    wheelOrder++;

    //right-front
    if ((dashboard_dev.data[10 +wheelOrder*3 +0])>WheelTemperatureLimit || \
        (dashboard_dev.data[10 +wheelOrder*3 +1])>WheelPressureLimit || \
        (dashboard_dev.data[10 +wheelOrder*3 +2])>WheelSuspensionLimit )
    {
        setBit(&(dashboard_dev.BigLED_Flag), 2,0);
        DB_CMD_SetAndShow_BigLED(2, 0);
    }
    else{
        setBit(&(dashboard_dev.BigLED_Flag), 2,1);
        DB_CMD_SetAndShow_BigLED(2, 1);
    }
    wheelOrder++;

    //left-back
    if ((dashboard_dev.data[10 +wheelOrder*3 +0])>WheelTemperatureLimit || \
        (dashboard_dev.data[10 +wheelOrder*3 +1])>WheelPressureLimit || \
        (dashboard_dev.data[10 +wheelOrder*3 +2])>WheelSuspensionLimit )
    {
        setBit(&(dashboard_dev.BigLED_Flag), 1,0);
        DB_CMD_SetAndShow_BigLED(1, 0);
    }
    else{
        setBit(&(dashboard_dev.BigLED_Flag), 1,1);
        DB_CMD_SetAndShow_BigLED(1, 1);
    }
    wheelOrder++;

    //right-back
    if ((dashboard_dev.data[10 +wheelOrder*3 +0])>WheelTemperatureLimit || \
        (dashboard_dev.data[10 +wheelOrder*3 +1])>WheelPressureLimit || \
        (dashboard_dev.data[10 +wheelOrder*3 +2])>WheelSuspensionLimit )
    {
        setBit(&(dashboard_dev.BigLED_Flag), 3,0);
        DB_CMD_SetAndShow_BigLED(3, 0);
    }
    else{
        setBit(&(dashboard_dev.BigLED_Flag), 3,1);
        DB_CMD_SetAndShow_BigLED(3, 1);
    }
}

void DB_Check_LED(){
    if (~(dashboard_dev.SmallLED_Flag) >0){
    setBit(&(dashboard_dev.BigLED_Flag), 5,0);
    DB_CMD_SetAndShow_BigLED(5,0);
    }
}

bool DB_PageViewRefresh(void){
    switch(dashboard_dev.page){
        case driving:
            for(int k = 0; k< Page_Driving_ItemAmount; k++){
                DB_CMD_Show_ItemValue(Page0Handler[k]);
                chThdSleepMilliseconds(1);
            }
            break;
        case pit1:
            for(int k=0; k< Page_Pit1_ItemAmount; k++){
                DB_CMD_Show_ItemValue(Page1Handler[k]);
                chThdSleepMilliseconds(1);
            }
            break;
        case pit2:
            for(int k=0; k< Page_Pit2_ItemAmount; k++){
                DB_CMD_Show_ItemValue(Page2Handler[k]);
                chThdSleepMilliseconds(1);
            }
            break;
        case pit3:
            for(int k=0; k< Page_Pit3_ItemAmount ;k++){
                DB_CMD_Show_ItemValue(Page3Handler[k]);
                chThdSleepMilliseconds(1);
            }
            break;
        default:
            return FALSE;
            break;
    }
    return TRUE;
}