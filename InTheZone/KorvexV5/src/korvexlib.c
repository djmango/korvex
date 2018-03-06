#include "main.h"
#include "constants.h"
#include "korvexlib.h"

/*-----------------------------------------------------------------------------*/
/*  drive control, user input, direct                                          */
/*-----------------------------------------------------------------------------*/
void driveControl(int chassisControlLeft, int chassisControlRight) {
  // chassis control
  motorSet(driveLeft, (chassisControlRight * -1));
  motorSet(driveRight, (chassisControlLeft * -1));
  motorSet(driveLeft2, (chassisControlRight * -1));
  motorSet(driveRight2, (chassisControlLeft * -1));
}

/*-----------------------------------------------------------------------------*/
/*  mobile goal control, user input, direct                                    */
/*-----------------------------------------------------------------------------*/
void mobileGoalControl(int moboLiftBtnUp, int moboLiftBtnDown) {
  // mobo lift control
  if (moboLiftBtnUp == 1) {
    motorSet(mobileGoal, 127);
  }
  if (moboLiftBtnDown == 1) {
    motorSet(mobileGoal, -127);
  }
  if (moboLiftBtnUp == 0 && moboLiftBtnDown == 0) {
    motorSet(mobileGoal, 0);
  }
}

/*-----------------------------------------------------------------------------*/
/*  mobile goal intake encoder based control                                   */
/*-----------------------------------------------------------------------------*/
void moboTo(int target, int waitTo) {
  int count = 0;
  float p;
  float d;
  int lastError = 0;
  int error = (target - encoderGet(mobilegoalencoder));
  int drive;
  // offset the targets, for easier readability
  error = (target - encoderGet(mobilegoalencoder));
  while (true)
  {
    if (count == (waitTo / 100) || (error == 0 && lastError == 0))
    {
      if (debugGlobal == true)
      {
        printf("task completed\n");
      }
      motorSet(mobileGoal, 0);
      return;
    }
    else
    {
      // calculate error
      error = (target - encoderGet(mobilegoalencoder));

      // calculate pid based on battery
      p = (error * 1);
      d = ((error - lastError) * 3.5);

      // store last error
      lastError = error;

      // calculate drive
      drive = (p + d);

      // if we are in debug mode, print error
      if (debugGlobal == true)
      {
        printf("rErr%d\n", error);
        printf("rDri%d\n", drive);
      }

      // set motor to drive
      motorSet(mobileGoal, drive * -1);
      count = count + 1;
      delay(100);
    }
  }
}

/*-----------------------------------------------------------------------------*/
/*  argument based encoder pid, for drive, tuned for skills mode               */
/*-----------------------------------------------------------------------------*/
void driveTo(int leftTarget, int rightTarget, int waitTo) {
  int leftError;
  int rightError;
  int leftDrive;
  int rightDrive;
  int leftLastError = 0;
  int rightLastError = 0;
  float leftP;
  float rightP;
  float leftI = 0;
  float rightI = 0;
  float leftD;
  float rightD;
  int count = 0;
  // offset the targets, for easier readability
  leftTarget = encoderGet(leftencoder) + leftTarget;
  rightTarget = encoderGet(rightencoder) + rightTarget;
  leftError = (leftTarget - encoderGet(leftencoder));
  rightError = (rightTarget - encoderGet(rightencoder));
  while (true) {
    if (count == (waitTo / 100) || (leftError == 0 && leftLastError == 0 && rightError == 0 && rightLastError == 0)) {
      if (debugGlobal == true) {
        printf("task completed\n");
      }
      motorSet(driveLeft, 0);
      motorSet(driveLeft2, 0);
      motorSet(driveRight, 0);
      motorSet(driveRight2, 0);
      return;
    } else {
      // calculate error
      leftError = (leftTarget - encoderGet(leftencoder));
      rightError = (rightTarget - encoderGet(rightencoder));

      // calculate pid based on battery
      if (powerLevelMain() > 8.2)
      { // if battery is above 8.2 volts
        leftP = (leftError * .95);
        if (abs(leftError) < 15)
          leftI = ((leftI + leftError) * .7);
        else
          leftI = 0;
        leftD = ((leftError - leftLastError) * 1);
        rightP = (rightError * .95);
        if (abs(rightError) < 15)
          rightI = ((rightI + rightError) * .7);
        else
          rightI = 0;
        rightD = ((rightError - rightLastError) * 1);
      }
      else if (powerLevelMain() > 7.9 && powerLevelMain() < 8.2)
      { // if battery is 7.9 - 8.2 volts
        leftP = (leftError * 1);
        if (abs(leftError) < 15)
          leftI = ((leftI + leftError) * .8);
        else
          leftI = 0;
        leftD = ((leftError - leftLastError) * 1);
        rightP = (rightError * 1);
        if (abs(rightError) < 15)
          rightI = ((rightI + rightError) * .8);
        else
          rightI = 0;
        rightD = ((rightError - rightLastError) * 1);
      }
      else if (powerLevelMain() > 7.6 && powerLevelMain() < 7.9)
      { // if battery is 7.6 - 7.9 volts
        leftP = (leftError * 1);
        if (abs(leftError) < 15)
          leftI = ((leftI + leftError) * 1.2);
        else
          leftI = 0;
        leftD = ((leftError - leftLastError) * 1);
        rightP = (rightError * 1);
        if (abs(rightError) < 15)
          rightI = ((rightI + rightError) * 1.2);
        else
          rightI = 0;
        rightD = ((rightError - rightLastError) * 1);
      }
      else
      { // if battery is less than 7.6 volts
        leftP = (leftError * 1);
        if (abs(leftError) < 15)
          leftI = ((leftI + leftError) * 1.1);
        else
          leftI = 0;
        leftD = ((leftError - leftLastError) * 1);
        rightP = (rightError * 1);
        if (abs(rightError) < 15)
          rightI = ((rightI + rightError) * 1.1);
        else
          rightI = 0;
        rightD = ((rightError - rightLastError) * 1);
      }
      

      // store last error
      leftLastError = leftError;
      rightLastError = rightError;

      // calculate drive
      leftDrive = (leftP + leftI + leftD);
      rightDrive = (rightP + rightI + rightD);

      // if we are in debug mode, print error
      if (debugGlobal == true) {
        printf("lErr%d\n", leftError);
        printf("lDri%d\n", leftDrive);
        printf("rErr%d\n", rightError);
        printf("rDri%d\n", rightDrive);
      }

      // set motor to drive
      motorSet(driveLeft, leftDrive * -1);
      motorSet(driveRight, rightDrive * -1);
      motorSet(driveLeft2, leftDrive * -1);
      motorSet(driveRight2, rightDrive * -1);
      count = count + 1;
      delay(100);
    }
  }
}

