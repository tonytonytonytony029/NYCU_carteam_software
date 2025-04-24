#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#include "BMS.h"



static THD_WORKING_AREA(bms_wa, 256);
static THD_FUNCTION(thread_bms, p) {
    
    while(true){
        chThdSleepMilliseconds(100);//
        BMS_readCANmsg();
    }

}

void BMS_VM_init(CANRxFrame* p){
    BMS_init(p);
    chThdCreateStatic(bms_wa, sizeof(bms_wa), NORMALPRIO + 7, thread_bms, NULL);
}