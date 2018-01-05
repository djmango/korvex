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
  if (isReverse == true) { //if in reverse, invert and switch sides for normal turning
    motorSet(driveLeft, (chassisControlRight * -1));
    motorSet(driveRight, (chassisControlLeft * -1));
  }
  else if (isReverse == false) { //if in normal operator, do not invert
    motorSet(driveRight, (chassisControlRight));
    motorSet(driveLeft, (chassisControlLeft));
  }
}

void dr4bControl(int dr4bControl) {
  // lift control
  if (dr4bControl > 15 || dr4bControl < -15) // if driver is trying to control dr4b, disable autostacker and let them
  {
    autoStackerEnabled = false;;
    dr4bLeftTarget = dr4bControl;
    dr4bRightTarget = dr4bControl;
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
  if (dr4bControl > 15 || dr4bControl < -15) // if driver is trying to control chain, disable autostacker and let them
  {
    autoStackerEnabled = false;
    ;
    dr4bLeftTarget = dr4bControl;
    dr4bRightTarget = dr4bControl;
  } else {
    autoStackerEnabled = true;
  }
  motorSet(chainBar, (joystickGetAnalog(2, 3) / 2 ));
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

/*-----------------------------------------------------------------------------*/
/*  Funky messages for the lcd */
/*-----------------------------------------------------------------------------*/
void lcdText() {
  lcdSetBacklight(uart1, true);
  lcdSetText(uart1, 1, "i want to");
  lcdSetText(uart1, 2, "sudo rm -rf myself");
}

void pidFeed() {
  taskCreate(
      dr4bLeftPid, TASK_DEFAULT_STACK_SIZE,
      (dr4bLeftTarget, encoderGet(dr4bleftencoder), 5, 0, 1),
      TASK_PRIORITY_DEFAULT);
  taskCreate(
      dr4bRightPid, TASK_DEFAULT_STACK_SIZE,
      (dr4bRightTarget, encoderGet(dr4brightencoder), 5, 0, 1),
      TASK_PRIORITY_DEFAULT);
  taskCreate(
      chainPid, TASK_DEFAULT_STACK_SIZE,
      (chainTarget, encoderGet(chainencoder), 5, 0, 1),
      TASK_PRIORITY_DEFAULT);
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
   chassisControl chassisControlLeft, chassisControlRight
   dr4bControl dr4bControl
   fineControlToggle fineBtn, reverseBtn
   mobileGoalControl moboLiftBtnUp, moboLiftBtnDown
   coneHandlerControl clawBtnUp, clawBtnDown, chainbarBtnUp, chainbarBtnDown
 */

void operatorControl() {
  lcdText();
  while (isEnabled()) {
    printf("right dr4b  %d\n", encoderGet(dr4brightencoder));
    printf("left dr4b  %d\n", encoderGet(dr4bleftencoder));
    printf("chain  %d\n", encoderGet(chainencoder));
    //argument based control scheme
    driveControl(joystickGetAnalog(1, 3), joystickGetAnalog(1, 2));
    dr4bControl(joystickGetAnalog(2, 2));
    fineControlToggle(joystickGetDigital(1, 7, JOY_DOWN), joystickGetDigital(1, 7, JOY_UP), joystickGetDigital(1, 8, JOY_UP), joystickGetDigital(1, 8, JOY_DOWN));
    mobileGoalControl( joystickGetDigital(1, 6, JOY_UP), joystickGetDigital(1, 6, JOY_DOWN));
    coneHandlerControl(joystickGetDigital(2, 5, JOY_UP), joystickGetDigital(2, 5, JOY_DOWN), joystickGetAnalog(2, 3));
    delay(20);
  }
}