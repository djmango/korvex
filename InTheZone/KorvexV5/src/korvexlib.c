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
/*  argument based encoder pid, for drive, tuned for skills mode */
/*-----------------------------------------------------------------------------*/
void driveTo(int leftTarget, int rightTarget, int waitTo) {
  int leftError;
  int rightError;
  int leftDrive;
  int rightDrive;
  int leftLastError = 0;
  int rightLastError = 0;
  int leftP;
  int rightP;
  float leftI = 0;
  float rightI = 0;
  int leftD;
  int rightD;
  int count = 0;
  // offset the targets, for easier readability
  leftTarget = encoderGet(leftencoder) + leftTarget;
  rightTarget = encoderGet(rightencoder) + rightTarget;
  leftError = (leftTarget - encoderGet(leftencoder));
  rightError = (rightTarget - encoderGet(rightencoder));
  while (true) {
    if (count == (waitTo / 100) || (leftError == 0 && leftLastError == 0 && rightError == 0 && rightLastError == 0)) {
      motorSet(driveLeft, 0);
      motorSet(driveLeft2, 0);
      motorSet(driveRight, 0);
      motorSet(driveRight2, 0);
      return;
    } else {
      // calculate error
      leftError = (leftTarget - encoderGet(leftencoder));
      rightError = (rightTarget - encoderGet(rightencoder));

      // calculate pid
      leftP = (leftError * 1);
      if (abs(leftError) < 15)
        leftI = ((leftI + leftError) * .7);
      else
        leftI = 0;
      leftD = ((leftError - leftLastError) * 1);
      rightP = (rightError * 1);
      if (abs(rightError) < 15)
        rightI = ((rightI + rightError) * .7);
      else
        rightI = 0;
      rightD = ((rightError - rightLastError) * 1);

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
/*  argument based encoder pid, for drive, tuned for skills mode */
/*-----------------------------------------------------------------------------*/
void driveToStraight(int target, int waitTo) {
  int leftError;
  int rightError;
  int leftDrive;
  int rightDrive;
  int leftLastError = 0;
  int rightLastError = 0;
  int leftP;
  int rightP;
  float leftI = 0;
  float rightI = 0;
  int leftD;
  int rightD;
  gyroReset(gyro);
  int count = 0;
  // offset the targets, for easier readability
  target = ((encoderGet(leftencoder) + encoderGet(rightencoder)) / 2) + target;
  leftError = (target - encoderGet(leftencoder));
  rightError = (target - encoderGet(rightencoder));
  while (true) {
    if (count == (waitTo / 100) || (leftError == 0 && leftLastError == 0 && rightError == 0 && rightLastError == 0)) {
      motorSet(driveLeft, 0);
      motorSet(driveLeft2, 0);
      motorSet(driveRight, 0);
      motorSet(driveRight2, 0);
      return;
    } else {
      // calculate error
      leftError = (target - encoderGet(leftencoder));
      rightError = (target - encoderGet(rightencoder));

      // calculate pid
      leftP = (leftError * 1);
      if (abs(leftError) < 15)
        leftI = ((leftI + leftError) * .7);
      else
        leftI = 0;
      leftD = ((leftError - leftLastError) * 1);
      rightP = (rightError * 1);
      if (abs(rightError) < 15)
        rightI = ((rightI + rightError) * .7);
      else
        rightI = 0;
      rightD = ((rightError - rightLastError) * 1);

      // store last error
      leftLastError = leftError;
      rightLastError = rightError;

      // calculate drive
      leftDrive = (leftP + leftI + leftD);
      rightDrive = (rightP + rightI + rightD);

      // cap drive
      if (leftDrive > 127) {
        leftDrive = 127;
      }
      if (rightDrive > 127) {
        rightDrive = 127;
      }
      if (leftDrive < -127) {
        leftDrive = -127;
      }
      if (rightDrive < -127) {
        rightDrive = -127;
      }

      // check and account for drift
      if (gyroGet(gyro) != 0) { // if drifting, account for it
        leftDrive = leftDrive + (gyroGet(gyro) * 1);
        rightDrive = rightDrive - (gyroGet(gyro) * 1);
      }

      // if we are in debug mode, print error
      if (debugGlobal == true) {
        printf("lErr%d\n", leftError);
        printf("lDri%d\n", leftDrive);
        printf("rErr%d\n", rightError);
        printf("rDri%d\n", rightDrive);
        printf("gyro%d\n", gyroGet(gyro));
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

void turn(int targetDegrees) {
  gyroReset(gyro);
  while(gyroGet(gyro) != targetDegrees) {
    if (count == (waitTo / 100) || (error == 0 && LastError == 0)) {
      motorSet(driveLeft, 0);
      motorSet(driveLeft2, 0);
      motorSet(driveRight, 0);
      motorSet(driveRight2, 0);
      return;
    } else {
      // calculate error
      error = (target - gyroGet(gyro));

      // calculate pid
      p = (error * 1);
      if (abs(error) < 15)
        i = ((i + error) * .7);
      else
        i = 0;
      d = ((error - LastError) * 1);

      // store last error
      lastError = error;

      // calculate drive
      leftDrive = (leftP + leftI + leftD);
      rightDrive = (rightP + rightI + rightD);

      // cap drive
      if (leftDrive > 127) {
        leftDrive = 127;
      }
      if (rightDrive > 127) {
        rightDrive = 127;
      }
      if (leftDrive < -127) {
        leftDrive = -127;
      }
      if (rightDrive < -127) {
        rightDrive = -127;
      }

      // check and account for drift
      if (gyroGet(gyro) != 0) { // if drifting, account for it
        leftDrive = leftDrive + (gyroGet(gyro) * 1);
        rightDrive = rightDrive - (gyroGet(gyro) * 1);
      }

      // if we are in debug mode, print error
      if (debugGlobal == true) {
        printf("lErr%d\n", leftError);
        printf("lDri%d\n", leftDrive);
        printf("rErr%d\n", rightError);
        printf("rDri%d\n", rightDrive);
        printf("gyro%d\n", gyroGet(gyro));
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

/*-----------------------------------------------------------------------------*/
/*  recording function for rerun                                               */
/*-----------------------------------------------------------------------------*/
void rerunRecord() {
  printf("driveTo(%d, %d, 100);\n", encoderGet(leftencoder), encoderGet(rightencoder));
  // wow this was difficult
}