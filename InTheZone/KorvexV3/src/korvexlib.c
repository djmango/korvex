#include "main.h"
#include "constants.h"
#include "korvexlib.h"

/*-----------------------------------------------------------------------------*/
/*  An argument based encoder pid, for drive left */
/*-----------------------------------------------------------------------------*/
void driveLeftPid(int pidKp, int pidKi, int pidKd) {
  int encoderTarget;
  int encoderCalcValue;
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  int pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (true) {
    encoderTarget = driveLeftTarget;
    encoderCalcValue = encoderGet(leftencoder);
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
    motorSet(driveLeft, pidDrive);
    delay(50);
    // Run at 50Hz
  }
}

/*-----------------------------------------------------------------------------*/
/*  An argument based encoder pid, for drive right */
/*-----------------------------------------------------------------------------*/
void driveRightPid(int pidKp, int pidKi, int pidKd) {
  int encoderTarget;
  int encoderCalcValue;
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  int pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (true) {
    encoderTarget = driveRightTarget;
    encoderCalcValue = encoderGet(rightencoder);
    printf("rvalue %d\n", encoderCalcValue);
    printf("rtarget %d\n", encoderTarget);
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
    printf("rdrive %d\n", pidDrive);
    motorSet(driveRight, pidDrive);
    delay(50);
    // Run at 50Hz
  }
}

/*-----------------------------------------------------------------------------*/
/*  An argument based PD for the lift and chainbar. Konsts are hardcoded       */
/*-----------------------------------------------------------------------------*/
void liftTo(int liftTarget, int chainTarget) {
  int liftError = -1; // -1 is a place holder, so it doesnt exit instantly
  int chainError = -1;
  int liftDrive;
  int chainDrive;
  int liftLastError;
  int chainLastError;
  int liftP;
  int chainP;
  int liftD;
  int chainD;
  liftLastError = 0;
  chainLastError = 0;
  while (true) {
    if (liftError == 0 && chainError == 0) {
      // return;
    } else {
      // calculate error
      liftError =
          (liftTarget -
           ((encoderGet(dr4bleftencoder) + encoderGet(dr4brightencoder)) / 2));
      chainError = (chainTarget - encoderGet(chainencoder));

      // calculate PD
      liftP = (liftError * 10);
      liftD = ((liftError - liftLastError) * 3);
      chainP = (chainError * 5);
      chainD = ((chainError - chainLastError) * 5);

      // calculate drive
      liftDrive = (liftP + liftD);
      chainDrive = (chainP + chainD);

      // limit drive
      // if (liftDrive > PID_DRIVE_MAX)
      //   liftDrive = PID_DRIVE_MAX;
      // if (liftDrive < PID_DRIVE_MIN)
      //   liftDrive = PID_DRIVE_MIN;
      if (chainDrive > PID_DRIVE_MAX)
        chainDrive = PID_DRIVE_MAX;
      if (chainDrive < PID_DRIVE_MIN)
        chainDrive = PID_DRIVE_MIN;

      // set motor to drive
      motorSet(dr4bLeft, (liftDrive));
      motorSet(dr4bRight, liftDrive * -1);
      motorSet(chainBar, chainDrive);
      printf("lift error %d\n", liftError);
      printf("lift drive %d\n", liftDrive);
      printf("chain error %d\n", chainError);
      printf("chain drive %d\n", chainDrive);
      delay(100);
    }
  }
}

/*-----------------------------------------------------------------------------*/
/*  An argument based autostacker, which sets the targets of each system,      */
/*  in accordance to presets corresponding to the cones stacked                */
/*-----------------------------------------------------------------------------*/
void autoStacker(int coneIncrement, bool isDriverload) { // cone increment will decide what function will run, each is specific to the height
    if (isDriverload == false) { // if we are not stacking driver load, assume we are stacking from ground
      switch (coneIncrement) {
      case 1: // stacking first cone
        liftTo(50, 40);
        delay(2000);
        liftTo(70, 60);
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