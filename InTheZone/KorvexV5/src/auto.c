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
    delay(500);
    motorSet(mobileGoal, 10);
    // drive into 1st mobile goal
    driveTo(570, 570, 1500);
    // pickup 1st mobile goal
    motorSet(mobileGoal, -127);
    // turn 190 degrees
    turnTo(193, 1800);
    motorSet(mobileGoal, 0);
    // drive into 20 point
    driveTo(850, 850, 2000);
    motorSet(mobileGoal, 127);
    driveTo(150, 150, 600);
    delay(200);
    // lower mobile goal intake, drop 1st mobile goal in 20 pt
    motorSet(mobileGoal, -127);
    delay(400);
    motorSet(mobileGoal, -50);
    // drive back to 5 pt line
    driveTo(-320, -320, 1400);
    motorSet(mobileGoal, 80);
    // turn to line up with 5 pt line
    turnTo(-110, 1600);
    motorSet(mobileGoal, 10);
    // drive to line up with 2nd mobile goal
    driveTo(430, 430, 2000);
    motorSet(mobileGoal, 0);
    // turn to line up with 2nd mobile goal
    turnTo(-40, 1000);
    // drive into 2nd mobile goal
    driveTo(600, 615, 1800);
    // pick up 2nd mobile goal and drive back
    motorSet(mobileGoal, -127);
    delay(200);
    motorSet(mobileGoal, -70);
    driveTo(-670, -670, 2200);
    motorSet(mobileGoal, -10);
    // turn to line up with 5 pt line
    turnTo(-155, 1000);
    // drive to line up drop of 2nd mobile goal
    driveTo(230, 230, 1200);
    // turn to drop off 2nd mobile goal
    turnTo(-100, 1000);
    // drive forward to drop off 2nd mobile goal
    driveTo(100, 100, 400);
    motorSet(mobileGoal, 80);
    driveTo(100, 95, 600);
    // drop 2nd mobile goal in 10 pt
    driveTo(-120, -120, 900);
    motorSet(mobileGoal, 0);
    // turn to line up with 5 pt line
    turnTo(90, 1500);
    // drive along 5 pt for 3rd mobile goal
    driveTo(420, 420, 2100);
    // turn to line up with 3rd mobile goal
    turnTo(89, 1500);
    // drive into 3rd mobile goal
    motorSet(mobileGoal, 20);
    driveTo(390, 390, 1700);
    // pick up 3rd mobile goal
    motorSet(mobileGoal, -127);
    delay(400);
    motorSet(mobileGoal, -80);
    turnTo(192, 2000);
    motorSet(mobileGoal, 0);
    // drive to drop 3rd mobile goal in 10 pt
    driveTo(670, 670, 2300);
    // drop off 3rd mobile goal in 10 pt
    motorSet(mobileGoal, 127);
    delay(800);
    driveTo(-120, -120, 500);
    motorSet(mobileGoal, 10);
    // turn around to line up with 4th mobile goal
    turnTo(-182, 1800);
    // drive into 4th mobile goal
    driveTo(900, 900, 3200);
    motorSet(mobileGoal, -127);
    turnTo(20, 1000);
    motorSet(mobileGoal, 0);
    // drive to 10 pt zone
    driveTo(900, 900, 3100);
    // drop off 4th mobile goal
    motorSet(mobileGoal, 127);
    driveTo(150, 150, 900);
    delay(1000);
    // lower mobile goal intake, drop 1st mobile goal in 20 pt
    motorSet(mobileGoal, -50);
    driveTo(-370, -370, 1900);
    motorSet(mobileGoal, 0);
    break;
  case 3: // skills 4
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    // lower mobile goal intake
    motorSet(mobileGoal, 127);
    delay(1500);
    motorSet(mobileGoal, 30);
    // drive into 1st mobile goal
    driveTo(570, 570, 1700);
    // pickup 1st mobile goal
    motorSet(mobileGoal, -127);
    delay(2000);
    motorSet(mobileGoal, 0);
    // turn 180 degrees
    turnTo(190, 1800);
    // drive into 20 point
    driveTo(850, 850, 2300);
    motorSet(mobileGoal, 127);
    driveTo(150, 150, 900);
    motorSet(mobileGoal, 40);
    // lower mobile goal intake, drop 1st mobile goal in 20 pt
    motorSet(mobileGoal, -50);
    driveTo(-320, -320, 1900);
    motorSet(mobileGoal, 80);
    // turn to line up with 5 pt line
    turnTo(-130, 1600);
    motorSet(mobileGoal, 20);
    // drive to line up with 2nd mobile goal
    driveTo(620, 620, 2600);
    motorSet(mobileGoal, 0);
    // turn to line up with 2nd mobile goal
    turnTo(-20, 800);
    // drive into 2nd mobile goal
    driveTo(400, 400, 1400);
    // pick up 2nd mobile goal and drive back
    motorSet(mobileGoal, -127);
    delay(2600);
    motorSet(mobileGoal, 0);
    // turn to drive back
    turnTo(25, 1000);
    // drive to line up drop of 2nd mobile goal
    driveTo(-600, -600, 2500);
    // turn to drop off 2nd mobile goal
    turnTo(130, 1000);
    // drive forward to drop off 2nd mobile goal
    motorSet(mobileGoal, 60);
    driveTo(350, 350, 2500);
    // drop 2nd mobile goal in 10 pt
    driveTo(-70, -70, 800);
    motorSet(mobileGoal, 0);
    // turn to line up with 5 pt line
    turnTo(90, 1600);
    // drive along 5 pt for 3rd mobile goal
    driveTo(420, 420, 2100);
    // turn to line up with 3rd mobile goal
    turnTo(89, 1500);
    // drive into 3rd mobile goal
    motorSet(mobileGoal, 20);
    driveTo(390, 390, 1700);
    // pick up 3rd mobile goal
    motorSet(mobileGoal, -127);
    delay(2300);
    motorSet(mobileGoal, 0);
    turnTo(185, 1800);
    // drive to drop 3rd mobile goal in 10 pt
    driveTo(680, 680, 2300);
    // drop off 3rd mobile goal in 10 pt
    motorSet(mobileGoal, 127);
    delay(1000);
    driveTo(-120, -120, 500);
    motorSet(mobileGoal, 30);
    // turn around to line up with 4th mobile goal
    turnTo(-182, 1800);
    // drive into 4th mobile goal
    driveTo(900, 900, 3200);
    motorSet(mobileGoal, -127);
    turnTo(15, 2100);
    motorSet(mobileGoal, 0);
    // drive to 10 pt zone
    driveTo(900, 900, 3100);
    // drop off 4th mobile goal
    motorSet(mobileGoal, 127);
    driveTo(150, 150, 900);
    delay(1000);
    // lower mobile goal intake, drop 1st mobile goal in 20 pt
    motorSet(mobileGoal, -50);
    driveTo(-370, -370, 1900);
    motorSet(mobileGoal, 0);
  case -1: // test
    encoderReset(rightencoder);
    encoderReset(leftencoder);
    // test
    moboTo(50, 1000);
  default :
    break;
  }
}