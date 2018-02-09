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
  debugGlobal = true;
  coneIncrementGlobal = 0;
  int auton = 7; // 1-6 reserved for auton, 7 skills, 9 defence, 10 test
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
  case 7: // skills
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    // lower mobile goal intake
    motorSet(mobileGoal, 127);
    delay(1000);
    motorSet(mobileGoal, 0);
    // drive into 1st mobile goal
    driveToSkills(560, 560, 1500);
    // pickup mobile goal
    motorSet(mobileGoal, -127);
    delay(400);
    // turn 180 degrees
    driveToSkills(-300, 300, 1400);
    motorSet(mobileGoal, 0);
    // drive back
    driveToSkills(650, 650, 2400);
    // lower mobile goal intake, drop 1st mobile goal in 10 pt
    motorSet(mobileGoal, 127);
    delay(1000);
    motorSet(mobileGoal, 0);
    driveToSkills(-150, -150, 1500);
    // turn
    driveToSkills(150, -150, 800);
    // drive to line up with 2nd mobile goal
    driveToSkills(150, 150, 1000);
    // turn to line up with 2nd mobile goal
    driveToSkills(80, -80, 1200);
    // drive into mobile goal
    driveToSkills(700, 700, 2200);
    // pick up 2nd mobile goal and drive back
    motorSet(mobileGoal, -127);
    delay(400);
    driveToSkills(-700, -700, 2200);
    motorSet(mobileGoal, 0);
    // turn to line up with 5 pt line
    driveToSkills(180, -180, 1000);
    driveToSkills(-120, -120, 600);
    driveToSkills(100, 100, 700);
    driveToSkills(75, -75, 700);
    // drive to line up drop of 2nd mobile goal
    driveToSkills(400, 400, 1200);
    // turn to drop off 2nd mobile goal
    driveToSkills(170, -170, 1200);
    // drive forward to drop off 2nd mobile goal
    driveToSkills(200, 200, 800);
    // lower mobile goal intake, drop 2nd mobile goal in 10 pt
    motorSet(mobileGoal, 127);
    delay(1000);
    motorSet(mobileGoal, 0);
    driveToSkills(-150, -150, 800);
    // turn to line up with 5 pt line
    driveToSkills(140, -140, 1200);
    break;
  case 8: // 20 pt skills, barely works
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    // lower mobile goal intake
    motorSet(mobileGoal, 127);
    delay(1000);
    motorSet(mobileGoal, 0);
    // drive into mobile goal
    driveToSkills(520, 520, 1800);
    // pickup mobile goal and drive back
    motorSet(mobileGoal, -127);
    delay(400);
    driveToSkills(-370, -370, 1800);
    motorSet(mobileGoal, 0);
    // turn to start line up with 20pt
    driveToSkills(170, -170, 1000);
    // drive to line up with 20 pt
    driveToSkills(290, 290, 1400);
    // turn to face 20 pt
    driveToSkills(160, -160, 1000);
    // drive into 20 pt
    driveToSkills(450, 450, 2500);
    // lower mobile goal intake
    motorSet(mobileGoal, 127);
    // driveToSkills(150, 150, 1500); // note future self. if this doesnt work, swap
    delay(1500);
    motorSet(mobileGoal, 0);
    // raise mobile goal intake
    driveToSkills(-100, -100, 1000);
    motorSet(mobileGoal, -127);
    delay(200);
    motorSet(mobileGoal, -127);
    // driveToSkills(-500, -500, 1800);
    delay(1800);
    motorSet(mobileGoal, 0);
    driveToSkills(-400, -400, 3000);
    break;
  case 9: // defence
    driveTo(1000, 1000, 4000);
    break;
  case 10: // test
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    encoderReset(chainencoder);
    encoderReset(dr4bencoder);
    driveToSkills(200, 200, 5000);
    driveToSkills(-200, -200, 5000);
  default :
    break;
  }
}
