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
//Documentation - I wil lbe taking input and storing it in an array, one for each input.
//For example, J1C1 would be Joystick 1, adding C selects channel, and if there is a button B selects button, so , and its values updated every 50 msec.
//Global variables
int cache[6][600];
//int J1C2[600];
//int J1C3[600];
//Initialize joystick 1 buttons - side note not enough memory
/*int J1C5BU[600];
int J1C5BD[600];
int J1C6BU[600];
int J1C6BD[600];
int J1C7BU[600];
int J1C7BL[600];
int J1C7BD[600];
int J1C7BR[600];
int J1C8BU[600];
int J1C8BL[600];
int J1C8BD[600];
int J1C8BR[600];
//Initialize joystick 2
int J2C7BU[600];
int J2C7BL[600];
int J2C7BD[600];
int J2C7BR[600];
int J2C8BU[600];
int J2C8BL[600];
int J2C8BD[600];
int J2C8BR[600];*/
int DriveTimer = 0; //Timer - update 1 time per driver cycle, every 25 msec
int RecSwitch = 1; // Switch between playback and record
int rightclawsensor;
int leftclawsensor;
int rightclawpower;
int leftclawpower;
int leftencodervalue;
int rightencodervalue;
bool clawsynced = false;
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
void setCache() { //Paste output here for cache
}
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
       motorSet(8, -127);
       motorSet(9, 127);
     }
     if (joystickGetDigital(1, 5, JOY_DOWN) == 1){
       motorSet(8, 127);
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
 void recordDrive() { // Record driver input while allowing driver control
   delay(1);
	 cache[1][DriveTimer] = joystickGetAnalog(1, 2);// Get analog value of vertical axis of right stick, joystick 1
	 cache[2][DriveTimer] = joystickGetAnalog(1, 4); // Get analog value of vertical axis of left stick, joystick 1
	 /*(J1C5BU[DriveTimer] = joystickGetDigital(1,5,JOY_UP); // Get boolean value of upper right bumper
	 J1C5BD[DriveTimer] = joystickGetDigital(1,5,JOY_DOWN); // Get boolean value of lower right bumper
   J1C6BU[DriveTimer] = joystickGetDigital(1,6,JOY_UP); // Get boolean value of upper left bumper
	 J1C6BD[DriveTimer] = joystickGetDigital(1,6,JOY_DOWN); // Get boolean value of lower left bumper*/
 }
 void playbackDrive() { // Set motor to array values
   delay(1);
   motorSet(2, -cache[1][DriveTimer]);
   motorSet(3, -cache[1][DriveTimer]);
   motorSet(4, cache[2][DriveTimer]);
   motorSet(5, cache[2][DriveTimer]);
 }
 void setMode() { // Check buttons to set mode
   delay(1);
   if (joystickGetDigital(1,7,JOY_DOWN) == 1){// If you push the lower button, turn on recording
     RecSwitch = 1;
     DriveTimer = 0;
   }
   if (joystickGetDigital(1,7,JOY_RIGHT) == 1){// If you push the right button, turn on normal drive
     RecSwitch = 2;
     DriveTimer = 0;
   }
   if (joystickGetDigital(1,7,JOY_LEFT) == 1){// If you push the left button, turn on playback from cache
     RecSwitch = 3;
     DriveTimer = 0;
     setCache();
   }
   if (joystickGetDigital(1,7,JOY_UP)){ // If you push the upper button, turn off recording NOT USABLE
     RecSwitch = 0;
   }
   if (RecSwitch == 1) { // Begin Record function
     recordDrive();
     updateDrive();
   } // End Record function
   if (RecSwitch == 2) { // Begin normal drive function
     updateDrive();
   }
   if (RecSwitch == 3) { // Begin normal drive function
     playbackDrive();
   }
   if (RecSwitch == 0) { // Begin playback function
     playbackDrive();
   }
 }

void operatorControl() { //Motor documentation, 1-2 left joystick, 3-4 right joystick, Left Claw 8 Right Claw 9
	while (1) {
    setMode();
    clawSync();
    updateSensors();
    //printf("Value is %d, time is %d \n", J1C2[DriveTimer], DriveTimer);
    //printf("J1C2[%d] = %d \n", DriveTimer, J1C2[DriveTimer]); //Print statements formatted so that copy-paste will work
    //printf("J1C3[%d] = %d \n", DriveTimer, J1C3[DriveTimer]);
    printf("left value %d \n", leftencodervalue);
    printf("right value %d \n", rightencodervalue);
    DriveTimer = DriveTimer + 1; // Record time
		delay(50); // Wait for refresh
	}
}
