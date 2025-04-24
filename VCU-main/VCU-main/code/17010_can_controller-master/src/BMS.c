#include <stdio.h>
#include <string.h>
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "stdlib.h"

//#include "ff.h"
#include "BMS.h"

#define rxmsg (*pRxmsg)

BMS_t BMS_dev;

static bool init = false;
static CANRxFrame* pRxmsg;


void BMS_init(){
    init = true;
    pRxmsg = 0;//pCRF;
}

bool BMS_readCANmsg(){ //void
    if(init != TRUE) return FALSE;
    
    if(rxmsg.SID != 0xBB) return TRUE;
    
    BMS_dev.state = rxmsg.data8[0];

    return TRUE;

}
