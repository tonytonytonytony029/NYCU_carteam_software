#include <stdio.h>
#include <string.h>
#include "ch.h"
#include "hal.h"
#include "shell.h"
#include "evtimer.h"
#include "chprintf.h"
#include "stdlib.h"
#include "hal_ad7124.h"
#include "mbtask.h"
#include "sysParam.h"
//#include "ff.h"
#include "vmu.h"
#include "shell.h"


/*static THD_WORKING_AREA(can_tx_wa,256);
static THD_FUNCTION(can_tx, p) {
    uint32_t input = 0;  // Replace with your actual input
    uint32_t bit_size = 0;// Replace with your actual bit size

    for(int i=0;i<5;i++){
      input=input*10+argv[0][i]-48;
    }
    for(int i=0;i<2;i++){
      bit_size=bit_size*10+argv[1][i]-48;
    }
    char output[MAX_BIT_SIZE + 1];  // Allocate enough space for max bit_size, plus null terminator

    char* result = DecToBin(bit_size, input, output);

    if (result[0] == 'E') {  // Error handling
        // Handle error here
      chprintf(&SD1,"error\n");
    } else {
        // Use the output here
        // This could be sending the data over a serial port, storing it to memory, etc.
      for(int i = 0 ;i<bit_size ; i++){
        chprintf(&SD1,output[i]);
      }
    }

    return 0;
  
}*/

int main(int argc, char* argv[]) {
    halInit();
    chSysInit();
    vmuInit();
    /*uint32_t input = 0;  // Replace with your actual input
    uint32_t bit_size = 0;// Replace with your actual bit size

    for(int i=0;i<5;i++){
      input=input*10+argv[0][i]-48;
    }
    for(int i=0;i<2;i++){
      bit_size=bit_size*10+argv[1][i]-48;
    }
    char output[MAX_BIT_SIZE + 1];  // Allocate enough space for max bit_size, plus null terminator

    char* result = DecToBin(bit_size, input, output);

    if (result[0] == 'E') {  // Error handling
        // Handle error here
      chprintf(&SD1,"error\n");
    } else {
        // Use the output here
        // This could be sending the data over a serial port, storing it to memory, etc.
      for(int i = 0 ;i<bit_size ; i++){
        chprintf((BaseSequentialStream *)&SD1,output[i]);
      }
    }*/
    //chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 7, can_tx, NULL);
    while (1) {
    chThdSleepMilliseconds(1000);
    }
  
    return 0;
}

