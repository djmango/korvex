#include "main.h"
#include "constants.h"
#include "korvexlib.h"

// functions

/*-----------------------------------------------------------------------------*/
/*  Drive control */
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

/*-----------------------------------------------------------------------------*/
/*  Lift control */
/*-----------------------------------------------------------------------------*/
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
/*  An argument based encoder pd, for drive */
/*-----------------------------------------------------------------------------*/
void driveTo(int leftTarget, int rightTarget, int waitTo) {
  int leftError;
  int rightError;
  int leftDrive;
  int rightDrive;
  int leftLastError = 0;
  int rightLastError = 0;
  int leftP;
  int rightP;
  int leftD;
  int rightD;
  int count = 0;
  while (true) {
    if (count == (waitTo / 100)) {
      return;
    } else {
      // calculate error
      leftError = (leftTarget - encoderGet(leftencoder));
      rightError = (rightTarget - encoderGet(rightencoder));
      printf("%d\n", leftError);
      printf("%d\n", rightError);
      // calculate pd
      leftP = (leftError * .8);
      leftD = ((leftError - leftLastError) * .4);
      rightP = (rightError * .6);
      rightD = ((rightError - rightLastError) * .4);

      // store last error
      leftLastError = leftError;
      rightLastError = rightError;

      // calculate drive
      leftDrive = (leftP + leftD);
      rightDrive = (rightP + rightD);

      // set motor to drive
      motorSet(driveLeft, leftDrive * 1);
      motorSet(driveRight, rightDrive * 1);
      count = count + 1;
      delay(100);
    }
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
  int liftLastError = 0;
  int chainLastError = 0;
  int liftP;
  int chainP;
  int liftD;
  int chainD;
  int count = 0;
  while (true) {
    if (count == (waitTo / 100)) {
      motorSet(dr4bLeft, 0);
      motorSet(dr4bRight, 0); // please work, its 4 in the morning
      motorSet(chainBar, 0); // im crying because our fisrt match is in 6 hours and this doesnt work
      return; // send help (and hugs pls)
    } else {
      // calculate error
      liftError =
          (liftTarget -
           ((encoderGet(dr4bleftencoder) + encoderGet(dr4brightencoder)) / 2));
      chainError = (chainTarget - encoderGet(chainencoder));

      // calculate PD
      liftP = (liftError * 11);
      liftD = ((liftError - liftLastError) * 0);
      chainP = (chainError * (2));
      chainD = ((chainError - chainLastError) * 0);

      // store last error
      liftLastError = liftError;
      chainLastError = chainLastError;

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

      // keep drive enabled
      driveControl(joystickGetAnalog(1, 3), joystickGetAnalog(1, 2));
      fineControlToggle(
          joystickGetDigital(1, 7, JOY_DOWN), joystickGetDigital(1, 7, JOY_UP),
          joystickGetDigital(1, 8, JOY_UP), joystickGetDigital(1, 8, JOY_DOWN));
      mobileGoalControl(joystickGetDigital(1, 6, JOY_UP),
                        joystickGetDigital(1, 6, JOY_DOWN));
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
        motorSet(claw, 30);
        liftTo(4, 96, 2200);
        motorSet(claw, -90);
        liftTo(4, 96, 600);
        motorSet(claw, 5);
        liftTo(0, 350, 1000);
        motorSet(claw, 0);
        break;
      case 2:
        motorSet(claw, 30);
        liftTo(5, 90, 2200);
        motorSet(claw, -90);
        liftTo(5, 90, 600);
        motorSet(claw, 5);
        liftTo(0, 350, 1000);
        motorSet(claw, 0);
        break;
      case 3:
        motorSet(claw, 30);
        liftTo(10, 110, 2200);
        motorSet(claw, -90);
        liftTo(10, 110, 500);
        motorSet(claw, 5);
        liftTo(0, 350, 1000);
        motorSet(claw, 0);
        break;
      case 4:
        motorSet(claw, 30);
        liftTo(20, 124, 3000);
        motorSet(claw, -90);
        liftTo(20, 124, 500);
        motorSet(claw, 5);
        liftTo(0, 350, 1000);
        motorSet(claw, 0);
        break;
      case 5:
        motorSet(claw, 30);
        liftTo(25, 130, 2200);
        motorSet(claw, -90);
        liftTo(25, 130, 500);
        motorSet(claw, 5);
        liftTo(0, 350, 1000);
        motorSet(claw, 0);
        break;
      case 6:
        motorSet(claw, 30);
        liftTo(30, 135, 2400);
        motorSet(claw, -90);
        liftTo(30, 135, 500);
        motorSet(claw, 5);
        liftTo(0, 350, 1000);
        motorSet(claw, 0);
        break;
      case 7:
        motorSet(claw, 30);
        liftTo(50, 135, 2400);
        motorSet(claw, -90);
        liftTo(50, 135, 500);
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
      case 1: // stacking first cone
        motorSet(claw, 30);
        liftTo(5, 45, 1700);
        motorSet(claw, -90);
        liftTo(5, 45, 600);
        motorSet(claw, 5);
        liftTo(0, 200, 1000);
        motorSet(claw, 0);
        break;
      case 2:
        motorSet(claw, 30);
        liftTo(5, 40, 1700);
        motorSet(claw, -90);
        liftTo(5, 40, 600);
        motorSet(claw, 5);
        liftTo(0, 200, 1000);
        motorSet(claw, 0);
        break;
      case 3:
        motorSet(claw, 30);
        liftTo(20, 85, 1700);
        motorSet(claw, -90);
        liftTo(20, 85, 500);
        motorSet(claw, 5);
        liftTo(0, 200, 1000);
        motorSet(claw, 0);
        break;
      case 4:
        motorSet(claw, 30);
        liftTo(28, 100, 2000);
        motorSet(claw, -90);
        liftTo(28, 100, 500);
        motorSet(claw, 5);
        liftTo(0, 200, 1000);
        motorSet(claw, 0);
        break;
      case 5:
        motorSet(claw, 30);
        liftTo(35, 75, 2200);
        motorSet(claw, -90);
        liftTo(35, 75, 500);
        motorSet(claw, 5);
        liftTo(0, 200, 1000);
        motorSet(claw, 0);
        break;
      case 6:
        motorSet(claw, 30);
        liftTo(40, 105, 2400);
        motorSet(claw, -90);
        liftTo(40, 105, 500);
        motorSet(claw, 5);
        liftTo(0, 200, 1000);
        motorSet(claw, 0);
        break;
      case 7:
        motorSet(claw, 30);
        liftTo(50, 125, 2400);
        motorSet(claw, -90);
        liftTo(50, 125, 500);
        motorSet(claw, 5);
        liftTo(0, 200, 1000);
        motorSet(claw, 0);
        break;
      default:
        break;
      }
    }
}