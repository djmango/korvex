#include "main.h"
#include "korvexlib.h"

// motors
pros::Motor chassisLeft1(LEFT_MTR1, pros::E_MOTOR_GEARSET_18, false);
pros::Motor chassisLeft2(LEFT_MTR2, pros::E_MOTOR_GEARSET_18, false);
pros::Motor chassisRight1(RIGHT_MTR1, pros::E_MOTOR_GEARSET_18, true);
pros::Motor chassisRight2(RIGHT_MTR2, pros::E_MOTOR_GEARSET_18, true);

pros::Motor liftMotor(LIFT_MTR, pros::E_MOTOR_GEARSET_18, false);
pros::Motor trayMotor(TRAY_MTR, pros::E_MOTOR_GEARSET_18, false);
pros::Motor intakeMotor1(INTAKE_MTR1, pros::E_MOTOR_GEARSET_18, false);
pros::Motor intakeMotor2(INTAKE_MTR2, pros::E_MOTOR_GEARSET_18, false);

// other
pros::Controller controller(pros::E_CONTROLLER_MASTER);

/*
a fun hack to force the compiler to spit out an error that shows you the type of something (fun for working with auto)
template <typename T> struct Foo;
Foo<decltype(chassis)> bar;
thx lachlan ;D
*/