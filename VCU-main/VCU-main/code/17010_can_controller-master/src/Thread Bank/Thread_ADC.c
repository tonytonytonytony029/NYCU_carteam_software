#include "ch.h"
#include "hal.h"

#ifndef _AD5593R_H
#include "ad5593r.h"
#endif

#include "FIR.h"
#include "vmu.h"
#include "motor_controller.h"

#include "VCU.h"

#define ADC_Sampling_Rate 100
#define ADC_Time_Between_Samples 1/ADC_Sampling_Rate*1000
#define Torque_MAX 60

extern int16_t ADC_Result;
int16_t ADC_DSP(int16_t* adc_data_array);
//int16_t ADC_1[8] = {0};
//int16_t ADC_2[8] = {0};
int ADC_counter = 0;

FIRFilter FIRFilter1, FIRFilter2;
#define FIR_order 10
static float FIR_ImpulseResponse[10] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
int16_t FIR_buffer1[10] = {0};
int16_t FIR_buffer2[10] = {0};
int16_t ProcessedSignal[2] ={0};
int16_t FinishedData = {0};
static int16_t torque = 0;
static int16_t torqueRatio = 0;
static int16_t APPS1_ratio = 0;
static int16_t APPS2_ratio = 0;
static int16_t error_counter_out_of_bound = 0;
static int16_t error_counter_implausibility = 0;
int16_t ProcessedSignalCompare(int16_t sig0, int16_t sig1);

int16_t ProcessedSignalCompare(int16_t sig0, int16_t sig1){
  if (sig0 > sig1) return sig0;
  else return sig1;
}

bool SendProcessedSignalToMotor();
bool SendProcessedSignalToMotor(){
  
}
void rt_OneStep(void);
void rt_test(bool);

/*
static THD_WORKING_AREA(adc_wa, 256);
static THD_FUNCTION(ThdAdc, d) {
  chRegSetThreadName("ADC");
  subsystem1_initialize();
  
  rt_test(true);
  
  
  while(true){
    chThdSleepMilliseconds(5);
    ADC_device_read(0);
    ADC_device_read(1);
    subsystem1_U.pedal1 = ad_dev0.results[0] / 1000.0;
    subsystem1_U.pedal2 = ad_dev0.results[1] / 1000.0;
    rt_OneStep();
    torqueRatio = subsystem1_Y.APPS_output;
  }
}
*/
int16_t APPS_check_implausibility(int16_t APPS1_input_ratio, int16_t APPS2_input_ratio, int16_t error_counter);


int16_t APPS_check_out_of_bound(int16_t APPS1_input, int16_t APPS2_input, int16_t error_counter);
int16_t APPS_check_out_of_bound(int16_t APPS1_input, int16_t APPS2_input, int16_t error_counter){
    if(APPS1_input > APPS1_UPPER_BOUND || APPS1_input < APPS1_LOWER_BOUND) {
        return error_counter + 1;
    }
    else if(APPS2_input > APPS2_UPPER_BOUND || APPS2_input < APPS2_LOWER_BOUND) {
        return error_counter + 1;
    }
    else return 0;
}

int16_t APPS_check_implausibility(int16_t APPS1_input_ratio, int16_t APPS2_input_ratio, int16_t error_counter){
    int8_t difference = 0;
    if(APPS1_input_ratio >= APPS2_input_ratio) difference = APPS1_input_ratio - APPS2_input_ratio;
    else difference = APPS2_input_ratio - APPS1_input_ratio;
    if(difference > 10) return error_counter + 1;
    else return 0;
}
int8_t error_stop = 0;
static THD_WORKING_AREA(adc_wa, 256);
static THD_FUNCTION(ThdAdc, d) {
    
    chRegSetThreadName("ADC");
    FIRFilter_Init(&FIRFilter1, FIR_ImpulseResponse, FIR_buffer1, FIR_order);
    FIRFilter_Init(&FIRFilter2, FIR_ImpulseResponse, FIR_buffer2, FIR_order);
    while(true){
        chThdSleepMilliseconds(5);
        
        ADC_device_read(0);
        error_counter_out_of_bound = APPS_check_out_of_bound(ad_dev0.results[ADC_PIN1], ad_dev0.results[ADC_PIN2], error_counter_out_of_bound);
        int16_t adc_raw_data = ad_dev0.results[ADC_PIN1] / 2;
        ProcessedSignal[0] = ad_dev0.results[ADC_PIN1];//FIRFilter_Update(&FIRFilter1, ad_dev0.results[ADC_PIN1]);
        APPS1_ratio = (ProcessedSignal[0] >= APPS1_START) ? (ProcessedSignal[0] - APPS1_START) * 100 / APPS1_RANGE : 0;
        ADC_device_read(1);
        ProcessedSignal[1] = ad_dev0.results[ADC_PIN2];//FIRFilter_Update(&FIRFilter2, ad_dev0.results[ADC_PIN2]);
        APPS2_ratio = (ProcessedSignal[1] >= APPS2_START) ? (ProcessedSignal[1] - APPS2_START) * 100 / APPS2_RANGE : 0;
        error_counter_implausibility = APPS_check_implausibility(APPS1_ratio, APPS2_ratio, error_counter_implausibility);
        torque = (APPS1_ratio + APPS2_ratio) / 2 * Torque_MAX / 100;
        error_stop = (error_counter_out_of_bound >= 20 || error_counter_implausibility >= 20) ? 1 : error_stop;
        //torque = (error_stop) ? 0 : torque;
        
        //chprintf((BaseSequentialStream*)&SD1,"ADC_02 read\r\n");
        
        //data compare
        
        FinishedData = ProcessedSignalCompare(ProcessedSignal[0], ProcessedSignal[1]);
        
//        if(FinishedData >= ACCEL_MAX){//
//            torque = Torque_MAX;
//        }
//        else if(FinishedData <= ACCEL_MIN){
//            torque = 0;
//        }
//        
//        else{
//            torque = (FinishedData-ACCEL_MIN)*Torque_MAX /ACCEL_RANGE;
//        }
//        torque = (ad_dev0.results[6] >= 3500)? 0 : torque;
        
        /*
        if(ad_dev0.results[6] > ACCEL_MAX){
            torque = Torque_MAX;
        }
        else if(ad_dev0.results[6] <= ACCEL_MIN){
            torque = 0;
        }    
        else{
            torque = (ad_dev0.results[6]-ACCEL_MIN)*Torque_MAX /ACCEL_MAX;
        }
        */
        ADC_counter++;
        torqueRatio = torque * 100 / Torque_MAX;
        //torqueRatio = torque*100/150;
        // data sending
        SendProcessedSignalToMotor();
        //chprintf((BaseSequentialStream*)&SD1,"ADC_02 read\r\n");
        //chprintf((BaseSequentialStream*)&SD1, "%d ", ad_dev0.results[0]);
        //chprintf((BaseSequentialStream*)&SD1, "acc_torqure = ");
        //chprintf((BaseSequentialStream*)&SD1, "%d, ", unprocessed_torque);
        //chprintf((BaseSequentialStream*)&SD1, "%d\r\n", torque);
        //...
        mc_dev.torque = torque * 10;
    }
}

void adc_init(void){
  chThdCreateStatic(adc_wa, sizeof(adc_wa), NORMALPRIO, ThdAdc, NULL);
}

