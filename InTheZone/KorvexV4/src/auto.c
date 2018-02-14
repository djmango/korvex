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
  debugGlobal = true;
  coneIncrementGlobal = 0;
  auton = 2; // 1-6 reserved for auton, 7 skills, 9 defence, 10 test, -10 for selection
  while (auton == -10) { // if auton has not been selected yet
    lcdAutSel(lcdReadButtons(uart1));
    delay(100);
  }
  lcdSetBacklight(uart1, false);
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
  case 2: // 20 point 1 cone red left
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    encoderReset(chainencoder);
    encoderReset(dr4bencoder);
    // lower mobile goal intake
    motorSet(mobileGoal, 127);
    motorSet(claw, -20);
    liftTo(20, 40, 1000);
    // account for jolt with slight right tilt
    motorSet(mobileGoal, 30);
    driveTo(125, 100, 900);
    // drive into mobile goal and pickup
    driveTo(710, 750, 2000);
    motorSet(mobileGoal, -127);
    delay(300);
    motorSet(mobileGoal, -80);
    // drive back and stack
    driveTo(-700, -700, 2200);
    liftTo(10, 35, 500);
    delay(200);
    motorSet(claw, 127);
    delay(100);
    motorSet(claw, 0);
    motorSet(mobileGoal, 0);
    // turn to line up with 5 pt line
    driveTo(250, -250, 1400);
    // drive along 5 pt line
    driveTo(430, 430, 1500);
    // turn to face 20 pt
    driveTo(145, -145, 1000);
    // drive into 20 pt weeeeee
    driveTo(500, 500, 1800);
    motorSet(mobileGoal, 127);
    liftTo(15, 30, 1400);
    motorSet(mobileGoal, -127);
    driveTo(-500, -500, 2000);
    motorSet(mobileGoal, 0);
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
    driveToSkills(580, 580, 2200);
    // pickup 1st mobile goal
    motorSet(mobileGoal, -127);
    delay(400);
    // turn 180 degrees
    driveToSkills(-300, 300, 1400);
    motorSet(mobileGoal, 0);
    // drive back
    driveToSkills(670, 670, 1900);
    // lower mobile goal intake, drop 1st mobile goal in 10 pt
    motorSet(mobileGoal, 127);
    delay(1000);
    motorSet(mobileGoal, 0);
    driveToSkills(-150, -150, 1300);
    // turn to line up with 5 pt line
    driveToSkills(150, -150, 700);
    // drive to line up with 2nd mobile goal
    driveToSkills(150, 150, 1200);
    // turn to line up with 2nd mobile goal
    driveToSkills(80, -80, 1200);
    // drive into 2nd mobile goal
    driveToSkills(700, 720, 2200);
    // pick up 2nd mobile goal and drive back
    motorSet(mobileGoal, -127);
    delay(400);
    driveToSkills(-670, -670, 2100);
    motorSet(mobileGoal, 0);
    // turn to line up with wall
    driveToSkills(180, -180, 1000);
    // ram into wall for accuracy
    driveToSkills(-150, -150, 800);
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    driveToSkills(100, 100, 700);
    // turn to line up with 5 pt line
    driveToSkills(75, -75, 700);
    // drive to line up drop of 2nd mobile goal
    driveToSkills(400, 400, 1200);
    // turn to drop off 2nd mobile goal
    driveToSkills(170, -170, 1200);
    // drive forward to drop off 2nd mobile goal
    driveToSkills(175, 175, 800);
    // lower mobile goal intake, drop 2nd mobile goal in 10 pt
    motorSet(mobileGoal, 127);
    delay(1000);
    motorSet(mobileGoal, 0);
    driveToSkills(-150, -150, 800);
    // turn to line up with 5 pt line
    driveToSkills(-150, 150, 1200);
    // drive along 5 pt for 3rd mobile goal
    driveToSkills(270, 270, 1800);
    // turn to line up with 3rd mobile goal
    driveToSkills(-150, 150, 1200);
    // drive into 3rd mobile goal
    driveToSkills(450, 450, 1400);
    // pick up 3rd mobile goal
    motorSet(mobileGoal, -127);
    delay(200);
    driveToSkills(-100, -100, 300);
    driveToSkills(-305, 305, 1400);
    motorSet(mobileGoal, 0);
    // drive to drop 3rd mobile goal in 10 pt
    driveToSkills(610, 610, 2200);
    // drop off 3rd mobile goal in 10 pt
    motorSet(mobileGoal, 127);
    delay(1000);
    motorSet(mobileGoal, 0);
    driveToSkills(-150, -150, 500);
    // turn around to line up with 4th mobile goal
    driveToSkills(-310, 310, 2000);
    // drive into 4th mobile goal
    driveToSkills(600, 600, 2000);
    driveToSkills(200, 200, 1000);
    motorSet(mobileGoal, -100);
    delay(200);
    // drive to 10 pt zone
    driveToSkills(650, 650, 1700);
    motorSet(mobileGoal, 0);
    // drop off 4th mobile goal
    motorSet(mobileGoal, 127);
    delay(1000);
    motorSet(mobileGoal, 0);
    break;
  case 9: // defence
    driveTo(1000, 1000, 4000);
    break;
  case 10: // test
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    encoderReset(chainencoder);
    encoderReset(dr4bencoder);
    liftTo(10, 50, 15000);
    break;
  default :
    break;
  }
}
