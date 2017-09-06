/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any
 * functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

/*
 * Runs the user operator control code. This function will be started in its own
 * task with the
 * default priority and stack size whenever the robot is enabled via the Field
 * Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is
 * disabled or
 * communications is lost, the operator control task will be stopped by the
 * kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the
 * VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX
 * Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX
 * Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is
 * available and
 * the scheduler is operational. However, proper use of delay() or
 * taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating
 * LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop,
 * even if empty.
 */
// all variables
int isFineControl = false;
int fineControl = 1;
int count = 0;
int rightrpm = 0;
int leftrpm = 0;

void updateDrive (int chassisControlLeft, int chassisControlRight, int liftControl, int moboLiftBtn, int moboTiltBtn, int clawBtn, int chainbarBtn, int fineBtn) {
  // Chassis control
  motorSet(2, (joystickGetAnalog(1,2) * fineControl));
  motorSet(3, (joystickGetAnalog(1,3) * fineControl));
  //lift control
  motorSet(3, (joystickGetAnalog(2,3)));
  motorSet(3, (joystickGetAnalog(2,3)));

  // mobo lift control
  if (joystickGetDigital(1, 5, JOY_UP) == 1) {
    motorSet(4, 127);
  }
  if (joystickGetDigital(1, 5, JOY_DOWN) == 1) {
    motorSet(4, -127);
  }
  if (joystickGetDigital(1, 5, JOY_DOWN) == 0 &&
      joystickGetDigital(1, 5, JOY_UP) == 0) {
    motorSet(4, 0);
  }
  //mobo tilt control
  if (joystickGetDigital(1, 6, JOY_UP) == 1) {
    motorSet(5, 127);
  }
  if (joystickGetDigital(1, 6, JOY_DOWN) == 1) {
    motorSet(5, -127);
  }
  if (joystickGetDigital(1, 6, JOY_DOWN) == 0 &&
      joystickGetDigital(1, 6, JOY_UP) == 0) {
    motorSet(5, 0);
  }
  // chain bar control
  if (joystickGetDigital(1, 8, JOY_UP) == 1) {
    // move up
    motorSet(8, 127);
  }
  if (joystickGetDigital(1, 8, JOY_DOWN) == 1) {
    // move down
    motorSet(8, -127);
  }
  if (joystickGetDigital(1, 7, JOY_DOWN) == 0 &&
      joystickGetDigital(1, 7, JOY_UP) == 0) {
    // dont move
    motorSet(8, 0);
  }
  // claw control
  if (joystickGetDigital(1, 7, JOY_UP) == 1) {
    // move up
    motorSet(8, 127);
  }
  if (joystickGetDigital(1, 7, JOY_DOWN) == 1) {
    // move down
    motorSet(8, -127);
  }
  if (joystickGetDigital(1, 7, JOY_DOWN) == 0 &&
      joystickGetDigital(1, 7, JOY_UP) == 0) {
    // dont move
    motorSet(8, 0);
  }

  // fine control toggle
  if (joystickGetDigital(1, 8, JOY_DOWN) == 1 &&
      isFineControl == false) { // toggle it on
    isFineControl = true;
    fineControl = .5;
  }
  if (joystickGetDigital(1, 8, JOY_UP) == 1 &&
      isFineControl == true) { // toggle it off
    isFineControl = false;
    fineControl = 1;
  }
}

/*port map current
port 1 = **scrubbed**
port 2 = drive left side
port 3 = drive right side //TODO: update for accuracy
port 4 = mobile goal lift 1
port 5 = mobile goal lift 2
port 6 = mobile goal tilt
port 7 = dr4b left
port 8 = dr4b right
port 9 = claw
port 10 = **scrubbed**
*/

/*port map DESIRED
port 1 = **scrubbed**
port 2 = drive left side (y-cable to power expander)
port 3 = drive right side (y-cable to power expander)
port 4 = mobile goal lift (y-cable)
port 5 = mobile goal tilt (direct)
port 6 = dr4b left (direct)
port 7 = dr4b right (direct)
port 8 = chain bar (y-cable)
port 9 = claw (direct)
port 10 = **scrubbed**
*/

void operatorControl() {
  //TaskHandle driveTaskHandle = taskRunLoop(updateDrive(), 50);
  while (true) {
		//chassisControlLeft, chassisControlRight, liftControl, moboLiftBtn, moboTiltBtn, clawBtn, chainbarBtn, fineBtn
		updateDrive(joystickGetAnalog(1,2), joystickGetAnalog(1,3), joystickGetAnalog(2,2))
    delay(20);
  }
  //taskDelete(driveTaskHandle);
}
