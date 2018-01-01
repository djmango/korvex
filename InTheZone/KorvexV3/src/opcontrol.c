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
void updateDrive(int chassisControlLeft, int chassisControlRight, int liftControl) {
  //chassis control
  if (isReverse == true) { //if in reverse, invert and switch sides for normal turning
    motorSet(driveLeft, (chassisControlRight * -1));
    motorSet(driveRight, (chassisControlLeft * -1));
  }
  else if (isReverse == false) { //if in normal operator, do not invert
    motorSet(driveRight, (chassisControlRight));
    motorSet(driveLeft, (chassisControlLeft));
  }
  //lift control
  motorSet(dr4bLeft, (liftControl));
  motorSet(dr4bRight, (liftControl));
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
void mobileGoalControl(int moboLiftBtnUp, int moboLiftBtnDown,
                       int moboTiltBtnUp, int moboTiltBtnDown) {
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
  // mobo tilt control
  if (moboTiltBtnUp == 1) {
    motorSet(5, 127);
  }
  if (moboTiltBtnDown == 1) {
    motorSet(5, -127);
  }
  if (moboTiltBtnUp == 0 && moboTiltBtnDown == 0) {
    motorSet(5, 0);
  }
}

/*-----------------------------------------------------------------------------*/
/*  Cone handler control */
/*-----------------------------------------------------------------------------*/
void coneHandlerControl(int clawBtnUp, int clawBtnDown, int chainbarBtnUp,
                        int chainbarBtnDown) {
  motorSet(chainBar2, (joystickGetAnalog(2, 3)));
  /* chain bar control
     if (chainbarBtnUp == 1) {
     // move up
     motorSet(8, 127);
     }
     if (chainbarBtnDown == 1) {
     // move down
     motorSet(8, -127);
     }
     if (chainbarBtnUp == 0 &&
      chainbarBtnDown == 0) {
     // dont move
     motorSet(8, 0);
     }*/
  // claw control
  if (clawBtnUp == 1) {
    // move up
    motorSet(9, 40);
  }
  if (clawBtnDown == 1) {
    // move down
    motorSet(9, -40);
  }
  if (clawBtnUp == 0 && clawBtnDown == 0) {
    // dont move
    motorSet(9, 0);
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

/*port map DESIRED
   port 1 = **scrubbed**
   port 2 = drive left side (y-cable to power expander)
   port 3 = drive right side (y-cable to power expander)
   port 4 = mobile goal lift (y-cable)
   port 5 = free
   port 6 = dr4b left (y-cable)
   port 7 = dr4b right (y-cable)
   port 8 = chain bar (direct)
   port 9 = claw (direct)
   port 10 = **scrubbed**
 */

/*control map DESIRED
   drive left side = mainLeftJoy(1,3)
   drive right side = mainRightJoy(1,2)
   mobile goal lift = seccondaryRightBumper(2,6)
   dr4b = seccondaryRightJoy(2,3)
   chain bar = seccondaryLeftJoy(2,2)
   claw = seccondaryLeftBumper(2,5)
 */

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
   chassisControl chassisControlLeft, chassisControlRight, liftControl
   fineControlToggle fineBtn, reverseBtn
   mobileGoalControl moboLiftBtn, moboTiltBtn
   coneHandlerControl clawBtn, chainbarBtn
 */
void operatorControl() {
  lcdText();
  while (isEnabled()) {
    //argument based control scheme
    updateDrive(joystickGetAnalog(1, 3), joystickGetAnalog(1, 2), joystickGetAnalog(2, 2));
    fineControlToggle(joystickGetDigital(1, 7, JOY_DOWN), joystickGetDigital(1, 7, JOY_UP), joystickGetDigital(1, 8, JOY_UP), joystickGetDigital(1, 8, JOY_DOWN));
    mobileGoalControl( joystickGetDigital(1, 6, JOY_UP), joystickGetDigital(1, 6, JOY_DOWN), joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN));
    coneHandlerControl(joystickGetDigital(2, 5, JOY_UP), joystickGetDigital(2, 5, JOY_DOWN), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_UP));
    delay(20);
  }
}