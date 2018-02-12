#include "main.h"
#include "constants.h"
#include "korvexlib.h"

// functions

/*-----------------------------------------------------------------------------*/
/*  drive control, user input, direct                                          */
/*-----------------------------------------------------------------------------*/
void driveControl(int chassisControlLeft, int chassisControlRight) {
  // chassis control
  motorSet(driveLeft, (chassisControlRight * -1));
  motorSet(driveRight, (chassisControlLeft * -1));
  motorSet(driveLeft2, (chassisControlRight * -1));
  motorSet(driveRight2, (chassisControlLeft * -1));
}

/*-----------------------------------------------------------------------------*/
/*  lift control, user input, direct                                           */
/*-----------------------------------------------------------------------------*/
void dr4bControl(int dr4bControl) {
  // lift control
  if (autoStackerEnabled == false) // if driver is trying to control dr4b, disable autostacker and let them
  {
    motorSet(dr4b, dr4bControl);
  }
}

/*-----------------------------------------------------------------------------*/
/*  mobile goal control, user input, direct                                    */
/*-----------------------------------------------------------------------------*/
void mobileGoalControl(int moboLiftBtnUp, int moboLiftBtnDown) {
  // mobo lift control
  // if (moboLiftBtnUp == true) { // if user wants mobile goal to be up, set values accordingly
  //   int target = 10;
  //   int error = 1;
  //   while (error != 0) {
  //     error = (target - encoderGet(mobilegoalencoder)); // get error
  //     int drive = error * 5;       // calc drive, which is motor power
  //     motorSet(mobileGoal, drive); // set motor to desired power
  //     if (isAutonomous == false) {
  //       driveControl(joystickGetAnalog(1, 3), joystickGetAnalog(1, 2));
  //     }
  //   }
  //   motorSet(mobileGoal, 0);
  // }
  // if (moboLiftBtnDown == 1) {
  //   int target = -10;
  //   int error = 1;
  //   while (error != 0) {
  //     error = (target - encoderGet(mobilegoalencoder)); // get error
  //     int drive = error * 5;       // calc drive, which is motor power
  //     motorSet(mobileGoal, drive); // set motor to desired power
  //     if (isAutonomous == false) {
  //       driveControl(joystickGetAnalog(1, 3), joystickGetAnalog(1, 2));
  //     }
  //   }
  //   motorSet(mobileGoal, 0);
  // }
  if (moboLiftBtnUp == 1) {
    motorSet(mobileGoal, 127);
  }
  if (moboLiftBtnDown == 1) {
    motorSet(mobileGoal, -127);
  }
  if (moboLiftBtnUp == 0 && moboLiftBtnDown == 0) {
    motorSet(mobileGoal, 0);
  }
}

/*-----------------------------------------------------------------------------*/
/*  cone handler control                                                       */
/*-----------------------------------------------------------------------------*/
void coneHandlerControl(int clawBtnUp, int clawBtnDown, int chainControl) {
  if (autoStackerEnabled == false) // if autostacker is operating, dont take input
  {
    motorSet(chainBar, chainControl * -1);
  }
  // claw control
  if (clawBtnUp == 1) {
    // move up
    motorSet(claw, 40);
  }
  if (clawBtnDown == 1) {
    // move down
    motorSet(claw, -40);
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
/*  argument based encoder pd, for drive */
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
      // calculate pd
      leftP = (leftError * .8);
      leftD = ((leftError - leftLastError) * .4);
      rightP = (rightError * .8);
      rightD = ((rightError - rightLastError) * .4);

      // store last error
      leftLastError = leftError;
      rightLastError = rightError;

      // calculate drive
      leftDrive = (leftP + leftD);
      rightDrive = (rightP + rightD);

      // set motor to drive
      motorSet(driveLeft, leftDrive * -1);
      motorSet(driveRight, rightDrive * -1);
      motorSet(driveLeft2, leftDrive * -1);
      motorSet(driveRight2, rightDrive * -1);
      count = count + 1;
      delay(100);
    }
  }
}

