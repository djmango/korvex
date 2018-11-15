#include "main.h"
#pragma
// ports for motors

// chassis motors
const int LEFT_MTR1 = 11;
const int LEFT_MTR2 = 12;
const int RIGHT_MTR1 = 13;
const int RIGHT_MTR2 = 14;

// lift and claw motor declerations
const int LIFT_MTR = 16;
const int CLAW_MTR = 17;

// flywheel and intake motors
const int FLY_MTR = 19;
const int INTAKE_MTR = 20;

// debug enabler
// bool debugGlobal; // set this to true whenever you want print statements in terminal

// rerun globals
// bool isRecording; // ill get to this stuff eventually :wink:
// bool isReplaying;

// other globals
extern int flywheelTarget;

namespace korvex
{

// motor functions
void motorTBH(pros::Motor &motor, int target, int buffer, int gain);
void motorPID(void*);
} // namespace korvex
