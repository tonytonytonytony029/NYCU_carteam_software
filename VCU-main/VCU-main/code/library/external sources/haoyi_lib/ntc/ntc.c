#include <math.h>
#include "ntc.h"

#define ABSOLUTE_TEMP   273.15

static double drive_voltage;
static double shunt_resistor_ohmn;
static double beta;
static double beta_calib_temp;
static double ntc_resistance;


/*
NTC table generator from 
http://www.sebulli.com/ntc/index.php?lang=en
*/

/**
* The NTC table has 33 interpolation points.
* Unit:0.1 ¢XC
* R25 = 10K, B=3435, PULL-DOWN, 1K
*/
const int NTC_table[33] = {
  -180, -15, 150, 260, 346, 419, 484, 543, 
  598, 651, 702, 751, 799, 848, 896, 945, 995, 
  1046, 1100, 1155, 1214, 1276, 1344, 1418, 
  1500, 1592, 1700, 1830, 1993, 2213, 2548, 
  3215, 3882
};

int NTC_ADC2Temperature(unsigned int adc_value)
{
 
  int p1,p2;
  /* Estimate the interpolating point before and after the ADC value. */
  p1 = NTC_table[ (adc_value >> 7)  ];
  p2 = NTC_table[ (adc_value >> 7)+1];
 
  /* Interpolate between both points. */
  return p1 + ( (p2-p1) * (adc_value & 0x007F) ) / 128;
};


double ntcGetTempFromV(double volt)
{
  double r_ntc = (drive_voltage - volt)*shunt_resistor_ohmn/volt;
  double tmp = r_ntc/ntc_resistance;
  tmp = log(tmp);
  tmp /= beta;
  tmp += 1.0/(ABSOLUTE_TEMP + beta_calib_temp);
  tmp = 1.0/tmp;
  tmp -= ABSOLUTE_TEMP;
  return tmp;
}

double ntcGetTempFromRatio(double ratio)
{
  double r_ntc = (1-ratio)*shunt_resistor_ohmn/(ratio);
  double tmp = r_ntc/ntc_resistance;
  tmp = log(tmp);
  tmp /= beta;
  tmp += 1.0/(ABSOLUTE_TEMP + beta_calib_temp);
  tmp = 1.0/tmp;
  tmp -= ABSOLUTE_TEMP;
  return tmp;
}

float ntcGetTempFromRatioF_RBOT(float ratio)
{
  float r_ntc = (1-ratio)*shunt_resistor_ohmn/(ratio);
  float tmp = r_ntc/ntc_resistance;
  tmp = log(tmp);
  tmp /= beta;
  tmp += 1.0/(ABSOLUTE_TEMP + beta_calib_temp);
  tmp = 1.0/tmp;
  tmp -= ABSOLUTE_TEMP;
  return tmp;
}

float ntcGetTempFromRatioF_RTOP(float ratio)
{
//  float r_ntc = (1-ratio)*shunt_resistor_ohmn/ratio;
  float r_ntc = (ratio)*shunt_resistor_ohmn/(1-ratio);
  float tmp = r_ntc/ntc_resistance;
  tmp = log(tmp);
  tmp /= beta;
  tmp += 1.0/(ABSOLUTE_TEMP + beta_calib_temp);
  tmp = 1.0/tmp;
  tmp -= ABSOLUTE_TEMP;
  return tmp;
}

float ntcGetTempFromRatioF_RTOP_Ex(float ratio, float r_shunt, float r_ntc_norm, uint16_t _beta, uint16_t _betaTemp)
{
//  float r_ntc = (1-ratio)*shunt_resistor_ohmn/ratio;
  float r_ntc = (ratio)*r_shunt/(1-ratio);
  float tmp = r_ntc/r_ntc_norm;
  tmp = log(tmp);
  tmp /= _beta;
  tmp += 1.0/(ABSOLUTE_TEMP + _betaTemp);
  tmp = 1.0/tmp;
  tmp -= ABSOLUTE_TEMP;
  return tmp;
}

int16_t ntcCalculateTempD10(int16_t mv, int16_t vdrv_mv, int16_t r_shunt, int16_t r_ntc_norm,int16_t beta, uint8_t betaTemp)
{
//  double r_ntc = (double)(vdrv_mv - mv)*r_shunt/(double)vdrv_mv;
  double r_ntc = (double)(mv)*r_shunt/(double)(vdrv_mv - mv);
  double tmp = r_ntc/(double)r_ntc_norm/1000.;
  tmp = log(tmp);
  tmp /= (double)beta;
  tmp += 1.0/(ABSOLUTE_TEMP + betaTemp);
  tmp = 1.0/tmp;
  tmp -= ABSOLUTE_TEMP;
  return (int16_t)(tmp * 10);
}

int16_t ntcCalculateTempD10_R(uint32_t r, uint32_t r_ntc_norm,int16_t beta, uint8_t betaTemp)
{
  double r_ntc = (double)(r);
  double tmp = r_ntc/(double)r_ntc_norm;
  tmp = log(tmp);
  tmp /= (double)beta;
  tmp += 1.0/(ABSOLUTE_TEMP + betaTemp);
  tmp = 1.0/tmp;
  tmp -= ABSOLUTE_TEMP;
  return (int16_t)(tmp * 10);
}

void ntcInit(double vdrv, double shunt_res, double ntc_res, double b, double beta_temp)
{
  drive_voltage = vdrv;
  shunt_resistor_ohmn = shunt_res;
  beta = b;
  ntc_resistance = ntc_res;
  beta_calib_temp = beta_temp;
}