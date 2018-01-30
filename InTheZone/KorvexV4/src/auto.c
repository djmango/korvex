/** @file auto.c
 * @brief File for autonomous code
 */

#include "main.h"
#include "constants.h"
#include "korvexlib.h"

/*
 * Runs the user autonomous code. This function will be started in its own task
 * with the default
 * priority and stack size whenever the robot is enabled via the Field
 * Management System or the
 * VEX Competition Switch in the autonomous mode. If the robot is disabled or
 * communications is
 * lost, the autonomous task will be stopped by the kernel. Re-enabling the
 * robot will restart
 * the task, not re-start it from where it left off.
 *
 * Code running in the autonomous task cannot access information from the VEX
 * Joystick. However,
 * the autonomous function can be invoked from another task if a VEX Competition
 * Switch is not
 * available, and it can access joystick information if called in this way.
 *
 * The autonomous task may exit, unlike operatorControl() which should never
 * exit. If it does
 * so, the robot will adelay a switch to another mode or disable/enable cycle.
 */

/*-----------------------------------------------------------------------------*/
/*  Funky messages for the lcd */
/*-----------------------------------------------------------------------------*/
void lcdTextA()
{
  lcdSetBacklight(uart1, true);
  lcdSetText(uart1, 1, "it works");
  lcdSetText(uart1, 2, "i  swear");
}

/* motors:
  driveLeft
  driveRight
  mobileGoal
  dr4bLeft
  dr4bRight
  chainBar
  claw
*/

void autonomous() {
  lcdTextA();
  debugGlobal = false;
  int auton = 0;
  switch (auton) {
  case 0 : // 5 point 1 cone left
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    encoderReset(chainencoder);
    encoderReset(dr4bencoder);
    encoderReset(mobilegoalencoder);
    // lower mobile goal intake
    motorSet(claw, 30);
    motorSet(mobileGoal, -127);
    liftTo(20, 40, 1000);
    // drive thru cones and into mobile goal
    driveTo(500, 500, 1700);
    motorSet(mobileGoal, -127);
    delay(100);
    motorSet(mobileGoal, 0);
    driveTo(900, 900, 1200);
    liftTo(40, 40, 1000);
    motorSet(mobileGoal, 127);
    liftTo(40, 40, 1000); // 6.2 secs in
    motorSet(mobileGoal, 0);
    // stack
    liftTo(0, 20, 400);
    motorSet(claw, -90);
    liftTo(0, 20, 600);
    motorSet(claw, 5);
    liftTo(0, 215, 1200);
    motorSet(claw, 0);
    // backwards
    driveTo(300, 300, 1200);
    // turn and drop
    driveTo(600, 0, 700);
    motorSet(mobileGoal, -127);
    liftTo(20, 215, 1400);
    motorSet(mobileGoal, 0);
    driveTo(600, -100, 1000);
    break;
  default :
    break;
  }
}
