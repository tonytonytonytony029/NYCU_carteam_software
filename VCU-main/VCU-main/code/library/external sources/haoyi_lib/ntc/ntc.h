#ifndef _NTC_H_
#define _NTC_H_

#include "hal.h"

void ntcInit(double vdrv, double shunt_res, double ntc_res, double beta, double beta_temp);
double ntcGetTempFromV(double volt);
double ntcGetTempFromRatio(double ratio);
int NTC_ADC2Temperature(unsigned int adc_value);
int16_t ntcCalculateTempD10(int16_t mv, int16_t vdrv_mv, int16_t r_shunt, int16_t r_ntc_norm,int16_t beta, uint8_t betaTemp);
int16_t ntcCalculateTempD10_R(uint32_t r, uint32_t r_ntc_norm,int16_t beta, uint8_t betaTemp);

float ntcGetTempFromRatioF_RBOT(float ratio);
float ntcGetTempFromRatioF_RTOP(float ratio);

float ntcGetTempFromRatioF_RTOP_Ex(float ratio, float r_shunt, float r_ntc_norm, uint16_t _beta, uint16_t _betaTemp);

#endif