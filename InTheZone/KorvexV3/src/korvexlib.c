#include "main.h"
#include "constants.h"
#include "korvexlib.h"

// functions

/*-----------------------------------------------------------------------------*/
/*  Drive and lift control */
/*-----------------------------------------------------------------------------*/
void driveControl(int chassisControlLeft, int chassisControlRight) {
  // chassis control
  if (isReverse ==
      true) { // if in reverse, invert and switch sides for normal turning
    motorSet(driveLeft, (chassisControlRight * -1));
    motorSet(driveRight, (chassisControlLeft * -1));
  } else if (isReverse == false) { // if in normal operator, do not invert
    motorSet(driveRight, (chassisControlRight));
    motorSet(driveLeft, (chassisControlLeft));
  }
}

void dr4bControl(int dr4bControl) {
  // lift control
  if (dr4bControl > 5 || dr4bControl < -5) // if driver is trying to control dr4b, disable autostacker and let them
  {
    autoStackerEnabled = false;;
    motorSet(dr4bLeft, (dr4bControl));
    motorSet(dr4bRight, dr4bControl * -1);
  }
  else {
    autoStackerEnabled = true;
  }
}

/*-----------------------------------------------------------------------------*/
/*  Toggle fine control for drive */
/*-----------------------------------------------------------------------------*/
void fineControlToggle(int fineBtn, int fineBtn2, int reverseBtn, int reverseBtn2) {
  // fine control toggle
  if (fineBtn == 1) { // toggle it on
    isFineControl = true;
    fineControl = .5;
  }
  if (fineBtn2 == 1) { // toggle it off
    isFineControl = false;
    fineControl = 1;
  }
  //reverse toggle
    if (reverseBtn == 1) { // toggle it on
    isReverse = true;
    fineControl = -1;
  }
  if (reverseBtn2 == 1) { // toggle it off
    isReverse = false;
    fineControl = 1;
  }
}

/*-----------------------------------------------------------------------------*/
/*  Mobile goal control */
/*-----------------------------------------------------------------------------*/
void mobileGoalControl(int moboLiftBtnUp, int moboLiftBtnDown) {
  // mobo lift control
  if (moboLiftBtnUp == 1) {
    motorSet(4, 127);
  }
  if (moboLiftBtnDown == 1) {
    motorSet(4, -127);
  }
  if (moboLiftBtnUp == 0 && moboLiftBtnDown == 0) {
    motorSet(4, 0);
  }
}

/*-----------------------------------------------------------------------------*/
/*  Cone handler control */
/*-----------------------------------------------------------------------------*/
void coneHandlerControl(int clawBtnUp, int clawBtnDown, int chainControl) {
  if (chainControl > 5 || chainControl < -5) // if driver is trying to control chain, disable autostacker and let them
  {
    autoStackerEnabled = false;
    motorSet(chainBar, chainControl * -1);
  } else {
    autoStackerEnabled = true;
  }
  // claw control
  if (clawBtnUp == 1) {
    // move up
    motorSet(claw, -40);
  }
  if (clawBtnDown == 1) {
    // move down
    motorSet(claw, 40);
  }
  if (clawBtnUp == 0 && clawBtnDown == 0) {
    // dont move
    motorSet(claw, 0);
  }
}

void autoStackControl(int incrementUpBtn, int incrementDownBtn, int incrementResetBtn, int driverloadBtn, int fieldloadBtn, int incrementUpNoFuncBtn) {
  if (driverloadBtn == 1) {
    isDriverloadGlobal = true;
  }
  if (fieldloadBtn == 1) {
    isDriverloadGlobal = false;
  }
  if (incrementUpBtn == 1) { 
    coneIncrementGlobal = coneIncrementGlobal + 1;
    autoStacker(coneIncrementGlobal, isDriverloadGlobal);
    delay(500);
  }
  if (incrementDownBtn == 1 && incrementUpBtn == 0) {
    coneIncrementGlobal = coneIncrementGlobal = coneIncrementGlobal - 1;
    delay(500);
  }
  if (incrementResetBtn == 1) {
    coneIncrementGlobal = 0;
  }
  if (incrementUpNoFuncBtn == 1) {
    coneIncrementGlobal = coneIncrementGlobal = coneIncrementGlobal + 1;
    delay(500);
  }
}

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
void liftTo(int liftTarget, int chainTarget, int waitTo) {
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
  int count = 0;
  liftLastError = 0;
  chainLastError = 0;
  while (true) {
    if (count == (waitTo / 100)) {
      motorSet(dr4bLeft, 0);
      motorSet(dr4bRight, 0);
      motorSet(chainBar, 0);
      return;
    } else {
      // calculate error
      liftError =
          (liftTarget -
           ((encoderGet(dr4bleftencoder) + encoderGet(dr4brightencoder)) / 2));
      chainError = (chainTarget - encoderGet(chainencoder));

      // calculate PD
      liftP = (liftError * 8);
      liftD = ((liftError - liftLastError) * 3);
      chainP = (chainError * (2));
      chainD = ((chainError - chainLastError) * 0);

      // calculate drive
      liftDrive = (liftP + liftD);
      chainDrive = (chainP + chainD);

      // set motor to drive
      motorSet(dr4bLeft, (liftDrive));
      motorSet(dr4bRight, liftDrive * -1);
      motorSet(chainBar, chainDrive);
      printf("lift error %d\n", liftError);
      printf("lift drive %d\n", liftDrive);
      printf("chain error %d\n", chainError);
      printf("chain drive %d\n", chainDrive);
      count = count + 1;
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
        motorSet(claw, 30);
        liftTo(0, 20, 1800);
        motorSet(claw, -127);
        delay(300);
        motorSet(claw, -5);
        liftTo(0, 350, 1000);
        motorSet(claw, 0);
        break;
      case 2:
        motorSet(claw, 30);
        liftTo(5, 40, 1700);
        motorSet(claw, -90);
        liftTo(5, 40, 600);
        motorSet(claw, 5);
        liftTo(0, 350, 1000);
        motorSet(claw, 0);
        break;
      case 3:
        motorSet(claw, 30);
        liftTo(20, 85, 1700);
        motorSet(claw, -90);
        liftTo(20, 85, 500);
        motorSet(claw, 5);
        liftTo(0, 350, 1000);
        motorSet(claw, 0);
        break;
      case 4:
        motorSet(claw, 30);
        liftTo(28, 100, 2000);
        motorSet(claw, -90);
        liftTo(28, 100, 500);
        motorSet(claw, 5);
        liftTo(0, 350, 1000);
        motorSet(claw, 0);
        break;
      case 5:
        motorSet(claw, 30);
        liftTo(35, 75, 2200);
        motorSet(claw, -90);
        liftTo(35, 75, 500);
        motorSet(claw, 5);
        liftTo(0, 350, 1000);
        motorSet(claw, 0);
        break;
      case 6:
        motorSet(claw, 30);
        liftTo(40, 105, 2400);
        motorSet(claw, -90);
        liftTo(40, 105, 500);
        motorSet(claw, 5);
        liftTo(0, 350, 1000);
        motorSet(claw, 0);
        break;
      case 7:
        motorSet(claw, 30);
        liftTo(50, 125, 2400);
        motorSet(claw, -90);
        liftTo(50, 125, 500);
        motorSet(claw, 5);
        liftTo(0, 350, 1000);
        motorSet(claw, 0);
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