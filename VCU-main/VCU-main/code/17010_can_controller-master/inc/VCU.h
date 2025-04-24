#include "ch.h"

#define VCU_DEBUG true

#define SERIAL_BAUD_1 115200
#define SERIAL_BAUD_2_3 9600

#define DRIVER_DIGIT_IO_ENABLE true

#define THD_ADC_TEST_ENABLE true
#define THD_ADC_TEST_SKIP true
#define THD_RTD_ENABLE true
#define THD_RTD_SKIP false
#define THD_DASHBOARD_ENABLE true
#define THD_ADC_ENABLE true
#define THD_MCU_ENABLE true
#define THD_BMS_ENABLE true

#if !defined(SYS_USE_CONSOLE)
#define SYS_USE_CONSOLE true
#define SYS_CONSOLE_CH SD1
#endif


#if VCU_DEBUG == true
#undef THD_ADC_TEST_SKIP
#undef THD_RTD_SKIP 
#undef THD_DASHBOARD_ENABLE 
#undef THD_ADC_ENABLE 
#undef THD_MCU_ENABLE 
#undef THD_BMS_ENABLE 
#undef SERIAL_BAUD_1 
#undef SERIAL_BAUD_2_3 

#define THD_ADC_TEST_SKIP true
#define THD_RTD_SKIP true
#define THD_DASHBOARD_ENABLE false
#define THD_ADC_ENABLE false
#define THD_MCU_ENABLE false
#define THD_BMS_ENABLE false
#define SERIAL_BAUD_1 9600
#define SERIAL_BAUD_2_3 9600
#endif


#define APPS1_PIN 6
#define APPS2_PIN 7
#define APPS1_LOWER_BOUND 500
#define APPS1_UPPER_BOUND 3400
#define APPS2_LOWER_BOUND 250
#define APPS2_UPPER_BOUND 1700
#define APPS1_START 800
#define APPS2_START 440
#define APPS1_RANGE (APPS1_UPPER_BOUND - APPS1_START)
#define APPS2_RANGE (APPS2_UPPER_BOUND - APPS2_START)

#define ADC_PIN1 APPS1_PIN
#define ADC_PIN2 APPS2_PIN
#define ADC_CH1_LIMIT_LOW APPS1_LOWER_BOUND
#define ADC_CH1_LIMIT_HIGH APPS1_UPPER_BOUND
#define ADC_CH2_LIMIT_LOW APPS2_LOWER_BOUND
#define ADC_CH2_LIMIT_HIGH APPS2_UPPER_BOUND

#define RTD_CH 2
#define RTD_level 4000


typedef enum{
  VCU_INIT,
  VCU_TEST,
  VCU_READY,
  VCU_DRIVE,
  VCU_FAIL,
} VCU_state_t;

typedef struct{
  VCU_state_t state;
} VCU_t;


void DriverInit();
int console_print(const char *fmt, ...);


void vcuStartFSM();
void vcuEnterTest();
void vcuEnterReady();
void vcuEnterDrive();

extern uint8_t* const pmask; // a pointer that pointing to a const. address