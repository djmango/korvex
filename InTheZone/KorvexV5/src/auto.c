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
    driveTo(570, 570, 1800);
    // pickup 1st mobile goal
    motorSet(mobileGoal, -127);
    delay(2000);
    motorSet(mobileGoal, 0);
    // turn 180 degrees
    turnTo(191, 2000);
    // drive into 20 point
    driveTo(850, 850, 2300);
    motorSet(mobileGoal, 127);
    driveTo(150, 150, 900);
    motorSet(mobileGoal, 40);
    // lower mobile goal intake, drop 1st mobile goal in 20 pt
    motorSet(mobileGoal, -50);
    driveTo(-320, -370, 1900);
    motorSet(mobileGoal, 127);
    // turn to line up with 5 pt line
    turnTo(-100, 1800);
    motorSet(mobileGoal, 20);
    // drive to line up with 2nd mobile goal
    driveTo(370, 370, 2000);
    motorSet(mobileGoal, 0);
    // turn to line up with 2nd mobile goal
    turnTo(-44, 1800);
    // drive into 2nd mobile goal
    driveTo(700, 715, 2200);
    // pick up 2nd mobile goal and drive back
    motorSet(mobileGoal, -127);
    delay(1900);
    motorSet(mobileGoal, 0);
    driveTo(-700, -700, 2600);
    // turn to line up with 5 pt line
    turnTo(-155, 1200);
    // drive to line up drop of 2nd mobile goal
    driveTo(230, 230, 1500);
    // turn to drop off 2nd mobile goal
    turnTo(-100, 1000);
    // drive forward to drop off 2nd mobile goal
    motorSet(mobileGoal, 127);
    driveTo(180, 180, 1000);
    // drop 2nd mobile goal in 10 pt
    driveTo(-70, -70, 800);
    motorSet(mobileGoal, 0);
    // turn to line up with 5 pt line
    turnTo(90, 1600);
    // drive along 5 pt for 3rd mobile goal
    driveTo(360, 360, 1800);
    // turn to line up with 3rd mobile goal
    turnTo(86, 1100);
    // drive into 3rd mobile goal
    motorSet(mobileGoal, 20);
    driveTo(350, 350, 1700);
    // pick up 3rd mobile goal
    motorSet(mobileGoal, -127);
    delay(2300);
    motorSet(mobileGoal, 0);
    turnTo(185, 1800);
    // drive to drop 3rd mobile goal in 10 pt
    driveTo(650, 650, 2200);
    // drop off 3rd mobile goal in 10 pt
    motorSet(mobileGoal, 127);
    delay(1000);
    driveTo(-120, -120, 500);
    motorSet(mobileGoal, 0);
    // turn around to line up with 4th mobile goal
    turnTo(-186, 1800);
    // drive into 4th mobile goal
    driveTo(860, 860, 2800);
    motorSet(mobileGoal, -127);
    delay(2100);
    motorSet(mobileGoal, 0);
    // drive to 10 pt zone
    driveTo(850, 850, 2900);
    // drop off 4th mobile goal
    motorSet(mobileGoal, 127);
    driveTo(150, 150, 900);
    delay(1000);
    // lower mobile goal intake, drop 1st mobile goal in 20 pt
    motorSet(mobileGoal, -50);
    driveTo(-370, -320, 1900);
    motorSet(mobileGoal, 0);
    break;
  case -1: // test
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    // test
    turnTo(300, 6000);
  default :
    break;
  }
}