/*-----------------------------------------------------------------------------*/
/*  gyro based turning                                                         */
/*-----------------------------------------------------------------------------*/
void turnTo(int targetDegrees, int waitTo) {
  gyroReset(gyro);
  int count = 0;
  float p;
  float i = 0;
  float d;
  int lastError = 0;
  int error = (targetDegrees - gyroGet(gyro));
  int drive;
  while(gyroGet(gyro) != targetDegrees) {
    if (count == (waitTo / 100) || (error == 0 && lastError == 0)) {
      if (debugGlobal == true) {
        printf("task completed\n");
      }
      motorSet(driveLeft, 0);
      motorSet(driveLeft2, 0);
      motorSet(driveRight, 0);
      motorSet(driveRight2, 0);
      return;
    } else {
      // calculate error
      error = (targetDegrees - gyroGet(gyro));

      // calculate pid based on power
      if (powerLevelMain() > 8.2) { // if battery is above 8.2 volts
        p = (error * 1);
        if (abs(error) < 500)
          i = ((i + error) * .45);
        else
          i = 0;
        d = ((error - lastError) * 3.5);
      }
      else if (powerLevelMain() > 7.9 && powerLevelMain() < 8.2) { // if battery is 7.9 - 8.2 volts
        p = (error * 1.2);
        if (abs(error) < 500)
          i = ((i + error) * .65);
        else
          i = 0;
        d = ((error - lastError) * 3.6);
      }
      else if (powerLevelMain() > 7.7 && powerLevelMain() < 7.9) { // if battery is 7.7 - 7.9 volts
        p = (error * 1.4);
        if (abs(error) < 500)
          i = ((i + error) * .77);
        else
          i = 0;
        d = ((error - lastError) * 3.5);
      }
      else { // if battery is less than 7.7 volts
        p = (error * 1.5);
        if (abs(error) < 500)
          i = ((i + error) * .80);
        else
          i = 0;
        d = ((error - lastError) * 3.5);
      }

      // store last error
      lastError = error;

      // calculate drive
      drive = (p + i + d);

      // if we are in debug mode, print error
      if (debugGlobal == true) {
        printf("error%d\n", error);
        printf("drive%d\n", drive);
        printf("gyro%d\n", gyroGet(gyro));
      }

      // set motor to drive
      motorSet(driveLeft, drive * 1);
      motorSet(driveRight, drive * -1);
      motorSet(driveLeft2, drive * 1);
      motorSet(driveRight2, drive * -1);
      count = count + 1;
      delay(100);
    }
  }
}

/*-----------------------------------------------------------------------------*/
/*  recording function for rerun                                               */
/*-----------------------------------------------------------------------------*/
void rerunRecord() {
  printf("driveTo(%d, %d, 100);\n", encoderGet(leftencoder), encoderGet(rightencoder));
  // wow this was difficult
}