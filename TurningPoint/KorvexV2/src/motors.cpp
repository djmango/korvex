#include "main.h"
#include "okapi/api.hpp"
#include "korvexlib.h"

// motors
pros::Motor chassisLeft1(LEFT_MTR1, pros::E_MOTOR_GEARSET_18, false);
pros::Motor chassisLeft2(LEFT_MTR2, pros::E_MOTOR_GEARSET_18, false);
pros::Motor chassisRight1(RIGHT_MTR1, pros::E_MOTOR_GEARSET_18, true);
pros::Motor chassisRight2(RIGHT_MTR2, pros::E_MOTOR_GEARSET_18, true);

pros::Motor flywheelMotor(FLY_MTR, pros::E_MOTOR_GEARSET_06, true);
pros::Motor intakeMotor(INTAKE_MTR, pros::E_MOTOR_GEARSET_18, false);
pros::Motor capflipMotor(CAPFLIP_MTR, pros::E_MOTOR_GEARSET_18, false);
pros::Motor descoreMotor(LIFT_MTR, pros::E_MOTOR_GEARSET_18, false);

// intake triggers
pros::ADIDigitalIn triggerBL(TRIGGER_BL);
pros::ADIDigitalIn triggerBR(TRIGGER_BR);
pros::ADIDigitalIn triggerTL(TRIGGER_TL);
pros::ADIDigitalIn triggerTR(TRIGGER_TR);

// other
okapi::Controller controller;
pros::Controller controllerPros(pros::E_CONTROLLER_MASTER);

using namespace okapi;
okapi::ChassisControllerPID chassis = okapi::ChassisControllerFactory::create(
    {LEFT_MTR2, LEFT_MTR1},     // Left motors (the sensor vals are read from first motor on each side)
    {-RIGHT_MTR2, -RIGHT_MTR1}, // Right motors

    IterativePosPIDController::Gains{0.002, 0.00, 0.0035}, // distance args
    IterativePosPIDController::Gains{0.002, 0.006, 0.001}, // angle args (keeps robot straight)
    IterativePosPIDController::Gains{0.0025, 0.00, 0.001}, // turn args

    AbstractMotor::gearset::green, // normal gearset
    {4_in, 12.5_in}                // 4 inch wheels, 12.5 inch wheelbase width
);

okapi::IterativePosPIDController liftControllerPID = okapi::IterativeControllerFactory::posPID(
    LIFT_MTR,
    0.01, 0.0, 0.005, 0 // ikP, ikI, ikD, ikBias,
);

//  a fun hack to force the compiler to spit out an error that shows you the type of something (fun for working with auto)
// template <typename T> struct Foo;
// Foo<decltype(chassis)> bar;
// thx lachlan ;D