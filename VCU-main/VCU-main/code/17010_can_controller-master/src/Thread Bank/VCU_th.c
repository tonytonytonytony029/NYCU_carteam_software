#include "hal.h"

#include "ch.h"
#include "vmu.h"
#include "VCU.h"
#include "digital_io.h" 

static THD_WORKING_AREA(waADCtest, 128);
static THD_FUNCTION(thdADCtest, arg) {

  if(THD_ADC_TEST_SKIP == true){
    chThdSleepMilliseconds(2000);
    (*pmask) = 0x01;
    chThdExit((msg_t)0);
  }

  int16_t* pch1 = &(ad_dev0.results[ADC_PIN1]);
  int16_t* pch2 = &(ad_dev0.results[ADC_PIN2]);

#define ch1 (*pch1)
#define ch2 (*pch2)
  int counter = 0;
  int time = 0;
  
  while (true) {
    chThdSleepMilliseconds(10);
    time++;
    
    if((ch1 > ADC_CH1_LIMIT_LOW) && (ch1 < ADC_CH1_LIMIT_HIGH) &&\
       (ch2 > ADC_CH2_LIMIT_LOW) && (ch2 < ADC_CH2_LIMIT_HIGH)){
      counter++;
    }
    
    if(time >= 150){
      time = 0;
      if(counter == 100) break;
    }
  }
#undef ch1
#undef ch2
  (*pmask) = 0x01;
  chThdExit((msg_t)0);
}

void thdCreateADCtest(){
#if THD_ADC_TEST_ENABLE == true
  chThdCreateStatic(waADCtest, sizeof(waADCtest), NORMALPRIO, thdADCtest, NULL);
#endif
}


static THD_WORKING_AREA(waRTD, 128);
static THD_FUNCTION(thdRTD, arg) {

  if(THD_RTD_SKIP == true){
    chThdSleepMilliseconds(2000);
    (*pmask) = 0x01;
    chThdExit((msg_t)0);
  }

  int Switch = 0;
  
  
  while (true) {
    static int counterS = 0;
    static int time = 0;
    
    chThdSleepMilliseconds(10); 
    time++;
    
    Switch = ad_dev0.results[RTD_CH];
    if(Switch >= RTD_level) counterS++;
    

    if(time >= 30){ 
      time = 0;
      if(counterS >= 15){
        break;
      }else{
        counterS = 0;
      } 
    }
    
  }
  *pmask = 0x01; // signal the main thread 
  
  chThdExit((msg_t)0);
}

void thdCreateRTDlock(){
#if THD_RTD_ENABLE == true
  chThdCreateStatic(waRTD, sizeof(waRTD), NORMALPRIO, thdRTD, NULL);
#endif
}

void thdCreateDashboard(){
#if THD_DASHBOARD_ENABLE == true
  dashboard_init();
    console_print(">>Thread for Dashboard created\r\n");
    chThdSleepMilliseconds(5);
#endif
}
void thdCreateBMS(){
#if THD_BMS_ENABLE == true

#endif
}
void thdCreateADC(){
#if THD_ADC_ENABLE == true
      adc_init();
      chThdSleepMilliseconds(5);
      console_print(">>Thread for accelarator created\r\n");
      chThdSleepMilliseconds(5);
#endif
}
void thdCreateMCU(){
#if THD_MCU_ENABLE == true
    can_rx_thd_init();
    chThdSleepMilliseconds(5);
    console_print(">>Thread for MCU created\r\n");
    chThdSleepMilliseconds(5);
#endif
}