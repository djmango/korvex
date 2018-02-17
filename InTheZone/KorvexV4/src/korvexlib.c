#include "main.h"
#include "constants.h"
#include "korvexlib.h"

/*-----------------------------------------------------------------------------*/
/*  drive control, user input, direct                                          */
/*-----------------------------------------------------------------------------*/
void driveControl(int chassisControlLeft, int chassisControlRight) {
  // chassis control
  if (motorGet(driveLeft) < 60 && motorGet(driveLeft) > -60) {
    motorSet(driveLeft, (chassisControlRight * -.3));
    motorSet(driveRight, (chassisControlLeft * -.3));
    motorSet(driveLeft2, (chassisControlRight * -.3));
    motorSet(driveRight2, (chassisControlLeft * -.3));
  }
  else {
    motorSet(driveLeft, (chassisControlRight * -1));
    motorSet(driveRight, (chassisControlLeft * -1));
    motorSet(driveLeft2, (chassisControlRight * -1));
    motorSet(driveRight2, (chassisControlLeft * -1));
  }
}

/*-----------------------------------------------------------------------------*/
/*  lift control, user input, direct                                           */
/*-----------------------------------------------------------------------------*/
void dr4bControl(int dr4bControl) {
  // lift control
  if (autoStackerEnabled == false) // if driver is trying to control dr4b, disable autostacker and let them
  {
    motorSet(dr4b, dr4bControl * -1);
  }
}

/*-----------------------------------------------------------------------------*/
/*  mobile goal control, user input, direct                                    */
/*-----------------------------------------------------------------------------*/
void mobileGoalControl(int moboLiftBtnUp, int moboLiftBtnDown) {
  // mobo lift control
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
  if (isDriverloadGlobal == true && autoStackerEnabled == false)
    liftTo(20, 110, 100);
  else
    motorSet(chainBar, chainControl * 1);
  // claw control
  if (clawBtnUp == 1) {
    // move up
    motorSet(claw, 60);
  }
  if (clawBtnDown == 1) {
    // move down
    motorSet(claw, -60);
  }
  if (clawBtnUp == 0 && clawBtnDown == 0) {
    // dont move
    motorSet(claw, 0);
  }
}

