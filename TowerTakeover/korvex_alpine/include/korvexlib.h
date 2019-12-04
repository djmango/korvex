#include "main.h"

#pragma
// ports for motors

// chassis motors
const int LEFT_MTR1 = 1;
const int LEFT_MTR2 = 2;
const int RIGHT_MTR1 = 3;
const int RIGHT_MTR2 = 4;

// lift and tray motor declerations
const int LIFT_MTR = 7;
const int TRAY_MTR = 8;

// flywheel and intake motors
const int INTAKE_MTR1 = 9;
const int INTAKE_MTR2 = 10;

// auton
extern int autonSelection;
extern bool autonPark;

// globals

// motor declerations
extern pros::Motor chassisLeft1;
extern pros::Motor chassisLeft2;
extern pros::Motor chassisRight1;
extern pros::Motor chassisRight2;
extern pros::Motor liftMotor;
extern pros::Motor trayMotor;
extern pros::Motor intakeMotor1;
extern pros::Motor intakeMotor2;

// other declerations
extern pros::Controller controller;