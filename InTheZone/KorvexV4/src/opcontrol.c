#include "main.h"
#include "constants.h"
#include "korvexlib.h"

/*-----------------------------------------------------------------------------*/
/*  Funky messages for the lcd */
/*-----------------------------------------------------------------------------*/
void lcdText() {
  lcdSetBacklight(uart1, true);
  lcdSetText(uart1, 1, "i want to");
  lcdSetText(uart1, 2, "sudo rm -rf myself");
}

/*control arguement names
   chassisControl chassisControlLeft, chassisControlRight
   dr4bControl dr4bControl
   fineControlToggle fineBtn, reverseBtn
   mobileGoalControl moboLiftBtnUp, moboLiftBtnDown
   coneHandlerControl clawBtnUp, clawBtnDown, chainbarBtnUp, chainbarBtnDown
 */

void operatorControl() {
  debugGlobal = false;
  isReverse = false;
  isFineControl = false;
  fineControl = 1;
  coneIncrementGlobal = 0;
  isDriverloadGlobal = false;
  while (isEnabled()) {
    //argument based control scheme
    if (debugGlobal == true) {
      printf("cc%d\n", coneIncrementGlobal);
      printf("d%d\n", encoderGet(dr4bencoder));
      printf("c%d\n", encoderGet(chainencoder));
      printf("r%d\n", encoderGet(rightencoder));
      printf("l%d\n", encoderGet(leftencoder));
    }
    driveControl(joystickGetAnalog(1, 2), joystickGetAnalog(1, 3));
    dr4bControl(joystickGetAnalog(2, 2));
    mobileGoalControl(joystickGetDigital(1, 6, JOY_UP), joystickGetDigital(1, 6, JOY_DOWN));
    coneHandlerControl(joystickGetDigital(2, 5, JOY_UP), joystickGetDigital(2, 5, JOY_DOWN), joystickGetAnalog(2, 3));
    autoStackControl(joystickGetDigital(2, 7, JOY_UP), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2,7, JOY_RIGHT), joystickGetDigital(2, 8, JOY_RIGHT), joystickGetDigital(2, 8, JOY_LEFT), joystickGetDigital(2, 8, JOY_UP));
    delay(20);
  }
}