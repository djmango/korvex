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

void autonomous()
{
    int autonStart = pros::millis(); // note the start time
    chassis.resetSensors();
    // -1 = skills
    // 0 = blue close, mid and top flag and park
    // 1 = blue close, all flags and park
    // 2 = blue far, opponent descore
    // 3 = red close, mid and top flag and park
    int auton = 1;
    // int auton = autonSelection; // this is to enable auton selector
    // std::cout << autonSelection << std::endl;
    bool preload = false;
    int tmp = 0;
    switch (auton) // TODO: retune turns, test both autons and clone 3 flag + park for red
    {
    case -2: // test
        chassis.turnAngle(90_deg);
        pros::delay(1000);
        chassis.turnAngle(-90_deg);
        break;
    case -1: // skills
        break;
    case 0: // blue close, full post, capflip and park
        // setup
        flywheelController.setGearing(okapi::AbstractMotor::gearset::blue);
        flywheelController.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
        tmp = 0;

        // actual auton

        intakeMotor.move_velocity(200);
        chassis.moveDistance(39_in); // going to cap with ball under it

        // wait until we intake ball to bot
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && !(tmp > 100)) // 2 sec timeout
        {
            tmp++;
            pros::delay(20);
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor.move_relative(-400, 200);

        // there is now a ball in both positions
        flywheelController.moveVelocity(600);
        chassis.moveDistance(-37_in);
        // back and turn into shooting position
        chassis.turnAngle(90_deg);
        chassis.moveDistance(-7_in);
        // shoot first ball when ready
        tmp = 0;
        while (!(flywheelController.getActualVelocity() > 590) && !(tmp > 50))
        {
            tmp++;
            pros::delay(20);
        }
        intakeMotor.move_relative(2000, 200);

        // shoot second ball and move to flip bot flag
        pros::delay(500);
        chassis.moveDistanceAsync(50_in);
        pros::delay(300); // this is the timing for the run and shoot
        intakeMotor.move_velocity(200);
        pros::delay(500);
        intakeMotor.move_velocity(0);
        flywheelController.moveVelocity(0);
        chassis.waitUntilSettled();

        chassis.moveDistance(-35_in);

        // turn to flip cap
        chassis.turnAngle(-45_deg);

        // move to cap
        capflipMotor.move_absolute(-700, 200);
        chassis.moveDistance(18_in);

        // flip cap
        capflipMotor.move_absolute(0, 150);
        pros::delay(200);

        // park
        chassis.turnAngle(-45_deg);
        chassis.moveDistance(18_in);
        chassis.turnAngle(-90_deg);

        // onwards soldiers! take the platform! fuck im bored
        chassis.moveDistance(30_in);
        chassis.moveDistance(20_in);

        break;

    case 1: // blue full post and park only
        // setup
        flywheelController.setGearing(okapi::AbstractMotor::gearset::blue);
        flywheelController.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
        tmp = 0;

        // actual auton

        intakeMotor.move_velocity(200);
        chassis.moveDistance(39_in); // going to cap with ball under it

        // wait until we intake ball to bot
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && !(tmp > 100)) // 2 sec timeout
        {
            tmp++;
            pros::delay(20);
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        pros::delay(100);
        intakeMotor.move_relative(-400, 200);

        // there is now a ball in both positions
        flywheelController.moveVelocity(600);
        chassis.moveDistance(-37_in);
        // back and turn into shooting position
        chassis.turnAngle(93_deg);
        chassis.moveDistance(-7_in);
        // shoot first ball when ready
        tmp = 0;
        while (!(flywheelController.getActualVelocity() > 590) && !(tmp > 50))
        {
            tmp++;
            pros::delay(20);
        }
        intakeMotor.move_relative(1500, 200);
        pros::delay(500);

        // shoot second ball
        flywheelController.moveVelocity(0);
        pros::delay(100);
        flywheelController.moveVelocity(490);
        intakeMotor.move_relative(500, 200);

        // move to flip bot flag
        chassis.turnAngleAsync(2_deg);
        pros::delay(600);
        chassis.moveDistanceAsync(52_in);
        pros::delay(500); // this is the timing for the run and shoot
        intakeMotor.move_velocity(-200);
        pros::delay(300);
        flywheelController.moveVelocity(0);
        chassis.waitUntilSettled();

        chassis.moveDistance(-68_in);

        // turn to park
        chassis.turnAngle(-85_deg);

        // move to platform
        chassis.moveDistance(23_in);

        // onto platform
        chassis.moveDistance(27_in);
        break;

    case 2: // blue stack
        break;
    case 3: // red close, mid and top flag and park
        break;

    case 4: // red front heavy
        break;
    case 5: // red far
        break;
    default:
        break;
    }
}
