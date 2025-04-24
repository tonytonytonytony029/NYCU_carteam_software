/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: subsystem1.c
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

#include "subsystem1.h"
#include "rtwtypes.h"
#include <math.h>
#include "subsystem1_private.h"

/* Named constants for Chart: '<S2>/Chart' */
#define subsyste_IN_APPS1critical_state ((uint8_T)1U)
#define subsyste_IN_APPS2critical_state ((uint8_T)2U)
#define subsystem1_IN_fail_state       ((uint8_T)3U)
#define subsystem1_IN_normal_state     ((uint8_T)4U)

/* Named constants for Chart: '<S4>/Chart' */
#define subsystem1_IN_Off              ((uint8_T)1U)
#define subsystem1_IN_On               ((uint8_T)2U)

/* Continuous states */
X_subsystem1_T subsystem1_X;

/* Block states (default storage) */
DW_subsystem1_T subsystem1_DW;

/* External inputs (root inport signals with default storage) */
ExtU_subsystem1_T subsystem1_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_subsystem1_T subsystem1_Y;

/* Real-time model */
static RT_MODEL_subsystem1_T subsystem1_M_;
RT_MODEL_subsystem1_T *const subsystem1_M = &subsystem1_M_;

/*
 * This function updates continuous states using the ODE3 fixed-step
 * solver algorithm
 */
static void rt_ertODEUpdateContinuousStates(RTWSolverInfo *si )
{
  /* Solver Matrices */
  static const real_T rt_ODE3_A[3] = {
    1.0/2.0, 3.0/4.0, 1.0
  };

  static const real_T rt_ODE3_B[3][3] = {
    { 1.0/2.0, 0.0, 0.0 },

    { 0.0, 3.0/4.0, 0.0 },

    { 2.0/9.0, 1.0/3.0, 4.0/9.0 }
  };

  time_T t = rtsiGetT(si);
  time_T tnew = rtsiGetSolverStopTime(si);
  time_T h = rtsiGetStepSize(si);
  real_T *x = rtsiGetContStates(si);
  ODE3_IntgData *id = (ODE3_IntgData *)rtsiGetSolverData(si);
  real_T *y = id->y;
  real_T *f0 = id->f[0];
  real_T *f1 = id->f[1];
  real_T *f2 = id->f[2];
  real_T hB[3];
  int_T i;
  int_T nXc = 2;
  rtsiSetSimTimeStep(si,MINOR_TIME_STEP);

  /* Save the state values at time t in y, we'll use x as ynew. */
  (void) memcpy(y, x,
                (uint_T)nXc*sizeof(real_T));

  /* Assumes that rtsiSetT and ModelOutputs are up-to-date */
  /* f0 = f(t,y) */
  rtsiSetdX(si, f0);
  subsystem1_derivatives();

  /* f(:,2) = feval(odefile, t + hA(1), y + f*hB(:,1), args(:)(*)); */
  hB[0] = h * rt_ODE3_B[0][0];
  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[0]);
  rtsiSetdX(si, f1);
  subsystem1_step();
  subsystem1_derivatives();

  /* f(:,3) = feval(odefile, t + hA(2), y + f*hB(:,2), args(:)(*)); */
  for (i = 0; i <= 1; i++) {
    hB[i] = h * rt_ODE3_B[1][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[1]);
  rtsiSetdX(si, f2);
  subsystem1_step();
  subsystem1_derivatives();

  /* tnew = t + hA(3);
     ynew = y + f*hB(:,3); */
  for (i = 0; i <= 2; i++) {
    hB[i] = h * rt_ODE3_B[2][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1] + f2[i]*hB[2]);
  }

  rtsiSetT(si, tnew);
  rtsiSetSimTimeStep(si,MAJOR_TIME_STEP);
}

