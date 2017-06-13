/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */
 int DriveTimer = 0; //Timer - update 1 time per driver cycle, every 25 msec
 int RecSwitch = 1; // Switch between playback and record
 int rightclawsensor;
 int leftclawsensor;
 int rightclawpower;
 int leftclawpower;
 int leftencodervalue;
 int rightencodervalue;
 bool clawsynced = false;
 void updateSensors() { //Update all sensor values
   leftencodervalue = encoderGet(encoderInit(1,2, false));
   rightencodervalue = encoderGet(encoderInit(3,4, false));
   //printf("left value %d \n", leftencodervalue);
   //printf("right value %d \n", rightencodervalue);
 }
 void clawSync(){ //Sync claw
   if(rightencodervalue < leftclawsensor && clawsynced == false){
   leftclawpower = 20;
   rightclawpower = -20;
   }
   if(rightclawsensor > leftclawsensor && clawsynced == false){
     leftclawpower = -20;
     rightclawpower = 20;
   }
 }
 void updateDrive() { //Update robot to joystick control
    //Chasis control
 	 motorSet(2, -joystickGetAnalog(1,2));
 	 motorSet(3, -joystickGetAnalog(1,2));
 	 motorSet(4, joystickGetAnalog(1,3));
 	 motorSet(5, joystickGetAnalog(1,3));
   motorSet(6, joystickGetAnalog(2,3));
   motorSet(7, joystickGetAnalog(2,3));
   motorSet(8, joystickGetAnalog(1,2));
   motorSet(9, joystickGetAnalog(1,2));
    //Lift control
    if (joystickGetDigital(1, 6, JOY_UP) == 1){
      motorSet(6, 127);
      motorSet(7, 127);
    }
    if (joystickGetDigital(1, 6, JOY_DOWN) == 1){
      motorSet(6, -127);
      motorSet(7, -127);
    }
    if (joystickGetDigital(1, 6, JOY_DOWN) == 0 && joystickGetDigital(1, 6, JOY_UP) == 0){
      motorSet(6, 0);
      motorSet(7, 0);
    }
    //Claw control
    if(joystickGetDigital(1, 5, JOY_UP) == 1 || joystickGetDigital(1, 5, JOY_DOWN) == 1){
      if (joystickGetDigital(1, 5, JOY_UP) == 1){
        motorSet(8, 127);
        motorSet(9, 127);
      }
      if (joystickGetDigital(1, 5, JOY_DOWN) == 1){
        motorSet(8, -127);
        motorSet(9, -127);
      }
      clawsynced = false;
    }
    else{
      motorSet(8, leftclawpower); //left side is 8
      motorSet(9, rightclawpower);
    }
    /*
    if (joystickGetDigital(1, 5, JOY_UP) == 1){
      motorSet(8, -127);
      motorSet(9, 127);
    }
    if (joystickGetDigital(1, 5, JOY_DOWN) == 1){
      motorSet(8, 127);
      motorSet(9, -127);
    }*/
    if (joystickGetDigital(1, 5, JOY_DOWN) == 0 && joystickGetDigital(1, 5, JOY_UP) == 0){
      motorSet(8, 0);
      motorSet(9, 0);
    }
  }
void operatorControl() {
	while (1) {
		//updateDrive();
		//updateSensors();
		//clawSync();
    motorSet(2, -joystickGetAnalog(1,2));
    motorSet(3, -joystickGetAnalog(1,2));
    motorSet(4, joystickGetAnalog(1,3));
    motorSet(5, joystickGetAnalog(1,3));
    motorSet(6, joystickGetAnalog(2,2));
    motorSet(7, joystickGetAnalog(2,2));
    if(joystickGetDigital(2, 5, JOY_UP) == 1 || joystickGetDigital(2, 6, JOY_UP) == 1 || joystickGetDigital(2, 5, JOY_DOWN) == 1 ||joystickGetDigital(2, 6, JOY_DOWN) == 1){
      if(joystickGetDigital(2, 5, JOY_UP) == 1){
        motorSet(8, 60);
      }
      if(joystickGetDigital(2, 5, JOY_DOWN) == 1){
        motorSet(8, -60);
      }
      if(joystickGetDigital(2, 6, JOY_UP) == 1){
        motorSet(9, 60);
      }
      if(joystickGetDigital(2, 6, JOY_DOWN) == 1){
        motorSet(9, -60);
      }
    }
    else{
      motorSet(8, joystickGetAnalog(2,3));
      motorSet(9, joystickGetAnalog(2,3));
    }
		delay(20);
	}
}
