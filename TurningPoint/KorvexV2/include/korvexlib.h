#include "main.h"
#include "okapi/api.hpp"

#pragma
// ports for motors

// chassis motors
const int LEFT_MTR1 = 1;
const int LEFT_MTR2 = 2;
const int RIGHT_MTR1 = 3;
const int RIGHT_MTR2 = 4;

// lift and claw motor declerations
const int LIFT_MTR = 7;
const int CAPFLIP_MTR = 8;

// flywheel and intake motors
const int FLY_MTR = 9;
const int INTAKE_MTR = 10;

// sensors
const int TRIGGER_BL = 2;
const int TRIGGER_BR = 3;
const int TRIGGER_TL = 4;
const int TRIGGER_TR = 5;

// auton
extern int autonSelection;

// motor declerations
extern pros::Motor chassisLeft1;
extern pros::Motor chassisLeft2;
extern pros::Motor chassisRight1;
extern pros::Motor chassisRight2;
extern pros::Motor liftMotor;
extern pros::Motor flywheelMotor;
extern pros::Motor capflipMotor;
extern pros::Motor intakeMotor;
extern pros::Motor descoreMotor;

// other declerations
extern pros::Controller controllerPros;
extern okapi::Controller controller;
extern pros::ADIDigitalIn triggerBL;
extern pros::ADIDigitalIn triggerBR;
extern pros::ADIDigitalIn triggerTL;
extern pros::ADIDigitalIn triggerTR;
extern okapi::ChassisControllerPID chassis;
extern okapi::IterativePosPIDController liftControllerPID;

// debug enabler
// bool debugGlobal; // set this to true whenever you want print statements in terminal

// rerun globals
// bool isRecording; // ill get to this stuff eventually :wink:
// bool isReplaying;

// other globals
// extern int flywheelTarget;

namespace korvex
{

// motor functions
void motorTBH(pros::Motor &motor, int target, int buffer, int gain);
void motorPID(void*);
} // namespace korvex
