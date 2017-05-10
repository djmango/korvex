#pragma config(Sensor, dgtl1,  rightencoder,   sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  leftencoder,    sensorQuadEncoder)
#pragma config(Motor,  port2,           righttop,      tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           rightbottom,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           lift1,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           lift2,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           lefttop,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           leftbottom,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           claw,          tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           hang,          tmotorVex393_MC29, openLoop)
#pragma DebuggerWindows("debugStream")
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

task main()
{
	SensorValue[rightencoder] = 0;
	SensorValue[leftencoder] = 0;
	int meme;
	int meme2;
	while(1)
	{
		meme = SensorValue[rightencoder];
		meme2 = SensorValue[leftencoder];
		wait1Msec(500);
		writeDebugStream("%f is the current value of right", meme );
    writeDebugStream("%f is the current value of left", meme2 );
	}
	while(1==0)
	{
		if(SensorValue[leftencoder] < 990)
		{
			motor[rightbottom] = -127;
			motor[righttop] = -127;
			motor[lefttop] = 127;
			motor[leftbottom] = 127;
		}
		else
		{
			motor[rightbottom] = 0;
			motor[righttop] = 0;
			motor[lefttop] = 0;
			motor[leftbottom] = 0;
		}
	}
}