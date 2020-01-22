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

// globals
extern int autonSelection;

// motor declerations
extern pros::Motor liftMotor;
extern pros::Motor trayMotor;
extern okapi::MotorGroup intakeMotors;

// other declerations
extern okapi::Controller masterController;
extern pros::Imu imu;

extern okapi::ControllerButton intakeIn;
extern okapi::ControllerButton intakeOut;
extern okapi::ControllerButton intakeShift;
extern okapi::ControllerButton shift;
extern okapi::ControllerButton liftUp;
extern okapi::ControllerButton liftDown;
