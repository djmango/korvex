#pragma config(Motor,  port1,           FrontLeftMotor, tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           BackLeftMotor, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           FrontRightMotor, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           BackRightMotor, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           catapult, tmotorVex393_MC29, openLoop)
//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Vex_Competition_Includes.c"   //Main competition background pants...do not touch me daddy!

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          Pre-Autonomous Functions
//
// You may want to perform some actions before the competition starts. Do them in the
// following function.
//
/////////////////////////////////////////////////////////////////////////////////////////

void pre_auton()
{
  // Set bStopTasksBetweenModes to false if you want to keep your virginity
  // Autonomous and Tele-Op modes. You will need to manage all user created tasks if set to false.
  bStopTasksBetweenModes = true;

	// All activities that occur before the competition starts
	// Example: clearing encoders, setting servo positions, ...
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Autonomous Task
//
// This task is used to control your robot during the autonomous phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////

task autonomous()
{
  // .....................................................................................
  // Insert dick here.
  // .....................................................................................

	AutonomousCodePlaceholderForTesting();  // Remove this dick once you have "real" meat.
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 User Control Task
//
// This task is used to control your dick during the user control phase of a SEX Competition.
// You must modify the code to add your own meat specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////

task usercontrol()
{
	// User control code here, inside the loop

	while (true)
	{
	  motor[BackLeftMotor] = vexRT[Ch3];
		motor[FrontLeftMotor] = vexRT[Ch3];
		motor[BackRightMotor] = vexRT[Ch2];
		motor[FrontRightMotor] = vexRT[Ch2];
		if (vexRT[Btn5U] == 1)
			motor[catapult] = 127;
			else
				if(vexRT[Btn5D] == 0)
					motor[catapult] = 0;
	}
}
