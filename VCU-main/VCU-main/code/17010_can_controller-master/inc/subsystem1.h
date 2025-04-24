/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: subsystem1.h
 *
 * Code generated for Simulink model 'subsystem1'.
 *
 * Model version                  : 2.0
 * Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
 * C/C++ source code generated on : Fri Apr 21 17:05:51 2023
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_subsystem1_h_
#define RTW_HEADER_subsystem1_h_
#ifndef subsystem1_COMMON_INCLUDES_
#define subsystem1_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#endif                                 /* subsystem1_COMMON_INCLUDES_ */

#include "subsystem1_types.h"
#include <string.h>

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
#define rtmGetStopRequested(rtm)       ((rtm)->Timing.stopRequestedFlag)
#endif

#ifndef rtmSetStopRequested
#define rtmSetStopRequested(rtm, val)  ((rtm)->Timing.stopRequestedFlag = (val))
#endif

#ifndef rtmGetStopRequestedPtr
#define rtmGetStopRequestedPtr(rtm)    (&((rtm)->Timing.stopRequestedFlag))
#endif

#ifndef rtmGetT
#define rtmGetT(rtm)                   (rtmGetTPtr((rtm))[0])
#endif

#ifndef rtmGetTPtr
#define rtmGetTPtr(rtm)                ((rtm)->Timing.t)
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  uint32_T durationCounter_1;          /* '<S4>/Chart' */
  uint32_T durationCounter_1_d;        /* '<S2>/Chart' */
  uint32_T durationCounter_2;          /* '<S2>/Chart' */
  uint32_T durationCounter_3;          /* '<S2>/Chart' */
  uint32_T durationCounter_1_k;        /* '<S2>/Chart' */
  uint32_T durationCounter_1_n;        /* '<S2>/Chart' */
  int8_T If_ActiveSubsystem;           /* '<S3>/If' */
  uint8_T is_active_c1_subsystem1;     /* '<S4>/Chart' */
  uint8_T is_c1_subsystem1;            /* '<S4>/Chart' */
  uint8_T is_active_c2_subsystem1;     /* '<S2>/Chart' */
  uint8_T is_c2_subsystem1;            /* '<S2>/Chart' */
} DW_subsystem1_T;

/* Continuous states (default storage) */
typedef struct {
  real_T AnalogFilterDesign2_CSTATE;   /* '<S5>/Analog Filter Design2' */
  real_T AnalogFilterDesign3_CSTATE;   /* '<S5>/Analog Filter Design3' */
} X_subsystem1_T;

/* State derivatives (default storage) */
typedef struct {
  real_T AnalogFilterDesign2_CSTATE;   /* '<S5>/Analog Filter Design2' */
  real_T AnalogFilterDesign3_CSTATE;   /* '<S5>/Analog Filter Design3' */
} XDot_subsystem1_T;

/* State disabled  */
typedef struct {
  boolean_T AnalogFilterDesign2_CSTATE;/* '<S5>/Analog Filter Design2' */
  boolean_T AnalogFilterDesign3_CSTATE;/* '<S5>/Analog Filter Design3' */
} XDis_subsystem1_T;

#ifndef ODE3_INTG
#define ODE3_INTG

/* ODE3 Integration Data */
typedef struct {
  real_T *y;                           /* output */
  real_T *f[3];                        /* derivatives */
} ODE3_IntgData;

#endif

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T pedal1;                       /* '<Root>/pedal1' */
  real_T pedal2;                       /* '<Root>/pedal2' */
} ExtU_subsystem1_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  boolean_T critical_error;            /* '<Root>/critical_error' */
  real_T APPS_output;                  /* '<Root>/APPS_output' */
} ExtY_subsystem1_T;

/* Real-time Model Data Structure */
struct tag_RTM_subsystem1_T {
  const char_T *errorStatus;
  RTWSolverInfo solverInfo;
  X_subsystem1_T *contStates;
  int_T *periodicContStateIndices;
  real_T *periodicContStateRanges;
  real_T *derivs;
  XDis_subsystem1_T *contStateDisabled;
  boolean_T zCCacheNeedsReset;
  boolean_T derivCacheNeedsReset;
  boolean_T CTOutputIncnstWithState;
  real_T odeY[2];
  real_T odeF[3][2];
  ODE3_IntgData intgData;

  /*
   * Sizes:
   * The following substructure contains sizes information
   * for many of the model attributes such as inputs, outputs,
   * dwork, sample times, etc.
   */
  struct {
    int_T numContStates;
    int_T numPeriodicContStates;
    int_T numSampTimes;
  } Sizes;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    uint32_T clockTick0;
    time_T stepSize0;
    uint32_T clockTick1;
    SimTimeStep simTimeStep;
    boolean_T stopRequestedFlag;
    time_T *t;
    time_T tArray[2];
  } Timing;
};

/* Continuous states (default storage) */
extern X_subsystem1_T subsystem1_X;

/* Block states (default storage) */
extern DW_subsystem1_T subsystem1_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_subsystem1_T subsystem1_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_subsystem1_T subsystem1_Y;

/* Model entry point functions */
extern void subsystem1_initialize(void);
extern void subsystem1_step(void);
extern void subsystem1_terminate(void);

/* Real-time Model object */
extern RT_MODEL_subsystem1_T *const subsystem1_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Note that this particular code originates from a subsystem build,
 * and has its own system numbers different from the parent model.
 * Refer to the system hierarchy for this subsystem below, and use the
 * MATLAB hilite_system command to trace the generated code back
 * to the parent model.  For example,
 *
 * hilite_system('final_code2/subsystem1')    - opens subsystem final_code2/subsystem1
 * hilite_system('final_code2/subsystem1/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'final_code2'
 * '<S1>'   : 'final_code2/subsystem1'
 * '<S2>'   : 'final_code2/subsystem1/Amplitude limit check '
 * '<S3>'   : 'final_code2/subsystem1/Arbitration model'
 * '<S4>'   : 'final_code2/subsystem1/Consistency check'
 * '<S5>'   : 'final_code2/subsystem1/Filiter'
 * '<S6>'   : 'final_code2/subsystem1/Amplitude limit check /Chart'
 * '<S7>'   : 'final_code2/subsystem1/Arbitration model/If Action Subsystem'
 * '<S8>'   : 'final_code2/subsystem1/Arbitration model/If Action Subsystem1'
 * '<S9>'   : 'final_code2/subsystem1/Consistency check/Chart'
 */
#endif                                 /* RTW_HEADER_subsystem1_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
