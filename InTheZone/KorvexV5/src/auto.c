#include "main.h"
#include "constants.h"
#include "korvexlib.h"

void autonomous() {
  debugGlobal = true;
  lcdSetBacklight(uart1, false);
  int auton = 2;
  switch (auton) {
  case 0: // skills 1
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    // lower mobile goal intake
    motorSet(mobileGoal, 127);
    driveTo(500, 500, 1000);
    motorSet(mobileGoal, 0);
    break;
  case 1: // skills 2
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    // lower mobile goal intake
    motorSet(mobileGoal, 127);
    delay(200);
    // drive into 1st mobile goal
    driveTo(750, 750, 1800);
    // pick up 1st mobile goal and drive back
    motorSet(mobileGoal, -127);
    delay(2500);
    driveTo(-600, -600, 2100);
    motorSet(mobileGoal, 0);
    // turn to line up with with 5 pt
    driveTo(250, -250, 1700);
    // drive to line up drop of 1st mobile goal
    driveTo(450, 450, 1200);
    // turn to drop off 1st mobile goal
    driveTo(270, -120, 1800);
    // drive forward to drop off 1st mobile goal
    motorSet(mobileGoal, 80);
    driveTo(275, 275, 800);
    // lower mobile goal intake, drop 1st mobile goal in 10 pt
    motorSet(mobileGoal, 127);
    delay(1000);
    motorSet(mobileGoal, 0);
    driveTo(-150, -150, 800);
    // turn to line up with 5 pt line
    driveTo(-150, 150, 1200);
    // drive along 5 pt for 3rd mobile goal
    driveTo(270, 270, 1800);
    // turn to line up with 3rd mobile goal
    driveTo(-150, 150, 1200);
    // drive into 3rd mobile goal
    driveTo(450, 450, 1400);
    // pick up 3rd mobile goal
    motorSet(mobileGoal, -127);
    delay(200);
    driveTo(-100, -100, 300);
    driveTo(-305, 305, 1400);
    motorSet(mobileGoal, 0);
    // drive to drop 3rd mobile goal in 10 pt
    driveTo(610, 610, 2200);
    // drop off 3rd mobile goal in 10 pt
    motorSet(mobileGoal, 127);
    delay(1000);
    motorSet(mobileGoal, 0);
    driveTo(-150, -150, 500);
    // turn around to line up with 4th mobile goal
    driveTo(-310, 310, 2000);
    // drive into 4th mobile goal
    driveTo(600, 600, 2000);
    driveTo(200, 200, 1000);
    motorSet(mobileGoal, -100);
    delay(200);
    // drive to 10 pt zone
    driveTo(650, 650, 1700);
    motorSet(mobileGoal, 0);
    // drop off 4th mobile goal
    motorSet(mobileGoal, 127);
    delay(1000);
    motorSet(mobileGoal, 0);
    break;
  case 2: // skills 3
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    // lower mobile goal intake
    motorSet(mobileGoal, 127);
    delay(1500);
    motorSet(mobileGoal, 30);
    // drive into 1st mobile goal
    driveTo(570, 570, 2000);
    // pickup 1st mobile goal
    motorSet(mobileGoal, -127);
    delay(2000);
    motorSet(mobileGoal, 0);
    // turn 180 degrees
    driveTo(-220, 220, 2000);
    // drive back
    driveTo(570, 570, 1800);
    // turn to face 20 point
    driveTo(-30, 30, 400);
    driveTo(400, 400, 1600);
    motorSet(mobileGoal, 127);
    driveTo(120, 120, 600);
    // lower mobile goal intake, drop 1st mobile goal in 20 pt
    motorSet(mobileGoal, -127);
    driveTo(-270, -300, 2500);
    motorSet(mobileGoal, 127);
    // turn to line up with 5 pt line
    driveTo(180, -180, 700);
    // drive to line up with 2nd mobile goal
    driveTo(220, 220, 1200);
    motorSet(mobileGoal, 0);
    // turn to line up with 2nd mobile goal
    driveTo(90, -90, 1600);
    // drive into 2nd mobile goal
    driveTo(700, 720, 2200);
    // pick up 2nd mobile goal and drive back
    motorSet(mobileGoal, -127);
    delay(2300);
    motorSet(mobileGoal, 0);
    driveTo(-670, -670, 2100);
    // turn to line up with wall
    driveTo(180, -180, 1000);
    // ram into wall for accuracy
    driveTo(-150, -150, 800);
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    driveTo(100, 100, 700);
    // turn to line up with 5 pt line
    driveTo(75, -75, 700);
    // drive to line up drop of 2nd mobile goal
    driveTo(400, 400, 1200);
    // turn to drop off 2nd mobile goal
    driveTo(170, -170, 1200);
    // drive forward to drop off 2nd mobile goal
    driveTo(175, 175, 800);
    // lower mobile goal intake, drop 2nd mobile goal in 10 pt
    motorSet(mobileGoal, 127);
    delay(1000);
    motorSet(mobileGoal, 0);
    driveTo(-150, -150, 800);
    // turn to line up with 5 pt line
    driveTo(-150, 150, 1200);
    // drive along 5 pt for 3rd mobile goal
    driveTo(270, 270, 1800);
    // turn to line up with 3rd mobile goal
    driveTo(-150, 150, 1200);
    // drive into 3rd mobile goal
    driveTo(450, 450, 1400);
    // pick up 3rd mobile goal
    motorSet(mobileGoal, -127);
    delay(2300);
    motorSet(mobileGoal, 0);
    driveTo(-100, -100, 300);
    driveTo(-305, 305, 1400);
    // drive to drop 3rd mobile goal in 10 pt
    driveTo(610, 610, 2200);
    // drop off 3rd mobile goal in 10 pt
    motorSet(mobileGoal, 127);
    delay(1000);
    motorSet(mobileGoal, 0);
    driveTo(-150, -150, 500);
    // turn around to line up with 4th mobile goal
    driveTo(-310, 310, 2000);
    // drive into 4th mobile goal
    driveTo(600, 600, 2000);
    driveTo(200, 200, 1000);
    motorSet(mobileGoal, -100);
    delay(200);
    // drive to 10 pt zone
    driveTo(650, 650, 1700);
    motorSet(mobileGoal, 0);
    // drop off 4th mobile goal
    motorSet(mobileGoal, 127);
    delay(1000);
    motorSet(mobileGoal, 0);
    break;
  case -1: // test
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    // test
    driveToStraight(500, 5000);
  default :
    break;
  }
}