/*-----------------------------------------------------------------------------*/
/*  argument based encoder pid, for drive, tuned for skills mode */
/*-----------------------------------------------------------------------------*/
void driveToSkills(int leftTarget, int rightTarget, int waitTo) {
  int leftError;
  int rightError;
  int leftDrive;
  int rightDrive;
  int leftLastError = 0;
  int rightLastError = 0;
  int leftP;
  int rightP;
  float leftI = 0;
  float rightI = 0;
  int leftD;
  int rightD;
  int count = 0;
  // offset the targets, for easier readability
  leftTarget = encoderGet(leftencoder) + leftTarget;
  rightTarget = encoderGet(rightencoder) + rightTarget;
  leftError = (leftTarget - encoderGet(leftencoder));
  rightError = (rightTarget - encoderGet(rightencoder));
  while (true) {
    if (count == (waitTo / 100) || (leftError == 0 && leftLastError == 0 && rightError == 0 && rightLastError == 0)) {
      motorSet(driveLeft, 0);
      motorSet(driveLeft2, 0);
      motorSet(driveRight, 0);
      motorSet(driveRight2, 0);
      return;
    } else {
      // calculate error
      leftError = (leftTarget - encoderGet(leftencoder));
      rightError = (rightTarget - encoderGet(rightencoder));
      // calculate pid
      leftP = (leftError * 1);
      if (abs(leftError) < 15)
        leftI = ((leftI + leftError) * .8);
      else
        leftI = 0;
      leftD = ((leftError - leftLastError) * 1);
      rightP = (rightError * 1);
      if (abs(rightError) < 15)
        rightI = ((rightI + rightError) * .8);
      else
        rightI = 0;
      rightD = ((rightError - rightLastError) * 1);

      // store last error
      leftLastError = leftError;
      rightLastError = rightError;

      // calculate drive
      leftDrive = (leftP + leftI + leftD);
      rightDrive = (rightP + rightI + rightD);

      // if we are in debug mode, print error
      if (debugGlobal == true) {
        printf("lErr%d\n", leftError);
        printf("lDri%d\n", leftDrive);
        printf("rErr%d\n", rightError);
        printf("rDri%d\n", rightDrive);
      }

      // set motor to drive
      motorSet(driveLeft, leftDrive * -1);
      motorSet(driveRight, rightDrive * -1);
      motorSet(driveLeft2, leftDrive * -1);
      motorSet(driveRight2, rightDrive * -1);
      count = count + 1;
      delay(100);
    }
  }
}

/*-----------------------------------------------------------------------------*/
/*  an argument based pd for the lift and chainbar. Konsts are hardcoded       */
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
      motorSet(dr4b, 0);
      motorSet(chainBar, 0); // im crying because our fisrt match is in 6 hours and this doesnt work
      return; // send help (and hugs pls)
    } else {
      // calculate error
      liftError = (liftTarget - encoderGet(dr4bencoder));
      chainError = (chainTarget - encoderGet(chainencoder));

      // calculate PD
      liftP = (liftError * 5);
      liftD = ((liftError - liftLastError) * 3);
      chainP = (chainError * 2);
      chainD = ((chainError - chainLastError) * 2);

      // store last error
      liftLastError = liftError;
      chainLastError = chainLastError;

      // calculate drive
      liftDrive = (liftP + liftD);
      chainDrive = (chainP + chainD);

      // set motor to drive
      motorSet(dr4b, liftDrive);
      motorSet(chainBar, chainDrive * -1);
      if (debugGlobal == true) {
        printf("lift error %d\n", liftError);
        printf("lift drive %d\n", liftDrive);
        printf("chain error %d\n", chainError);
        printf("chain drive %d\n", chainDrive);
      }
      // keep drive enabled if in driver
      if (isAutonomous() == false) {
        driveControl(joystickGetAnalog(1, 3), joystickGetAnalog(1, 2));
        mobileGoalControl(joystickGetDigital(1, 6, JOY_UP),
                          joystickGetDigital(1, 6, JOY_DOWN));
      }
      count = count + 1;
      delay(100);
    }
  }
}

