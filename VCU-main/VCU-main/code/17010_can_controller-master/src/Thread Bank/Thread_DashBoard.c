#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "digital_io.h"
#include "DashBoard.h"
#include "vmu.h"
#include "motor_controller.h"

#define LOCAL_TEST true

void db_test();
void db_test(){  
  int n = 0;
  
  //1. Dashboard refresh checking
  while(true){
      chThdSleepMilliseconds(500);
      DB_CMD_SetAndShow_SmallLED(5,n);
      n++;
      n%=2;
      DB_CMD_Show_MotorSpeed();
      dashboard_dev.motorSpeed = 1000 * n;
  }
  
  //2. Dashboard mode changing
  while(false){
    DB_CMD_Show_Page(n);
            n++;
            n%=4;
            chThdSleepMilliseconds(500);
  }
  
  
  /*while(false){
      chThdSleepMilliseconds(500);
      dashboard_dev.data[42] = n;
      DB_CMD_Show_ItemValue(42);
      n++;
      n%=20;
  }*/

}


void DB_VirtualMachineReset(void){
    dashboard_dev.motorSpeed = 0;
    dashboard_dev.wheelDirection = 0;
    dashboard_dev.page = driving;
    dashboard_dev.BigLED_Flag = 0xFF;
    dashboard_dev.SmallLED_Flag = 0xFF;
}

uint8_t change_page_flag;
static THD_WORKING_AREA(dashboard_wa, 256);
static THD_FUNCTION(thread_dashboard, p) {
    (void)p;
  uint8_t n = 0;
  change_page_flag = dio_read(1);//
    DB_VirtualMachineReset();
    dashboard_dev.data[42] = 0;
    
    if(LOCAL_TEST == true) db_test();
    
    while(true){
        //DB_Check_Lap();
        //DB_Check_Wheel();
        //DB_Check_LED();
        //DB_PageViewRefresh();
        chThdSleepMilliseconds(500);

        
        change_page_flag = dio_read(1);

        
        if(change_page_flag){
            DB_CMD_Show_Page(n);
            n++;
            n%=4;
            chThdSleepMilliseconds(300);
        }
        
        
        
        //n++;
        //n%=4;
        //chprintf((BaseSequentialStream*)&SD1,"\r\n");
        /*
        if(dio_read(1) == 0){
            change_page_flag++;
            chThdSleepMilliseconds(10);
        }
        if(change_page_flag){
            change_page_flag--;
            DB_CMD_Show_Page(n);
            n++;
            n%=4;
            chThdSleepMilliseconds(300);
        }
      */
    }

}

void dashboard_init(void){
    chThdCreateStatic(dashboard_wa, sizeof(dashboard_wa), NORMALPRIO + 7, thread_dashboard, NULL);
}