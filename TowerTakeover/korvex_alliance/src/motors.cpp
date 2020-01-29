#include "main.h"
#include "korvexlib.h"
#include "okapi/api.hpp"

using namespace okapi;

pros::Motor liftMotor(LIFT_MTR, pros::E_MOTOR_GEARSET_36, false);
pros::Motor trayMotor(TRAY_MTR, pros::E_MOTOR_GEARSET_36, false);
okapi::MotorGroup intakeMotors({-INTAKE_MTR1, INTAKE_MTR2});


// controller
Controller masterController;
ControllerButton liftUp(ControllerDigital::R1);
ControllerButton liftDown(ControllerDigital::R2);
ControllerButton intakeIn(ControllerDigital::L1);
ControllerButton intakeOut(ControllerDigital::L2);
ControllerButton intakeShift(ControllerDigital::right);
ControllerButton shift(ControllerDigital::Y);

// other
pros::Imu imu(IMU_PORT);

/*
a fun hack to force the compiler to spit out an error that shows you the type of something (fun for working with auto)
template <typename T> struct Foo;
Foo<decltype(chassis)> bar;
thx lachlan ;D
*/