/*-----------------------------------------------------------------------------*/
/*  lcd autonomous selector, for convinience and stuff                         */
/*-----------------------------------------------------------------------------*/
void lcdAutSel(int input) {
  // min and max holds
  int lcdHoldMin = -2;
  int lcdHoldMax = 3;
  
  // update holder accordingly
  if (input == 1) { // if the left button is pushed
    lcdHoldGlobal = lcdHoldGlobal - 1;
  }
  if (input == 4) { // if the right button is pushed
    lcdHoldGlobal = lcdHoldGlobal + 1;
  }

  // limit lcd hold
  if (lcdHoldGlobal > lcdHoldMax) {
    lcdHoldGlobal = lcdHoldMax;
  }
  if (lcdHoldGlobal < lcdHoldMin) {
    lcdHoldGlobal = lcdHoldMin;
  }
  
  // display according to holder
  switch (lcdHoldGlobal) {
    case -2: // red right
      lcdSetText(uart1, 1, "   red  right   ");
      lcdSetText(uart1, 2, "<- | select | ->");
      break;
    case -1: // red left
      lcdSetText(uart1, 1, "   red   left   ");
      lcdSetText(uart1, 2, "<- | select | ->");
      break;
    case 0: // disabled
      lcdSetText(uart1, 1, "    disabled    ");
      lcdSetText(uart1, 2, "<- | select | ->");
      break;
    case 1: // blue left
      lcdSetText(uart1, 1, "   blue  left   ");
      lcdSetText(uart1, 2, "<- | select | ->");
      break;
    case 2: // blue right
      lcdSetText(uart1, 1, "   blue right   ");
      lcdSetText(uart1, 2, "<- | select | ->");
      break;
    case 3: // skills
      lcdSetText(uart1, 1, "     skills     ");
      lcdSetText(uart1, 2, "<- | select | ->");
      break;
  }

  if (input == 2) { // ooh you got chosen, now do your thing
    switch (lcdHoldGlobal) {
    case -2: // red right
      lcdSetText(uart1, 1, "|  red  right  |");
      lcdSetText(uart1, 2, "korvex  robotics");
      auton = 2;
      break;
    case -1: // red left
      lcdSetText(uart1, 1, "|  red   left  |");
      lcdSetText(uart1, 2, "korvex  robotics");
      auton = 3;
      break;
    case 0: // disabled
      lcdSetText(uart1, 1, "|   disabled   |");
      lcdSetText(uart1, 2, "korvex  robotics");
      auton = -1;
      break;
    case 1: // blue left
      lcdSetText(uart1, 1, "|  blue  left  |");
      lcdSetText(uart1, 2, "korvex  robotics");
      auton = 0;
      break;
    case 2: // blue right
      lcdSetText(uart1, 1, "|  blue right  |");
      lcdSetText(uart1, 2, "korvex  robotics");
      auton = 1;
      break;
    case 3: // skills
      lcdSetText(uart1, 1, "     skills     ");
      lcdSetText(uart1, 2, "korvex  robotics");
      auton = 7;
      break;
    }
    return;
  }

  // return if im sad
  if (input == 0) { // if we dont get any input, why do anything
    if (isAutonomous() == false)
    {
      return;
    }
    else
    {
      lcdAutSel(lcdReadButtons(uart1)); // haha self calling functions, what could go wrong
    }
  }
}

/*-----------------------------------------------------------------------------*/
/*  recording function for rerun                                               */
/*-----------------------------------------------------------------------------*/
void rerunRecord() {
  printf("driveTo(%d, %d, 100);\n", encoderGet(leftencoder), encoderGet(rightencoder));
  // wow this was difficult
}

/*-----------------------------------------------------------------------------*/
/*  an argument based autostacker, which sets the targets of each system,      */
/*  in accordance to presets corresponding to the cones stacked                */
/*-----------------------------------------------------------------------------*/
void autoStacker(int coneIncrement, bool isDriverload) { // cone increment will decide what function will run, each is specific to the height
    autoStackerEnabled = true;
    if (isDriverload == false) { // if we are not stacking driver load, assume we are stacking from ground
      switch (coneIncrement) {
      case 1: // stacking first cone
        motorSet(claw, 30);
        liftTo(0, 130, 1600);
        motorSet(claw, -40);
        liftTo(0, 130, 400);
        motorSet(claw, 15);
        liftTo(0, 10, 1000);
        motorSet(claw, 0);
        autoStackerEnabled = false;
        break;
      case 2:
        motorSet(claw, 30);
        liftTo(0, 130, 1600);
        motorSet(claw, -40);
        liftTo(0, 130, 400);
        motorSet(claw, 15);
        liftTo(0, 10, 1000);
        motorSet(claw, 0);
        autoStackerEnabled = false;
        break;
      case 3:
        motorSet(claw, 30);
        liftTo(5, 120, 1600);
        motorSet(claw, -40);
        liftTo(5, 120, 400);
        motorSet(claw, 15);
        liftTo(0, 10, 1000);
        motorSet(claw, 0);
        autoStackerEnabled = false;
        break;
      case 4:
        motorSet(claw, 30);
        liftTo(25, 115, 1600);
        motorSet(claw, -40);
        liftTo(25, 115, 400);
        motorSet(claw, 15);
        liftTo(25, 40, 800);
        motorSet(claw, 0);
        liftTo(0, 10, 500);
        autoStackerEnabled = false;
        break;
      case 5:
        motorSet(claw, 30);
        liftTo(40, 115, 1600);
        motorSet(claw, -40);
        liftTo(40, 115, 400);
        motorSet(claw, 15);
        liftTo(40, 40, 1000);
        motorSet(claw, 0);
        liftTo(0, 10, 500);
        autoStackerEnabled = false;
      case 6:
        motorSet(claw, 30);
        liftTo(50, 110, 1600);
        motorSet(claw, -40);
        liftTo(45, 110, 400);
        motorSet(claw, 15);
        liftTo(55, 40, 1000);
        motorSet(claw, 0);
        liftTo(0, 10, 500);
        autoStackerEnabled = false;
      default:
        autoStackerEnabled = false;
        break;
      }
    }
    if (isDriverload == true) { // if we are stacking driver loads, use the corresponding set of presets
      switch (coneIncrement) {
      default:
        autoStackerEnabled = false;
        break;
      }
    }
}