/* Model step function */
void subsystem1_step(void)
{
  real_T rtb_Divide;
  real_T rtb_Divide1;
  real_T tmp;
  boolean_T Switch1;
  boolean_T Switch3;
  boolean_T rtb_Limit_check;
  if (rtmIsMajorTimeStep(subsystem1_M)) {
    /* set solver stop time */
    rtsiSetSolverStopTime(&subsystem1_M->solverInfo,
                          ((subsystem1_M->Timing.clockTick0+1)*
      subsystem1_M->Timing.stepSize0));
  }                                    /* end MajorTimeStep */

  /* Update absolute time of base rate at minor time step */
  if (rtmIsMinorTimeStep(subsystem1_M)) {
    subsystem1_M->Timing.t[0] = rtsiGetT(&subsystem1_M->solverInfo);
  }

  /* Product: '<S5>/Divide' incorporates:
   *  Constant: '<S5>/Constant1'
   *  Constant: '<S5>/Constant3'
   *  StateSpace: '<S5>/Analog Filter Design2'
   *  Sum: '<S5>/Add3'
   */
  rtb_Divide = (subsystem1_X.AnalogFilterDesign2_CSTATE - 0.75) *
    0.3236245954692557;

  /* Switch: '<S2>/Switch' incorporates:
   *  Constant: '<S2>/Constant'
   */
  if (rtb_Divide > 0.0) {
    tmp = rtb_Divide;
  } else {
    tmp = 2.0;
  }

  /* Switch: '<S2>/Switch1' incorporates:
   *  Switch: '<S2>/Switch'
   */
  Switch1 = !(tmp > 1.0);

  /* Product: '<S5>/Divide1' incorporates:
   *  Constant: '<S5>/Constant2'
   *  Constant: '<S5>/Constant4'
   *  StateSpace: '<S5>/Analog Filter Design3'
   *  Sum: '<S5>/Add5'
   */
  rtb_Divide1 = (subsystem1_X.AnalogFilterDesign3_CSTATE - 0.375) *
    0.64724919093851141;

  /* Switch: '<S2>/Switch2' incorporates:
   *  Constant: '<S2>/Constant3'
   */
  if (rtb_Divide1 > 0.0) {
    tmp = rtb_Divide1;
  } else {
    tmp = 2.0;
  }

  /* Switch: '<S2>/Switch3' incorporates:
   *  Switch: '<S2>/Switch2'
   */
  Switch3 = !(tmp > 1.0);
  if (rtmIsMajorTimeStep(subsystem1_M)) {
    /* Chart: '<S2>/Chart' */
    if (subsystem1_DW.is_active_c2_subsystem1 == 0U) {
      subsystem1_DW.is_active_c2_subsystem1 = 1U;
      subsystem1_DW.durationCounter_3 = 0U;
      subsystem1_DW.durationCounter_2 = 0U;
      subsystem1_DW.durationCounter_1_d = 0U;
      subsystem1_DW.is_c2_subsystem1 = subsystem1_IN_normal_state;
      rtb_Limit_check = true;

      /* Outport: '<Root>/critical_error' */
      subsystem1_Y.critical_error = false;
    } else {
      switch (subsystem1_DW.is_c2_subsystem1) {
       case subsyste_IN_APPS1critical_state:
        if (Switch1) {
          subsystem1_DW.durationCounter_3 = 0U;
          subsystem1_DW.durationCounter_2 = 0U;
          subsystem1_DW.durationCounter_1_d = 0U;
          subsystem1_DW.is_c2_subsystem1 = subsystem1_IN_normal_state;
          rtb_Limit_check = true;

          /* Outport: '<Root>/critical_error' */
          subsystem1_Y.critical_error = false;
        } else {
          if (Switch3) {
            subsystem1_DW.durationCounter_1_k = 0U;
          }

          if (subsystem1_DW.durationCounter_1_k > 2000U) {
            subsystem1_DW.is_c2_subsystem1 = subsystem1_IN_fail_state;
            rtb_Limit_check = false;

            /* Outport: '<Root>/critical_error' */
            subsystem1_Y.critical_error = true;
          } else {
            rtb_Limit_check = true;

            /* Outport: '<Root>/critical_error' */
            subsystem1_Y.critical_error = true;
          }
        }
        break;

       case subsyste_IN_APPS2critical_state:
        if (Switch3) {
          subsystem1_DW.durationCounter_3 = 0U;
          subsystem1_DW.durationCounter_2 = 0U;
          subsystem1_DW.durationCounter_1_d = 0U;
          subsystem1_DW.is_c2_subsystem1 = subsystem1_IN_normal_state;
          rtb_Limit_check = true;

          /* Outport: '<Root>/critical_error' */
          subsystem1_Y.critical_error = false;
        } else {
          if (Switch1) {
            subsystem1_DW.durationCounter_1_n = 0U;
          }

          if (subsystem1_DW.durationCounter_1_n > 2000U) {
            subsystem1_DW.is_c2_subsystem1 = subsystem1_IN_fail_state;
            rtb_Limit_check = false;

            /* Outport: '<Root>/critical_error' */
            subsystem1_Y.critical_error = true;
          } else {
            rtb_Limit_check = true;

            /* Outport: '<Root>/critical_error' */
            subsystem1_Y.critical_error = true;
          }
        }
        break;

       case subsystem1_IN_fail_state:
        rtb_Limit_check = false;

        /* Outport: '<Root>/critical_error' */
        subsystem1_Y.critical_error = true;
        break;

       default:
        /* case IN_normal_state: */
        if (Switch1 || Switch3) {
          subsystem1_DW.durationCounter_2 = 0U;
        }

        if (subsystem1_DW.durationCounter_2 > 2000U) {
          subsystem1_DW.is_c2_subsystem1 = subsystem1_IN_fail_state;
          rtb_Limit_check = false;

          /* Outport: '<Root>/critical_error' */
          subsystem1_Y.critical_error = true;
        } else {
          if (Switch1) {
            subsystem1_DW.durationCounter_1_d = 0U;
          }

          if (subsystem1_DW.durationCounter_1_d > 2000U) {
            subsystem1_DW.durationCounter_1_k = 0U;
            subsystem1_DW.is_c2_subsystem1 = subsyste_IN_APPS1critical_state;
            rtb_Limit_check = true;

            /* Outport: '<Root>/critical_error' */
            subsystem1_Y.critical_error = true;
          } else {
            if (Switch3) {
              subsystem1_DW.durationCounter_3 = 0U;
            }

            if (subsystem1_DW.durationCounter_3 > 2000U) {
              subsystem1_DW.durationCounter_1_n = 0U;
              subsystem1_DW.is_c2_subsystem1 = subsyste_IN_APPS2critical_state;
              rtb_Limit_check = true;

              /* Outport: '<Root>/critical_error' */
              subsystem1_Y.critical_error = true;
            } else {
              rtb_Limit_check = true;

              /* Outport: '<Root>/critical_error' */
              subsystem1_Y.critical_error = false;
            }
          }
        }
        break;
      }
    }

    Switch1 = !Switch1;
    if (Switch1) {
      subsystem1_DW.durationCounter_1_d++;
    } else {
      subsystem1_DW.durationCounter_1_d = 0U;
    }

    Switch3 = !Switch3;
    if (Switch1 && Switch3) {
      subsystem1_DW.durationCounter_2++;
    } else {
      subsystem1_DW.durationCounter_2 = 0U;
    }

    if (Switch3) {
      subsystem1_DW.durationCounter_3++;
      subsystem1_DW.durationCounter_1_k++;
    } else {
      subsystem1_DW.durationCounter_3 = 0U;
      subsystem1_DW.durationCounter_1_k = 0U;
    }

    if (Switch1) {
      subsystem1_DW.durationCounter_1_n++;
    } else {
      subsystem1_DW.durationCounter_1_n = 0U;
    }

    /* End of Chart: '<S2>/Chart' */
  }

  /* RelationalOperator: '<S4>/Relational Operator' incorporates:
   *  Abs: '<S4>/Abs'
   *  Constant: '<S4>/Constant4'
   *  Sum: '<S4>/Add6'
   */
  Switch3 = (fabs(rtb_Divide - rtb_Divide1) < 0.1);
  if (rtmIsMajorTimeStep(subsystem1_M)) {
    /* Chart: '<S4>/Chart' */
    if (subsystem1_DW.is_active_c1_subsystem1 == 0U) {
      subsystem1_DW.is_active_c1_subsystem1 = 1U;
      subsystem1_DW.durationCounter_1 = 0U;
      subsystem1_DW.is_c1_subsystem1 = subsystem1_IN_On;
      Switch1 = true;
    } else if (subsystem1_DW.is_c1_subsystem1 == subsystem1_IN_Off) {
      Switch1 = false;
    } else {
      /* case IN_On: */
      if (Switch3) {
        subsystem1_DW.durationCounter_1 = 0U;
      }

      if (subsystem1_DW.durationCounter_1 > 2000U) {
        subsystem1_DW.is_c1_subsystem1 = subsystem1_IN_Off;
        Switch1 = false;
      } else {
        Switch1 = true;
      }
    }

    if (!Switch3) {
      subsystem1_DW.durationCounter_1++;
    } else {
      subsystem1_DW.durationCounter_1 = 0U;
    }

    /* End of Chart: '<S4>/Chart' */

    /* If: '<S3>/If' */
    if (rtsiIsModeUpdateTimeStep(&subsystem1_M->solverInfo)) {
      subsystem1_DW.If_ActiveSubsystem = (int8_T)((!rtb_Limit_check) ||
        (!Switch1));
    }

    if (subsystem1_DW.If_ActiveSubsystem == 0) {
      /* Outputs for IfAction SubSystem: '<S3>/If Action Subsystem' incorporates:
       *  ActionPort: '<S7>/Action Port'
       */
      /* Outport: '<Root>/APPS_output' incorporates:
       *  Constant: '<S5>/Constant5'
       *  Product: '<S5>/Divide2'
       *  SignalConversion generated from: '<S7>/APPS'
       *  Sum: '<S5>/Add1'
       */
      subsystem1_Y.APPS_output = (rtb_Divide + rtb_Divide1) / 2.0;

      /* End of Outputs for SubSystem: '<S3>/If Action Subsystem' */
    } else {
      /* Outputs for IfAction SubSystem: '<S3>/If Action Subsystem1' incorporates:
       *  ActionPort: '<S8>/Action Port'
       */
      /* Outport: '<Root>/APPS_output' incorporates:
       *  Constant: '<S3>/Constant'
       *  SignalConversion generated from: '<S8>/APPS'
       */
      subsystem1_Y.APPS_output = 0.0;

      /* End of Outputs for SubSystem: '<S3>/If Action Subsystem1' */
    }

    /* End of If: '<S3>/If' */
  }

  if (rtmIsMajorTimeStep(subsystem1_M)) {
    rt_ertODEUpdateContinuousStates(&subsystem1_M->solverInfo);

    /* Update absolute time for base rate */
    /* The "clockTick0" counts the number of times the code of this task has
     * been executed. The absolute time is the multiplication of "clockTick0"
     * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
     * overflow during the application lifespan selected.
     */
    ++subsystem1_M->Timing.clockTick0;
    subsystem1_M->Timing.t[0] = rtsiGetSolverStopTime(&subsystem1_M->solverInfo);

    {
      /* Update absolute timer for sample time: [0.001s, 0.0s] */
      /* The "clockTick1" counts the number of times the code of this task has
       * been executed. The resolution of this integer timer is 0.001, which is the step size
       * of the task. Size of "clockTick1" ensures timer will not overflow during the
       * application lifespan selected.
       */
      subsystem1_M->Timing.clockTick1++;
    }
  }                                    /* end MajorTimeStep */
}

