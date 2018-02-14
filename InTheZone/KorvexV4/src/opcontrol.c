#include "main.h"
#include "constants.h"
#include "korvexlib.h"

/*control arguement names
   chassisControl chassisControlLeft, chassisControlRight
   dr4bControl dr4bControl
   fineControlToggle fineBtn, reverseBtn
   mobileGoalControl moboLiftBtnUp, moboLiftBtnDown
   coneHandlerControl clawBtnUp, clawBtnDown, chainbarBtnUp, chainbarBtnDown
 */

void operatorControl() {
  debugGlobal = true;
  lcdSetBacklight(uart1, true);
  lcdHoldGlobal = 0;
  auton = -10;
  coneIncrementGlobal = 0;
  isRecording = false;
  isDriverloadGlobal = false;
  int count = 0;
  while (isEnabled()) {
    // values of assorted things on the robot
    if (debugGlobal == true) {
      printf("cc%d\n", coneIncrementGlobal);
      printf("d%d\n", encoderGet(dr4bencoder));
      printf("c%d\n", encoderGet(chainencoder));
      printf("r%d\n", encoderGet(rightencoder));
      printf("l%d\n", encoderGet(leftencoder));
    }
    if (auton == -10)  { // if auton has not been selected yet
      lcdAutSel(lcdReadButtons(uart1));
      delay(100);
    }
    if (isRecording == true) {
      if (count == 5) {
        count = 0;
        rerunRecord();
      }
    }
    //argument based control scheme
    driveControl(joystickGetAnalog(1, 2), joystickGetAnalog(1, 3));
    dr4bControl(joystickGetAnalog(2, 2));
    mobileGoalControl(joystickGetDigital(1, 6, JOY_UP), joystickGetDigital(1, 6, JOY_DOWN));
    coneHandlerControl(joystickGetDigital(2, 5, JOY_UP), joystickGetDigital(2, 5, JOY_DOWN), joystickGetAnalog(2, 3));
    autoStackControl(joystickGetDigital(2, 7, JOY_UP), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2,7, JOY_RIGHT), joystickGetDigital(2, 8, JOY_RIGHT), joystickGetDigital(2, 8, JOY_LEFT), joystickGetDigital(2, 8, JOY_UP));
    count = count + 1;
    delay(20);
  }
}