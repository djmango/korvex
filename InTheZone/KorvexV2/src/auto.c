/** @file auto.c
 * @brief File for autonomous code
 *
 * This file should contain the user autonomous() function and any functions
 * related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

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
 * so, the robot will await a switch to another mode or disable/enable cycle.
 */
/* port map DESIRED
port 1 = **scrubbed**
port 2 = drive left side (y-cable to power expander)
port 3 = drive right side (y-cable to power expander)
port 4 = mobile goal lift (y-cable)
port 5 = free
port 6 = dr4b left (y-cable)
port 7 = dr4b right (y-cable)
port 8 = chain bar (y-cable)
port 9 = claw (direct)
port 10 = **scrubbed**
*/

// defs
// motor defs
#define driveLeft 2
#define driveRight 3
#define mobileGoal 4
#define dr4bLeft 6
#define dr4bRight 7
#define chainBar 8
#define claw 9

// pid defs
#define PID_DRIVE_MAX 127
#define PID_DRIVE_MIN (-127)
#define PID_INTEGRAL_LIMIT 50

// vars
float pid_Kp = 2.0;
float pid_Ki = 0.04;
float pid_Kd = 0.05;
bool encoderPIDenabled = false;
int encoderValueLeft;
int encoderTargetLeft;
int motorLeft;

int encoderValueRight;
int encoderTargetRight;
int motorRight;

// leftDriveValue = encoderGet(leftencoder);
// rightDriveValue = encoderGet(rightencoder);
void encoderReset() {
  //reset all encoders
  encoderReset (leftencoder);
  encoderReset (rightencoder);
}
void driveLeftPID() {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  float pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (encoderPIDenabled = true) {
    float encoderCalcValue = (encoderValueLeft * 31.9024 / 360);
    // convert to a universal unit, in this case centimeters because science
    // 31.9024 is avg circumfrence of omniwheel, 360 due to how the optical
    // shaft encoder counts
    // sensor scale becuase why not
    // encoder target becuase i kinda need that
    // glenn IS the code
    // derek IS the coder

    // mathy stuff

    // calculate error
    pidError = encoderCalcValue - encoderTargetLeft;

    // integral - if Ki is not 0
    if (pid_Ki != 0) {
      // If we are inside controlable window then integrate the error
      if (abs(pidError) < PID_INTEGRAL_LIMIT)
        pidIntegral = pidIntegral + pidError;
      else
        pidIntegral = 0;
    } else
      pidIntegral = 0;

    // calculate the derivative
    pidDerivative = pidError - pidLastError;
    pidLastError = pidError;

    // calculate drive
    pidDrive =
        (pid_Kp * pidError) + (pid_Ki * pidIntegral) + (pid_Kd * pidDerivative);

    // limit drive
    if (pidDrive > PID_DRIVE_MAX)
      pidDrive = PID_DRIVE_MAX;
    if (pidDrive < PID_DRIVE_MIN)
      pidDrive = PID_DRIVE_MIN;

    // send to motor
    motorSet(motorLeft, pidDrive);

    // Run at 50Hz
  }
}
void driveRightPID() {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  float pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (encoderPIDenabled = true) {
    float encoderCalcValue = (encoderValueRight * 31.9024 / 360);
    // convert to a universal unit, in this case centimeters because science
    // 31.9024 is avg circumfrence of omniwheel, 360 due to how the optical
    // shaft encoder counts
    // sensor scale becuase why not
    // encoder target becuase i kinda need that
    // glenn IS the code
    // derek IS the coder

    // mathy stuff

    // calculate error
    pidError = encoderCalcValue - encoderTargetRight;

    // integral - if Ki is not 0
    if (pid_Ki != 0) {
      // If we are inside controlable window then integrate the error
      if (abs(pidError) < PID_INTEGRAL_LIMIT)
        pidIntegral = pidIntegral + pidError;
      else
        pidIntegral = 0;
    } else
      pidIntegral = 0;

    // calculate the derivative
    pidDerivative = pidError - pidLastError;
    pidLastError = pidError;

    // calculate drive
    pidDrive =
        (pid_Kp * pidError) + (pid_Ki * pidIntegral) + (pid_Kd * pidDerivative);

    // limit drive
    if (pidDrive > PID_DRIVE_MAX)
      pidDrive = PID_DRIVE_MAX;
    if (pidDrive < PID_DRIVE_MIN)
      pidDrive = PID_DRIVE_MIN;

    // send to motor
    motorSet(motorRight, pidDrive);

    // Run at 50Hz
  }
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
  TaskHandle driveTaskHandleLeft = taskRunLoop(driveLeftPID, 25);
  TaskHandle driveTaskHandleRight = taskRunLoop(driveRightPID, 25);

  //init all the important stuff, it shouldnt change throughout the code
  encoderPIDenabled = true; // when you do not want the drive powered, encoderPIDenabled must be false
  encoderValueLeft = encoderGet(leftencoder); //this gives the read value of the optical shaft encoder
  motorLeft = driveLeft; //the motor the pid is calculating for
  encoderValueRight = encoderGet(rightencoder);
  motorRight = driveRight;
  //testing the target, it stays at target until reset
  encoderTargetLeft = 12; //target, in cm
  encoderTargetRight = 12;
  //this will stay at 12 cm, until a reset
  encoderReset();
  encoderTargetLeft = -6;
  encoderTargetRight = 6;
  //should turn counter-clockwise? not sure, gota test
  taskDelete(driveTaskHandleLeft);
  taskDelete(driveTaskHandleRight);
}
