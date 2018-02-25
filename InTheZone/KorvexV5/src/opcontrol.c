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
  debugGlobal = false;
  lcdSetBacklight(uart1, true);
  isRecording = false;
  int count = 0;
  while (isEnabled()) {
    // values of assorted things on the robot
    if (debugGlobal == true) {
      printf("r%d\n", encoderGet(rightencoder));
      printf("l%d\n", encoderGet(leftencoder));
    }
    if (isRecording == true && count == 5) {
      count = 0;
      rerunRecord();
    }
    //argument based control scheme
    driveControl(joystickGetAnalog(1, 2), joystickGetAnalog(1, 3));
    mobileGoalControl(joystickGetDigital(1, 6, JOY_UP), joystickGetDigital(1, 6, JOY_DOWN));
    count = count + 1;
    delay(20);
  }
}