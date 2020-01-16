#include "main.h"
#include "korvexlib.h"
#include "okapi/api.hpp"

using namespace okapi;

pros::Motor liftMotor(LIFT_MTR, pros::E_MOTOR_GEARSET_36, false);
pros::Motor trayMotor(TRAY_MTR, pros::E_MOTOR_GEARSET_36, false);
pros::Motor intakeMotor1(INTAKE_MTR1, pros::E_MOTOR_GEARSET_18, true);
pros::Motor intakeMotor2(INTAKE_MTR2, pros::E_MOTOR_GEARSET_18, false);

// controller
Controller masterController;
ControllerButton bumperRU(ControllerDigital::R1);
ControllerButton bumperRD(ControllerDigital::R2);
ControllerButton bumperLU(ControllerDigital::L1);
ControllerButton bumperLD(ControllerDigital::L2);
ControllerButton shift(ControllerDigital::Y);
ControllerButton liftReset(ControllerDigital::X);

// other
pros::Imu imu(IMU_PORT);

/*
a fun hack to force the compiler to spit out an error that shows you the type of something (fun for working with auto)
template <typename T> struct Foo;
Foo<decltype(chassis)> bar;
thx lachlan ;D
*/