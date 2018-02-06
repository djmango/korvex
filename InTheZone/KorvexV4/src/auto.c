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
  coneIncrementGlobal = 0;
  int auton = 1;
  switch (auton) {
  case 0: // 5 point 1 cone blue left
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    encoderReset(chainencoder);
    encoderReset(dr4bencoder);
    // lower mobile goal intake
    motorSet(claw, 127);
    delay(300);
    motorSet(claw, 20);
    motorSet(mobileGoal, 127);
    liftTo(0, 120, 1200);
    motorSet(mobileGoal, 0);
    // drive thru cones and into mobile goal
    driveTo(560, 520, 2000);
    // final drop of mobo just in case
    motorSet(mobileGoal, 90);
    delay(200);
    motorSet(mobileGoal, 0);
    // drive into mobo and pickup
    driveTo(900, 900, 1400);
    motorSet(mobileGoal, -127);
    liftTo(0, 130, 1500);
    motorSet(mobileGoal, 0);
    // stack
    liftTo(0, 130, 400);
    motorSet(claw, -90);
    liftTo(0, 130, 400);
    motorSet(claw, 0);
    // backwards
    driveTo(200, 200, 2200);
    // turn and drop
    driveTo(0, 400, 1500);
    motorSet(mobileGoal, 127);
    liftTo(0, 120, 1400);
    motorSet(mobileGoal, 0);
    driveTo(-300, 100, 1000);
    break;
  case 1: // 5 point 1 cone blue right
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    encoderReset(chainencoder);
    encoderReset(dr4bencoder);
    // lower mobile goal intake
    motorSet(claw, 127);
    delay(300);
    motorSet(claw, 20);
    motorSet(mobileGoal, 127);
    liftTo(0, 120, 1200);
    motorSet(mobileGoal, 0);
    // drive thru cones and into mobile goal
    driveTo(560, 520, 2000);
    // final drop of mobo just in case
    motorSet(mobileGoal, 90);
    delay(200);
    motorSet(mobileGoal, 0);
    // drive into mobo and pickup
    driveTo(900, 900, 1400);
    motorSet(mobileGoal, -127);
    liftTo(0, 130, 1500);
    motorSet(mobileGoal, 0);
    // stack
    liftTo(0, 130, 400);
    motorSet(claw, -90);
    liftTo(0, 130, 400);
    motorSet(claw, 0);
    // backwards
    driveTo(200, 200, 2200);
    // turn and drop
    driveTo(400, 0, 800);
    motorSet(mobileGoal, 127);
    liftTo(0, 120, 1400);
    motorSet(mobileGoal, 0);
    driveTo(300, -300, 1000);
    break;
  case 2: // 5 point 1 cone red left
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    encoderReset(chainencoder);
    encoderReset(dr4bencoder);
    // lower mobile goal intake
    motorSet(claw, 127);
    delay(300);
    motorSet(claw, 20);
    motorSet(mobileGoal, 127);
    liftTo(0, 120, 1200);
    motorSet(mobileGoal, 0);
    // drive thru cones and into mobile goal
    driveTo(440, 480, 1800);
    // final drop of mobo just in case
    motorSet(mobileGoal, 90);
    delay(200);
    motorSet(mobileGoal, 0);
    // drive into mobo and pickup
    driveTo(900, 900, 2200);
    motorSet(mobileGoal, -127);
    liftTo(0, 130, 1500);
    motorSet(mobileGoal, 0);
    // stack
    liftTo(0, 130, 400);
    motorSet(claw, -90);
    liftTo(0, 130, 400);
    motorSet(claw, 0);
    // backwards
    driveTo(200, 200, 2200);
    // turn and drop
    driveTo(0, 400, 1500);
    motorSet(mobileGoal, 127);
    liftTo(0, 120, 1400);
    motorSet(mobileGoal, 0);
    driveTo(-300, 100, 1000);
    break;
  case 3: // left tricky stationary
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    encoderReset(chainencoder);
    encoderReset(dr4bencoder);
    // grab cone
    motorSet(claw, 127);
    liftTo(0, 120, 300);
    motorSet(claw, 20);
    // turn to face stationary
    driveTo(535, 100, 2000);
    // move closer to stationary
    driveTo(700, 225, 1500);
    // lift
    liftTo(75, 100, 2000);
    // final position with stationary
    driveTo(855, 380, 1500);
    motorSet(claw, -40);
    delay(400);
    motorSet(claw, 0);
    liftTo(30, 130, 1000);
    driveTo(700, 250, 1000);
    liftTo(0, 10, 1000);
    break;
  case 4: // 10 point 1 cone blue left
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    encoderReset(chainencoder);
    encoderReset(dr4bencoder);
    // lower mobile goal intake
    motorSet(claw, 127);
    delay(300);
    motorSet(claw, 20);
    motorSet(mobileGoal, 127);
    liftTo(0, 120, 1200);
    motorSet(mobileGoal, 0);
    // drive thru cones and into mobile goal
    driveTo(520, 520, 2000);
    motorSet(mobileGoal, 90);
    delay(400);
    motorSet(mobileGoal, 0);
    driveTo(900, 900, 1200);
    motorSet(mobileGoal, -127);
    liftTo(0, 130, 1500);
    motorSet(mobileGoal, 0);
    // stack
    liftTo(0, 130, 400);
    motorSet(claw, -90);
    liftTo(0, 130, 400);
    motorSet(claw, 0);
    // backwards
    driveTo(-300, 200, 3500);
    // turn and drop
    driveTo(200, 700, 1500);
    motorSet(mobileGoal, 127);
    liftTo(0, 120, 1400);
    motorSet(mobileGoal, 0);
    driveTo(-100, 100, 1000);
    break;
  case 8: // defence
    driveTo(1000, 1000, 4000);
    break;
  case 9: // test
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    encoderReset(chainencoder);
    encoderReset(dr4bencoder);
    liftTo(10, 0, 100000);
  default :
    break;
  }
}
