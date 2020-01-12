#include "main.h"
#include "okapi/api.hpp"

#pragma
// ports for motors

// chassis motors
const int LEFT_MTR1 = 9;
const int LEFT_MTR2 = 10;
const int RIGHT_MTR1 = 2;
const int RIGHT_MTR2 = 1;

// lift and tray motors
const int TRAY_MTR = 7;
const int LIFT_MTR = 13;

// flywheel and intake motors
const int INTAKE_MTR1 = 11;
const int INTAKE_MTR2 = 20;

// sensors
const int IMU_PORT = 6;

// auton
extern int autonSelection;

// globals

// motor declerations
extern pros::Motor chassisLeftFront;
extern pros::Motor chassisLeftBack;
extern pros::Motor chassisRightFront;
extern pros::Motor chassisRightBack;
extern pros::Motor liftMotor;
extern pros::Motor trayMotor;
extern pros::Motor intakeMotor1;
extern pros::Motor intakeMotor2;

// other declerations
extern okapi::Controller masterController;
extern pros::Imu imu;

extern okapi::ControllerButton bumperLU;
extern okapi::ControllerButton bumperLD;
extern okapi::ControllerButton bumperRU;
extern okapi::ControllerButton bumperRD;
extern okapi::ControllerButton shift;