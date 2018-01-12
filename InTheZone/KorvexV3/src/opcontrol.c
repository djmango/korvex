#include "main.h"
#include "constants.h"
#include "korvexlib.h"

// defs

//control modifiers
bool isReverse = false;
bool isFineControl = false;
float fineControl = 1;

// functions

/*-----------------------------------------------------------------------------*/
/*  Drive and lift control */
/*-----------------------------------------------------------------------------*/
void driveControl(int chassisControlLeft, int chassisControlRight) {
  //chassis control
  if (chassisControlLeft > 5 || chassisControlRight < -5 || chassisControlRight > 5 || chassisControlLeft < -5){
    if (isReverse == true) { // if in reverse, invert and switch sides for normal turning
      motorSet(driveLeft, (chassisControlRight * -1));
      motorSet(driveRight, (chassisControlLeft * -1));
    } else if (isReverse == false) { // if in normal operator, do not invert
      motorSet(driveRight, (chassisControlRight));
      motorSet(driveLeft, (chassisControlLeft));
    }
  }
}

void dr4bControl(int dr4bControl) {
  // lift control
  if (dr4bControl > 5 || dr4bControl < -5) // if driver is trying to control dr4b, disable autostacker and let them
  {
    autoStackerEnabled = false;;
    motorSet(dr4bLeft, (dr4bControl * -1));
    motorSet(dr4bRight, dr4bControl);
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
    motorSet(chainBar, chainControl);
  } else {
    autoStackerEnabled = true;
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

void autoStackControl(int incrementUpBtn, int incrementDownBtn, int driverloadBtn, int fieldloadBtn) {
  if (driverloadBtn == 1) {
    isDriverloadGlobal = true;
  }
  if (fieldloadBtn == 1) {
    isDriverloadGlobal = false;
  }
  if (incrementUpBtn == 1) { 
    coneIncrementGlobal = coneIncrementGlobal + 1;
    autoStacker(coneIncrementGlobal, isDriverloadGlobal);
  }
  if (incrementDownBtn == 1 && incrementUpBtn == 0) {
    coneIncrementGlobal = coneIncrementGlobal = coneIncrementGlobal - 1;
    delay(100);
  }
}

/*-----------------------------------------------------------------------------*/
/*  Funky messages for the lcd */
/*-----------------------------------------------------------------------------*/
void lcdText() {
  lcdSetBacklight(uart1, true);
  lcdSetText(uart1, 1, "i want to");
  lcdSetText(uart1, 2, "sudo rm -rf myself");
}

/*motor port names:
   driveLeft
   driveRight
   mobileGoal
   dr4bLeft
   dr4bRight
   chainBar
   claw
 */

/*control arguement names
   chassisControl chassisControlLeft, chassisControlRight
   dr4bControl dr4bControl
   fineControlToggle fineBtn, reverseBtn
   mobileGoalControl moboLiftBtnUp, moboLiftBtnDown
   coneHandlerControl clawBtnUp, clawBtnDown, chainbarBtnUp, chainbarBtnDown
 */

void operatorControl() {
  lcdText();
  coneIncrementGlobal = 0;
  isDriverloadGlobal = false;
  // liftTo(0, 150, 100000);
  while (isEnabled()) {
    //argument based control scheme
    printf("%d\n", coneIncrementGlobal);
    driveControl(joystickGetAnalog(1, 3), joystickGetAnalog(1, 2));
    dr4bControl(joystickGetAnalog(2, 2));
    fineControlToggle(joystickGetDigital(1, 7, JOY_DOWN), joystickGetDigital(1, 7, JOY_UP), joystickGetDigital(1, 8, JOY_UP), joystickGetDigital(1, 8, JOY_DOWN));
    mobileGoalControl( joystickGetDigital(1, 6, JOY_UP), joystickGetDigital(1, 6, JOY_DOWN));
    coneHandlerControl(joystickGetDigital(2, 5, JOY_UP), joystickGetDigital(2, 5, JOY_DOWN), joystickGetAnalog(2, 3));
    autoStackControl(joystickGetDigital(2, 7, JOY_UP), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_RIGHT), joystickGetDigital(2, 7, JOY_LEFT));
    delay(20);
  }
}