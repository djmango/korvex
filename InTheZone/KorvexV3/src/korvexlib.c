#include "main.h"
#include "constants.h"

/*-----------------------------------------------------------------------------*/
/*  An argument based encoder pid, for drive left */
/*-----------------------------------------------------------------------------*/
void driveLeftPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd)
{
    float pidError;
    float pidLastError;
    float pidIntegral;
    float pidDerivative;
    int pidDrive;
    pidLastError = 0;
    pidIntegral = 0;
    while (true)
    {
        // calculate error
        pidError = encoderCalcValue - encoderTarget;

        // integral - if Ki is not 0
        if (pidKi != 0)
        {
            // If we are inside controlable window then integrate the error
            if (abs(pidError) < PID_INTEGRAL_LIMIT)
                pidIntegral = pidIntegral + pidError;
            else
                pidIntegral = 0;
        }
        else
            pidIntegral = 0;

        // calculate the derivative
        pidDerivative = pidError - pidLastError;
        pidLastError = pidError;

        // calculate drive
        pidDrive = (pidKp * pidError) + (pidKi * pidIntegral) + (pidKd * pidDerivative);
        // limit drive
        if (pidDrive > PID_DRIVE_MAX)
            pidDrive = PID_DRIVE_MAX;
        if (pidDrive < PID_DRIVE_MIN)
            pidDrive = PID_DRIVE_MIN;
        // send back
        motorSet(driveLeft, pidDrive);
        delay(50);
        // Run at 50Hz
    }
}

/*-----------------------------------------------------------------------------*/
/*  An argument based encoder pid, for drive right */
/*-----------------------------------------------------------------------------*/
void driveRightPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd)
{
    float pidError;
    float pidLastError;
    float pidIntegral;
    float pidDerivative;
    int pidDrive;
    pidLastError = 0;
    pidIntegral = 0;
    while (true)
    {
        // calculate error
        pidError = encoderCalcValue - encoderTarget;

        // integral - if Ki is not 0
        if (pidKi != 0)
        {
            // If we are inside controlable window then integrate the error
            if (abs(pidError) < PID_INTEGRAL_LIMIT)
                pidIntegral = pidIntegral + pidError;
            else
                pidIntegral = 0;
        }
        else
            pidIntegral = 0;

        // calculate the derivative
        pidDerivative = pidError - pidLastError;
        pidLastError = pidError;

        // calculate drive
        pidDrive = (pidKp * pidError) + (pidKi * pidIntegral) + (pidKd * pidDerivative);
        // limit drive
        if (pidDrive > PID_DRIVE_MAX)
            pidDrive = PID_DRIVE_MAX;
        if (pidDrive < PID_DRIVE_MIN)
            pidDrive = PID_DRIVE_MIN;
        // send back
        motorSet(driveRight, pidDrive);
        delay(50);
        // Run at 50Hz
    }
}

/*-----------------------------------------------------------------------------*/
/*  An argument based encoder pid, for dr4b left */
/*-----------------------------------------------------------------------------*/
void dr4bLeftPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd) {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  int pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (true) {
    // calculate error
    pidError = encoderCalcValue - encoderTarget;

    // integral - if Ki is not 0
    if (pidKi != 0) {
      // If we are inside controlable window then integrate the error
      if (abs(pidError) < PID_INTEGRAL_LIMIT)
        pidIntegral = pidIntegral + pidError;
      else
        pidIntegral = 0;
    } else
      pidIntegral = 0;

    // calculate the derivative
    pidDerivative = pidError - pidLastError;
    pidLastError = pidError;

    // calculate drive
    pidDrive =
        (pidKp * pidError) + (pidKi * pidIntegral) + (pidKd * pidDerivative);
    // limit drive
    if (pidDrive > PID_DRIVE_MAX)
      pidDrive = PID_DRIVE_MAX;
    if (pidDrive < PID_DRIVE_MIN)
      pidDrive = PID_DRIVE_MIN;
    // send back
    motorSet(dr4bLeft, pidDrive);
    delay(50);
    // Run at 50Hz
  }
}

/*-----------------------------------------------------------------------------*/
/*  An argument based encoder pid, for dr4b right */
/*-----------------------------------------------------------------------------*/
void dr4bRightPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd) {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  int pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (true) {
    // calculate error
    pidError = encoderCalcValue - encoderTarget;

    // integral - if Ki is not 0
    if (pidKi != 0) {
      // If we are inside controlable window then integrate the error
      if (abs(pidError) < PID_INTEGRAL_LIMIT)
        pidIntegral = pidIntegral + pidError;
      else
        pidIntegral = 0;
    } else
      pidIntegral = 0;

    // calculate the derivative
    pidDerivative = pidError - pidLastError;
    pidLastError = pidError;

    // calculate drive
    pidDrive =
        (pidKp * pidError) + (pidKi * pidIntegral) + (pidKd * pidDerivative);
    // limit drive
    if (pidDrive > PID_DRIVE_MAX)
      pidDrive = PID_DRIVE_MAX;
    if (pidDrive < PID_DRIVE_MIN)
      pidDrive = PID_DRIVE_MIN;
    // send back
    motorSet(dr4bRight, pidDrive);
    delay(50);
    // Run at 50Hz
  }
}

/*-----------------------------------------------------------------------------*/
/*  An argument based encoder pid, for dr4b right */
/*-----------------------------------------------------------------------------*/
void chainPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd) {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  int pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (true) {
    // calculate error
    pidError = encoderCalcValue - encoderTarget;

    // integral - if Ki is not 0
    if (pidKi != 0) {
      // If we are inside controlable window then integrate the error
      if (abs(pidError) < PID_INTEGRAL_LIMIT)
        pidIntegral = pidIntegral + pidError;
      else
        pidIntegral = 0;
    } else
      pidIntegral = 0;

    // calculate the derivative
    pidDerivative = pidError - pidLastError;
    pidLastError = pidError;

    // calculate drive
    pidDrive =
        (pidKp * pidError) + (pidKi * pidIntegral) + (pidKd * pidDerivative);
    // limit drive
    if (pidDrive > PID_DRIVE_MAX)
      pidDrive = PID_DRIVE_MAX;
    if (pidDrive < PID_DRIVE_MIN)
      pidDrive = PID_DRIVE_MIN;
    // send back
    motorSet(chainBar, pidDrive);
    delay(50);
    // Run at 50Hz
  }
}

/*-----------------------------------------------------------------------------*/
/*  An argument based pid autostacker, which sets the targets of each system,  */
/*  in accordance to presets corresponding to the cones stacked                */
/*-----------------------------------------------------------------------------*/
void autoStacker(int coneIncrement, bool isDriverload) { // cone increment will decide what function will run, each is specific to the height
    if (isDriverload == false) { // if we are not stacking driver load, assume we are stacking from ground
      switch (coneIncrement) {
      case 1: // stacking first cone
        break;
      default:
        break;
      }
    }
    if (isDriverload == true) { // if we are stacking driver loads, use the corresponding set of presets
      switch (coneIncrement) {
      case 1:
        break;
      default:
        break;
      }
    }
}