/* Derivatives for root system: '<Root>' */
void subsystem1_derivatives(void)
{
  XDot_subsystem1_T *_rtXdot;
  _rtXdot = ((XDot_subsystem1_T *) subsystem1_M->derivs);

  /* Derivatives for StateSpace: '<S5>/Analog Filter Design2' incorporates:
   *  Inport: '<Root>/pedal1'
   */
  _rtXdot->AnalogFilterDesign2_CSTATE = -500.0 *
    subsystem1_X.AnalogFilterDesign2_CSTATE;
  _rtXdot->AnalogFilterDesign2_CSTATE += 500.0 * subsystem1_U.pedal1;

  /* Derivatives for StateSpace: '<S5>/Analog Filter Design3' incorporates:
   *  Inport: '<Root>/pedal2'
   */
  _rtXdot->AnalogFilterDesign3_CSTATE = -500.0 *
    subsystem1_X.AnalogFilterDesign3_CSTATE;
  _rtXdot->AnalogFilterDesign3_CSTATE += 500.0 * subsystem1_U.pedal2;
}

/* Model initialize function */
void subsystem1_initialize(void)
{
  /* Registration code */
  {
    /* Setup solver object */
    rtsiSetSimTimeStepPtr(&subsystem1_M->solverInfo,
                          &subsystem1_M->Timing.simTimeStep);
    rtsiSetTPtr(&subsystem1_M->solverInfo, &rtmGetTPtr(subsystem1_M));
    rtsiSetStepSizePtr(&subsystem1_M->solverInfo,
                       &subsystem1_M->Timing.stepSize0);
    rtsiSetdXPtr(&subsystem1_M->solverInfo, &subsystem1_M->derivs);
    rtsiSetContStatesPtr(&subsystem1_M->solverInfo, (real_T **)
                         &subsystem1_M->contStates);
    rtsiSetNumContStatesPtr(&subsystem1_M->solverInfo,
      &subsystem1_M->Sizes.numContStates);
    rtsiSetNumPeriodicContStatesPtr(&subsystem1_M->solverInfo,
      &subsystem1_M->Sizes.numPeriodicContStates);
    rtsiSetPeriodicContStateIndicesPtr(&subsystem1_M->solverInfo,
      &subsystem1_M->periodicContStateIndices);
    rtsiSetPeriodicContStateRangesPtr(&subsystem1_M->solverInfo,
      &subsystem1_M->periodicContStateRanges);
    rtsiSetErrorStatusPtr(&subsystem1_M->solverInfo, (&rtmGetErrorStatus
      (subsystem1_M)));
    rtsiSetRTModelPtr(&subsystem1_M->solverInfo, subsystem1_M);
  }

  rtsiSetSimTimeStep(&subsystem1_M->solverInfo, MAJOR_TIME_STEP);
  subsystem1_M->intgData.y = subsystem1_M->odeY;
  subsystem1_M->intgData.f[0] = subsystem1_M->odeF[0];
  subsystem1_M->intgData.f[1] = subsystem1_M->odeF[1];
  subsystem1_M->intgData.f[2] = subsystem1_M->odeF[2];
  subsystem1_M->contStates = ((X_subsystem1_T *) &subsystem1_X);
  rtsiSetSolverData(&subsystem1_M->solverInfo, (void *)&subsystem1_M->intgData);
  rtsiSetIsMinorTimeStepWithModeChange(&subsystem1_M->solverInfo, false);
  rtsiSetSolverName(&subsystem1_M->solverInfo,"ode3");
  rtmSetTPtr(subsystem1_M, &subsystem1_M->Timing.tArray[0]);
  subsystem1_M->Timing.stepSize0 = 0.001;

  /* Start for If: '<S3>/If' */
  subsystem1_DW.If_ActiveSubsystem = -1;

  /* InitializeConditions for StateSpace: '<S5>/Analog Filter Design2' */
  subsystem1_X.AnalogFilterDesign2_CSTATE = 0.0;

  /* InitializeConditions for StateSpace: '<S5>/Analog Filter Design3' */
  subsystem1_X.AnalogFilterDesign3_CSTATE = 0.0;
}

/* Model terminate function */
void subsystem1_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
