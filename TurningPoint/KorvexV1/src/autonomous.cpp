#include "main.h"
#include "okapi/api.hpp"
#include "korvexlib.h"
using namespace okapi;

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

// factory constructs
auto chassis = okapi::ChassisControllerFactory::create(
    {LEFT_MTR1, LEFT_MTR2},               // Left motors
    {-RIGHT_MTR1, -RIGHT_MTR2},           // Right motors
    okapi::AbstractMotor::gearset::green, // Normal gearset
    {4_in, 12.5_in}                       // 4 inch wheels, 12.5 inch wheelbase width
);

auto chassisController = okapi::AsyncControllerFactory::motionProfile(
    1.0,    // Maximum linear velocity of the Chassis in m/s
    2.0,    // Maximum linear acceleration of the Chassis in m/s/s
    5.0,    // Maximum linear jerk of the Chassis in m/s/s/s
    chassis // Chassis Controller
);

pros::Motor flywheelPros_A(FLY_MTR, pros::E_MOTOR_GEARSET_06, true);

pros::Motor intakeMotor_A(INTAKE_MTR, pros::E_MOTOR_GEARSET_18, true);

pros::ADIDigitalIn triggerBL_A(TRIGGER_BL);
pros::ADIDigitalIn triggerBR_A(TRIGGER_BR);
pros::ADIDigitalIn triggerTL_A(TRIGGER_TL);
pros::ADIDigitalIn triggerTR_A(TRIGGER_TR);

void autonomous()
{
    // setup
    flywheelPros_A.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    // actual auton
    intakeMotor_A.move_velocity(200);
    chassis.moveDistanceAsync(970); // going to cap with ball under it
    // wait until we intake ball to bot
    while (!(triggerBL_A.get_new_press() || triggerBR_A.get_new_press()))
    {
        pros::delay(20);
    }
    // theres a ball at the top, we want to pull it down back to the trigger
    intakeMotor_A.move_velocity(-200);
    while (!(triggerTL_A.get_new_press() || triggerTR_A.get_new_press()))
    {
        pros::delay(20);
    }
    intakeMotor_A.move_velocity(0);
    // there is now a ball in both positions
    flywheelPros_A.move_velocity(590);
    chassis.moveDistance(-1000);
    // back and turn into shooting position
    chassis.turnAngle(255);
    chassis.moveDistance(-420);
    // shoot first ball when ready
    while (!(flywheelPros_A.get_actual_velocity() > 590))
    {
        pros::delay(20);
    }
    intakeMotor_A.move_velocity(200);
    pros::delay(500);
    intakeMotor_A.move_velocity(0);
    // start moving for bot flag toggle
    chassis.moveDistance(1000);

    // shoot second ball
    while (!(flywheelPros_A.get_actual_velocity() > 590))
    {
        pros::delay(20);
    }
    intakeMotor_A.move_velocity(200);
    pros::delay(500);
    intakeMotor_A.move_velocity(0);
    flywheelPros_A.move_velocity(0);
    // ram into bot flag
    chassis.moveDistance(400);
    // move to park
    chassis.moveDistance(-1900);
    chassis.turnAngle(-260);
    chassis.moveDistance(800);
}