/*-----------------------------------------------------------------------------*/
/*  auto stack control                                                         */
/*-----------------------------------------------------------------------------*/
void autoStackControl(int incrementUpBtn, int incrementDownBtn, int incrementResetBtn, int driverloadBtn, int fieldloadBtn, int incrementUpNoFuncBtn, int resetBtn, int bufferUp, int bufferDown) {
  if (driverloadBtn == 1) {
    isDriverloadGlobal = true;
  }
  if (fieldloadBtn == 1) {
    isDriverloadGlobal = false;
  }
  if (incrementUpBtn == 1) {
    coneIncrementGlobal = coneIncrementGlobal + 1;
    autoStacker(coneIncrementGlobal, isDriverloadGlobal);
    delay(300);
  }
  if (incrementDownBtn == 1 && incrementUpBtn == 0) {
    coneIncrementGlobal = coneIncrementGlobal = coneIncrementGlobal - 1;
    delay(300);
  }
  if (incrementResetBtn == 1) {
    coneIncrementGlobal = 0;
  }
  if (incrementUpNoFuncBtn == 1) {
    coneIncrementGlobal = coneIncrementGlobal = coneIncrementGlobal + 1;
    delay(300);
  }
  if (bufferUp == 1) {
    chainBufferGlobal = -5;
  }
  if (bufferDown == 1) {
    chainBufferGlobal = 5;
  }
  if (resetBtn == 1) {
    encoderReset(dr4bencoder);
    encoderReset(chainencoder);
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
  // offset the targets, for easier readability
  leftTarget = encoderGet(leftencoder) + leftTarget;
  rightTarget = encoderGet(rightencoder) + rightTarget;
  leftError = (leftTarget - encoderGet(leftencoder));
  rightError = (rightTarget - encoderGet(rightencoder));
  while (true) {
    if (count == (waitTo / 100)) {
      motorSet(driveLeft, 0);
      motorSet(driveLeft2, 0);
      motorSet(driveRight, 0);
      motorSet(driveRight2, 0);
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
  float chainI = 0;
  int liftD;
  int chainD;
  int chainBuffer = 0;
  int count = 0;
  while (true) {
    if (count == (waitTo / 100)) {
      motorSet(dr4b, 0);
      motorSet(chainBar, 0); // im crying because our fisrt match is in 6 hours and this doesnt work
      return; // send help (and hugs pls)
    } else {
      // calculate error
      liftError = (liftTarget - encoderGet(dr4bencoder));
      chainError = ((chainTarget + chainBufferGlobal - 8) - encoderGet(chainencoder));

      // calculate pid
      liftP = (liftError * 6);
      liftD = ((liftError - liftLastError) * 5);
      chainP = (chainError * .5);
      if (abs(chainError) < 20 && chainI > 20) // if we are in range for I to be desireable
        chainI = ((chainI + chainError) * .55);
      else
        chainI = 20; // constant buffer because the motors are awful
      chainD = ((chainError - chainLastError) * 1);

      // store last error
      liftLastError = liftError;
      chainLastError = chainLastError;
      
      // a buffer because im lazy
      if (chainError != 0 && chainError == chainLastError) {
        chainBuffer = chainBuffer + 15;
      }
      else
        chainBuffer = 0;

      // calculate drive
      liftDrive = (liftP + liftD);
      chainDrive = (chainP + chainI + chainD + chainBuffer);

      // set motor to drive
      motorSet(dr4b, liftDrive  * -1);
      motorSet(chainBar, chainDrive * -1);
      if (debugGlobal == true) {
        printf("lift error %d\n", liftError);
        printf("lift drive %d\n", liftDrive);
        printf("chain error %d\n", chainError);
        printf("chain drive %d\n", chainDrive);
      }
      // keep drive enabled if in driver
      if (isAutonomous() == false) {
        driveControl(joystickGetAnalog(1, 2), joystickGetAnalog(1, 3));
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
  int lcdHoldMin = -3;
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
    case -4: // defence right
      lcdSetText(uart1, 1, "  defence right ");
      lcdSetText(uart1, 2, "<- | select | ->");
    case -3: // defence left
      lcdSetText(uart1, 1, "  defence left  ");
      lcdSetText(uart1, 2, "<- | select | ->");
      break;
    case -2: // stationary left
      lcdSetText(uart1, 1, " stationary left");
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
    case 1: // stationary right
      lcdSetText(uart1, 1, "stationary right");
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
    case -4: // defence right
      lcdSetText(uart1, 1, "  defence right  ");
      lcdSetText(uart1, 2, "u got rekt lol xd");
      auton = 9;
      break;
    case -3: // defence left
      lcdSetText(uart1, 1, "  defence left  ");
      lcdSetText(uart1, 2, "u got rekt lol xd");
      auton = 8;
      break;
    case -2: // stationary left
      lcdSetText(uart1, 1, "| station left |");
      lcdSetText(uart1, 2, "haha we fooled u");
      auton = 6;
      break;
    case -1: // red left
      lcdSetText(uart1, 1, "|  red   left  |");
      lcdSetText(uart1, 2, "korvex  robotics");
      auton = 2;
      break;
    case 0: // disabled
      lcdSetText(uart1, 1, "|   disabled   |");
      lcdSetText(uart1, 2, "i not very smart");
      auton = -1;
      break;
    case 1: // stationary right
      lcdSetText(uart1, 1, "|station right |");
      lcdSetText(uart1, 2, "haha we fooled u");
      auton = 5;
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
        motorSet(claw, -30);
        liftTo(0, 50, 1200);
        motorSet(claw, 40);
        liftTo(0, 50, 500);
        motorSet(claw, -10);
        liftTo(0, 210, 800);
        motorSet(claw, 0);
        autoStackerEnabled = false;
        break;
      case 2:
        motorSet(claw, -30);
        liftTo(9, 45, 1300);
        motorSet(claw, 40);
        liftTo(9, 45, 500);
        motorSet(claw, -10);
        liftTo(0, 210, 800);
        motorSet(claw, 0);
        autoStackerEnabled = false;
        break;
      case 3:
        motorSet(claw, -30);
        liftTo(26, 140, 900);
        liftTo(26, 45, 600);
        motorSet(claw, 40);
        liftTo(28, 45, 500);
        motorSet(claw, -10);
        liftTo(0, 210, 800);
        motorSet(claw, 0);
        autoStackerEnabled = false;
        break;
      case 4:
        motorSet(claw, -30);
        liftTo(48, 140, 800);
        liftTo(48, 50, 1000);
        motorSet(claw, 40);
        liftTo(48, 50, 600);
        motorSet(claw, -10);
        liftTo(0, 210, 800);
        motorSet(claw, 0);
        autoStackerEnabled = false;
        break;
      case 5:
        motorSet(claw, -30);
        liftTo(48, 140, 800);
        liftTo(48, 50, 1000);
        motorSet(claw, 40);
        liftTo(48, 50, 600);
        motorSet(claw, -10);
        liftTo(0, 210, 800);
        motorSet(claw, 0);
        autoStackerEnabled = false;
        break;
      case 6:
        motorSet(claw, -30);
        liftTo(60, 140, 1000);
        liftTo(60, 58, 1000);
        motorSet(claw, 40);
        liftTo(62, 58, 600);
        motorSet(claw, -10);
        liftTo(0, 210, 800);
        motorSet(claw, 0);
        autoStackerEnabled = false;
        break;
      case 7:
        motorSet(claw, -30);
        liftTo(65, 140, 1200);
        liftTo(75, 53, 1500);
        motorSet(claw, 40);
        liftTo(78, 53, 600);
        motorSet(claw, -10);
        liftTo(0, 210, 800);
        motorSet(claw, 0);
        autoStackerEnabled = false;
        break;
      case 8:
        motorSet(claw, -30);
        liftTo(75, 140, 1200);
        liftTo(85, 55, 1500);
        motorSet(claw, 40);
        liftTo(88, 55, 600);
        motorSet(claw, -10);
        liftTo(0, 210, 800);
        motorSet(claw, 0);
        autoStackerEnabled = false;
        break;
      case 9:
        motorSet(claw, -30);
        liftTo(85, 140, 1500);
        liftTo(95, 50, 1500);
        motorSet(claw, 40);
        liftTo(98, 50, 600);
        motorSet(claw, -10);
        liftTo(100, 140, 1000);
        liftTo(0, 210, 1200);
        motorSet(claw, 0);
        autoStackerEnabled = false;
        break;
      case 10:
        motorSet(claw, -30);
        liftTo(110, 140, 1500);
        liftTo(110, 55, 1500);
        motorSet(claw, 40);
        liftTo(113, 55, 600);
        motorSet(claw, -10);
        liftTo(0, 210, 800);
        motorSet(claw, 0);
        autoStackerEnabled = false;
        break;
        break;
      default:
        autoStackerEnabled = false;
        break;
      }
    }
    if (isDriverload == true) { // if we are stacking driver loads, use the corresponding set of presets
      switch (coneIncrement) {
      case 1: // stacking first cone
        motorSet(claw, -30);
        liftTo(0, 46, 700);
        motorSet(claw, 40);
        liftTo(0, 46, 500);
        motorSet(claw, -10);
        liftTo(20, 110, 400);
        motorSet(claw, 0);
        autoStackControl(joystickGetDigital(2, 7, JOY_UP), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_RIGHT), joystickGetDigital(2, 8, JOY_RIGHT), joystickGetDigital(2, 8, JOY_LEFT), joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN), joystickGetDigital(2, 6, JOY_UP), joystickGetDigital(2, 6, JOY_DOWN));
        if (isDriverloadGlobal == false) {
          autoStackerEnabled = false; 
          break;
        }
        else {
          coneIncrementGlobal = coneIncrementGlobal + 1;
          autoStacker(coneIncrementGlobal, true);
        }
        break;
      case 2:
        motorSet(claw, -30);
        liftTo(9, 47, 800);
        motorSet(claw, 40);
        liftTo(9, 47, 500);
        motorSet(claw, -10);
        liftTo(20, 100, 1000);
        motorSet(claw, 0);
        autoStackControl(joystickGetDigital(2, 7, JOY_UP), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_RIGHT), joystickGetDigital(2, 8, JOY_RIGHT), joystickGetDigital(2, 8, JOY_LEFT), joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN), joystickGetDigital(2, 6, JOY_UP), joystickGetDigital(2, 6, JOY_DOWN));
        if (isDriverloadGlobal == false)
        {
          autoStackerEnabled = false;
          break;
        }
        else
        {
          coneIncrementGlobal = coneIncrementGlobal + 1;
          autoStacker(coneIncrementGlobal, true);
        }
        break;
      case 3:
        motorSet(claw, -30);
        liftTo(23, 50, 1000);
        motorSet(claw, 40);
        liftTo(25, 50, 500);
        motorSet(claw, -10);
        liftTo(20, 100, 400);
        motorSet(claw, 0);
        autoStackControl(joystickGetDigital(2, 7, JOY_UP), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_RIGHT), joystickGetDigital(2, 8, JOY_RIGHT), joystickGetDigital(2, 8, JOY_LEFT), joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN), joystickGetDigital(2, 6, JOY_UP), joystickGetDigital(2, 6, JOY_DOWN));
        if (isDriverloadGlobal == false)
        {
          autoStackerEnabled = false;
          break;
        }
        else
        {
          coneIncrementGlobal = coneIncrementGlobal + 1;
          autoStacker(coneIncrementGlobal, true);
        }
        break;
      case 4:
        motorSet(claw, -30);
        liftTo(35, 90, 800);
        liftTo(35, 51, 800);
        motorSet(claw, 40);
        liftTo(35, 51, 600);
        motorSet(claw, -10);
        liftTo(20, 100, 400);
        motorSet(claw, 0);
        autoStackControl(joystickGetDigital(2, 7, JOY_UP), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_RIGHT), joystickGetDigital(2, 8, JOY_RIGHT), joystickGetDigital(2, 8, JOY_LEFT), joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN), joystickGetDigital(2, 6, JOY_UP), joystickGetDigital(2, 6, JOY_DOWN));
        if (isDriverloadGlobal == false)
        {
          autoStackerEnabled = false;
          break;
        }
        else
        {
          coneIncrementGlobal = coneIncrementGlobal + 1;
          autoStacker(coneIncrementGlobal, true);
        }
      case 5:
        motorSet(claw, -30);
        liftTo(46, 90, 800);
        liftTo(46, 55, 1000);
        motorSet(claw, 40);
        liftTo(50, 55, 600);
        motorSet(claw, -10);
        liftTo(20, 100, 400);
        motorSet(claw, 0);
        autoStackControl(joystickGetDigital(2, 7, JOY_UP), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_RIGHT), joystickGetDigital(2, 8, JOY_RIGHT), joystickGetDigital(2, 8, JOY_LEFT), joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN), joystickGetDigital(2, 6, JOY_UP), joystickGetDigital(2, 6, JOY_DOWN));
        if (isDriverloadGlobal == false)
        {
          autoStackerEnabled = false;
          break;
        }
        else
        {
          coneIncrementGlobal = coneIncrementGlobal + 1;
          autoStacker(coneIncrementGlobal, true);
        }
        break;
      case 6:
        motorSet(claw, -30);
        liftTo(60, 90, 1000);
        liftTo(60, 55, 1000);
        motorSet(claw, 40);
        liftTo(62, 55, 600);
        motorSet(claw, -10);
        liftTo(20, 100, 600);
        motorSet(claw, 0);
        autoStackControl(joystickGetDigital(2, 7, JOY_UP), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_RIGHT), joystickGetDigital(2, 8, JOY_RIGHT), joystickGetDigital(2, 8, JOY_LEFT), joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN), joystickGetDigital(2, 6, JOY_UP), joystickGetDigital(2, 6, JOY_DOWN));
        if (isDriverloadGlobal == false)
        {
          autoStackerEnabled = false;
          break;
        }
        else
        {
          coneIncrementGlobal = coneIncrementGlobal + 1;
          autoStacker(coneIncrementGlobal, true);
        }
        break;
      case 7:
        motorSet(claw, -30);
        liftTo(65, 90, 1200);
        liftTo(75, 50, 1500);
        motorSet(claw, 40);
        liftTo(78, 50, 600);
        motorSet(claw, -10);
        liftTo(20, 100, 400);
        motorSet(claw, 0);
        autoStackControl(joystickGetDigital(2, 7, JOY_UP), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_RIGHT), joystickGetDigital(2, 8, JOY_RIGHT), joystickGetDigital(2, 8, JOY_LEFT), joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN), joystickGetDigital(2, 6, JOY_UP), joystickGetDigital(2, 6, JOY_DOWN));
        if (isDriverloadGlobal == false)
        {
          autoStackerEnabled = false;
          break;
        }
        else
        {
          coneIncrementGlobal = coneIncrementGlobal + 1;
          autoStacker(coneIncrementGlobal, true);
        }
        break;
      case 8:
        motorSet(claw, -30);
        liftTo(75, 90, 1200);
        liftTo(85, 50, 1500);
        motorSet(claw, 40);
        liftTo(88, 50, 600);
        motorSet(claw, -10);
        liftTo(20, 100, 400);
        motorSet(claw, 0);
        autoStackControl(joystickGetDigital(2, 7, JOY_UP), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_RIGHT), joystickGetDigital(2, 8, JOY_RIGHT), joystickGetDigital(2, 8, JOY_LEFT), joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN), joystickGetDigital(2, 6, JOY_UP), joystickGetDigital(2, 6, JOY_DOWN));
        if (isDriverloadGlobal == false)
        {
          autoStackerEnabled = false;
          break;
        }
        else
        {
          coneIncrementGlobal = coneIncrementGlobal + 1;
          autoStacker(coneIncrementGlobal, true);
        }
        break;
      case 9:
        motorSet(claw, -30);
        liftTo(85, 90, 1500);
        liftTo(95, 50, 1500);
        motorSet(claw, 40);
        liftTo(98, 50, 600);
        motorSet(claw, -10);
        liftTo(20, 100, 400);
        motorSet(claw, 0);
        autoStackControl(joystickGetDigital(2, 7, JOY_UP), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_RIGHT), joystickGetDigital(2, 8, JOY_RIGHT), joystickGetDigital(2, 8, JOY_LEFT), joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN), joystickGetDigital(2, 6, JOY_UP), joystickGetDigital(2, 6, JOY_DOWN));
        if (isDriverloadGlobal == false)
        {
          autoStackerEnabled = false;
          break;
        }
        else
        {
          coneIncrementGlobal = coneIncrementGlobal + 1;
          autoStacker(coneIncrementGlobal, true);
        }
        break;
      case 10:
        motorSet(claw, -30);
        liftTo(110, 90, 1500);
        liftTo(110, 50, 1500);
        motorSet(claw, 40);
        liftTo(113, 50, 600);
        motorSet(claw, -10);
        liftTo(20, 100, 400);
        motorSet(claw, 0);
        autoStackControl(joystickGetDigital(2, 7, JOY_UP), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_RIGHT), joystickGetDigital(2, 8, JOY_RIGHT), joystickGetDigital(2, 8, JOY_LEFT), joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN), joystickGetDigital(2, 6, JOY_UP), joystickGetDigital(2, 6, JOY_DOWN));
        if (isDriverloadGlobal == false)
        {
          autoStackerEnabled = false;
          break;
        }
        else
        {
          coneIncrementGlobal = coneIncrementGlobal + 1;
          autoStacker(coneIncrementGlobal, true);
        }
        break;
      case 11:
        motorSet(claw, -30);
        liftTo(115, 90, 1500);
        liftTo(120, 50, 1500);
        motorSet(claw, 40);
        liftTo(125, 50, 600);
        motorSet(claw, -10);
        liftTo(20, 100, 400);
        motorSet(claw, 0);
        autoStackControl(joystickGetDigital(2, 7, JOY_UP), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_RIGHT), joystickGetDigital(2, 8, JOY_RIGHT), joystickGetDigital(2, 8, JOY_LEFT), joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN), joystickGetDigital(2, 6, JOY_UP), joystickGetDigital(2, 6, JOY_DOWN));
        if (isDriverloadGlobal == false)
        {
          autoStackerEnabled = false;
          break;
        }
        else
        {
          coneIncrementGlobal = coneIncrementGlobal + 1;
          autoStacker(coneIncrementGlobal, true);
        }
        break;
      default:
        autoStackerEnabled = false;
        break;
      }
    }
}
