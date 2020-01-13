#include "main.h"
#include "korvexlib.h"
#include "okapi/api.hpp"

// motors
pros::Motor chassisLeftFront(LEFT_MTR1, pros::E_MOTOR_GEARSET_18, true);
pros::Motor chassisLeftBack(LEFT_MTR2, pros::E_MOTOR_GEARSET_18, true);
pros::Motor chassisRightFront(RIGHT_MTR1, pros::E_MOTOR_GEARSET_18, false);
pros::Motor chassisRightBack(RIGHT_MTR2, pros::E_MOTOR_GEARSET_18, false);

pros::Motor liftMotor(LIFT_MTR, pros::E_MOTOR_GEARSET_36, false);
pros::Motor trayMotor(TRAY_MTR, pros::E_MOTOR_GEARSET_36, false);
pros::Motor intakeMotor1(INTAKE_MTR1, pros::E_MOTOR_GEARSET_18, true);
pros::Motor intakeMotor2(INTAKE_MTR2, pros::E_MOTOR_GEARSET_18, false);

// controller
okapi::Controller masterController;
okapi::ControllerButton bumperRU(okapi::ControllerDigital::R1);
okapi::ControllerButton bumperRD(okapi::ControllerDigital::R2);
okapi::ControllerButton bumperLU(okapi::ControllerDigital::L1);
okapi::ControllerButton bumperLD(okapi::ControllerDigital::L2);
okapi::ControllerButton shift(okapi::ControllerDigital::Y);
okapi::ControllerButton liftReset(okapi::ControllerDigital::X);

// other
pros::Imu imu(IMU_PORT);

/*
a fun hack to force the compiler to spit out an error that shows you the type of something (fun for working with auto)
template <typename T> struct Foo;
Foo<decltype(chassis)> bar;
thx lachlan ;D
*/