/** @file auto.c
 * @brief File for autonomous code
 *
 * This file should contain the user autonomous() function and any functions related to it.
 *
 * Copyright (c) 2011-2014, Purdue University ACM SIG BOTS.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Purdue University ACM SIG BOTS nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PURDUE UNIVERSITY ACM SIG BOTS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Purdue Robotics OS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

/*
 * Runs the user autonomous code. This function will be started in its own task with the default
 * priority and stack size whenever the robot is enabled via the Field Management System or the
 * VEX Competition Switch in the autonomous mode. If the robot is disabled or communications is
 * lost, the autonomous task will be stopped by the kernel. Re-enabling the robot will restart
 * the task, not re-start it from where it left off.
 *
 * Code running in the autonomous task cannot access information from the VEX Joystick. However,
 * the autonomous function can be invoked from another task if a VEX Competition Switch is not
 * available, and it can access joystick information if called in this way.
 *
 * The autonomous task may exit, unlike operatorControl() which should never exit. If it does
 * so, the robot will await a switch to another mode or disable/enable cycle.
 */

//FUNCTIONS: Convert chasis rotations to inches and vise versa for easy autonomous coding. Do this by having functions which before being
//called are given a value (inches) to move or rotate.
int inchesmoved = 0;
int inchestomove = 0;
int rightinches = 0;
int leftinches = 0;
int nextstep = 0;
int rightinchestomove = 0;
int leftinchestomove = 0;


void autonomous() {
	leftencoder = 0;
	rightencoder = 0;
	inchestomove = 26;
	nextstep = 1;
	motorSet(clawright, 80);
	motorSet(clawleft, 80);
	while (nextstep == 1)
	{
		moveforward();
	}
	inchestomove = 0;
	inchreset();
	rightinchestomove = 6;
	while (nextstep == 2)
	{
		turnright();
	}
	motorSet(clawleft, 0);
	motorSet(clawright, 0);
	rightinchestomove = 0;
	inchreset();
	inchestomove = 28;
	while (nextstep == 3)
	{
		moveforward();
	}
	inchestomove = 0;
	inchreset();
	leftinchestomove = 4.7;
	motorSet(clawleft, 127);
	motorSet(clawright, 127);
	wait1Msec(600);
	motorSet(clawleft, 80);
	motorSet(clawright, 80);
	wait1Msec(400);
	motorSet(lift1, 127);
	motorSet(lift2, 127);
	motorSet(lift3, 127);
	wait1Msec(100);
	while (nextstep == 4)
	{
		turnleft();
	}
	leftinchestomove = 0;
	inchreset();
	inchestomove = 24;
	while (nextstep == 5)
	{
		moveforward();
	}
	inchestomove = 0;
	inchreset();
	inchestomove = -20;
	motorSet(clawleft, -127);
	motorSet(clawright, -127);
	wait1Msec(500);
	motorSet(clawleft, 80);
	motorSet(clawright, 80);
	while (nextstep == 6)
	{
		movebackward();
	}
	motorSet(clawleft, 0);
	motorSet(clawright, 0);
	motorSet(lift1, 0);
	motorSet(lift2, 0);
	motorSet(lift3 = 0);
	while (nextstep == 7)
	{
		movebackward();
	}
}
