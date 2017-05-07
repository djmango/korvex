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
void operatorControl(int clawrightvalue, int clawleftvalue) {
//Main loop
	while(true){
		//make coding great again - an initiative by sulaiman ghori
		//Claw lock
		if(analogRead(1) < analogRead(2)){
			clawleftvalue = clawleftvalue - 1;
			clawrightvalue = clawrightvalue + 1;
		}
		if(analogRead(1) > analogRead(2)){
			clawleftvalue = clawleftvalue + 1;
			clawrightvalue = clawrightvalue - 1;
		}
		//Define joysticks
		int Ch2 = joystickGetAnalog(1,2);
		int Ch3 = joystickGetAnalog(1,3);
		int Ch2Xmtr2 = joystickGetAnalog(2,2);
		int Ch3Xmtr2 = joystickGetAnalog(2,3);
		//Right side of the robot is controlled by the right joystick, Y-axis
		motorSet(right, -Ch2);
		//Left side of the robot is controlled by the left joystick, Y-axis
		motorSet(left, -Ch3*-1);
		//Lift motors are on 3 ports, last 2 inverted, controled by right joystick, seccond controller, Y-axis
		motorSet(lift1, Ch3Xmtr2*-1);
		motorSet(lift2, Ch2Xmtr2*1);
		motorSet(lift3, Ch2Xmtr2 *1);
		//Claw control, checks if controler not in use then locks if it is not
		if(Ch3Xmtr2 > 10){
			motorSet(clawright, Ch3Xmtr2);
			motorSet(clawleft, Ch3Xmtr2*-1);
		}
		else{
			motorSet(clawright, clawrightvalue);
			motorSet(clawleft, clawleftvalue*-1);
		}

//LCD Debug code
		lcdPrint(uart2, 1, "Right claw value is %d", (analogRead(1)));
	}
	while (1) {
		delay(20);
	}
}
