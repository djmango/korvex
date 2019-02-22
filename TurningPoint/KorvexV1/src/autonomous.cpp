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
// auto chassisA = okapi::ChassisControllerFactory::create(
//     {LEFT_MTR1, LEFT_MTR2},               // Left motors
//     {-RIGHT_MTR1, -RIGHT_MTR2},           // Right motors
//     okapi::AbstractMotor::gearset::green, // Normal gearset
//     {4_in, 12.5_in}                       // 4 inch wheels, 12.5 inch wheelbase width
// );

auto chassisA = ChassisControllerFactory::create( // .002 .0005 .002
    {LEFT_MTR1, LEFT_MTR2},                      // Left motors
    {-RIGHT_MTR1, -RIGHT_MTR2},                  // Right motors
    // IterativePosPIDController::Gains{0.007, 0.000, 0.007}, // distance args
    // IterativePosPIDController::Gains{0.003, 0.00, 0.002}, // angle args (keeps robot straight)
    // IterativePosPIDController::Gains{0.005, 0.00, 0.0005}, // turn args

    IterativePosPIDController::Gains{0.004, 0.000, 0.004}, // distance args
    IterativePosPIDController::Gains{0.0035, 0.00, 0.001},  // angle args (keeps robot straight)
    IterativePosPIDController::Gains{0.002, 0.00, 0.0005}, // turn args

    AbstractMotor::gearset::green, // normal gearset
    {4_in, 12.5_in}                // 4 inch wheels, 12.5 inch wheelbase width
);

pros::Motor flywheelMotor_A1(FLY_MTR1, pros::E_MOTOR_GEARSET_06, true);
pros::Motor flywheelMotor_A2(FLY_MTR2, pros::E_MOTOR_GEARSET_06, true);
pros::Motor intakeMotor_A(INTAKE_MTR, pros::E_MOTOR_GEARSET_18, true);
pros::Motor liftMotor_A(LIFT_MTR, pros::E_MOTOR_GEARSET_18, false);

pros::ADIDigitalIn triggerBL_A(TRIGGER_BL);
pros::ADIDigitalIn triggerBR_A(TRIGGER_BR);
pros::ADIDigitalIn triggerTL_A(TRIGGER_TL);
pros::ADIDigitalIn triggerTR_A(TRIGGER_TR);

void autonomous()
{
    chassisA.resetSensors();
    // -1 = skills
    // 0 = blue close, mid and top flag and park
    // 1 = blue close, all flags and park
    // 2 = blue far, opponent descore
    // 3 = red close, mid and top flag and park
    int auton = -2;
    // int auton = autonSelection;
    int tmp = 0;
    switch (auton)
    {
    case -2: // test
        liftMotor_A.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor_A1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor_A2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        chassisA.setMaxVelocity(200); // this might fix things

        // actual auton
        chassisA.moveDistance(8_in);
        // chassisA.waitUntilSettled();
        // chassisA.moveDistance(-8_in);
        // chassisA.turnAngle(350);
        // chassisA.waitUntilSettled();
        // chassisA.turnAngle(-350);
        chassisA.stop();
        break;
    case -1: // skills
        // setup
        liftMotor_A.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor_A1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor_A2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        chassisA.setMaxVelocity(100); // this might fix things

        // actual auton
        intakeMotor_A.move_velocity(200);
        chassisA.moveDistanceAsync(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        while (!(triggerBL_A.get_new_press() || triggerBR_A.get_new_press()) && !(tmp > 200)) // 1 sec timeout
        {
            pros::delay(20);
            tmp++;
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor_A.move_velocity(-200);
        while (!(triggerTL_A.get_new_press() || triggerTR_A.get_new_press()))
        {
            pros::delay(20);
        }
        intakeMotor_A.move_velocity(0);

        // there is now a ball in both positions
        flywheelMotor_A1.move_velocity(600);
        flywheelMotor_A2.move_velocity(600);
        chassisA.moveDistance(-39_in);
        // back and turn into shooting position
        chassisA.turnAngle(-267);
        chassisA.moveDistance(-15_in);
        // shoot first ball when ready
        while (!(flywheelMotor_A1.get_actual_velocity() > 590))
        {
            pros::delay(20);
        }
        intakeMotor_A.move_velocity(200);
        pros::delay(500);
        intakeMotor_A.move_velocity(0);

        // second ball shot position
        chassisA.moveDistance(33_in);

        // shoot second ball
        while (!(flywheelMotor_A1.get_actual_velocity() > 590))
        {
            pros::delay(20);
        }
        intakeMotor_A.move_velocity(200);
        pros::delay(500);
        intakeMotor_A.move_velocity(0);
        flywheelMotor_A1.move_velocity(0);
        flywheelMotor_A2.move_velocity(0);

        // run into bot flag
        chassisA.turnAngle(-25);
        chassisA.moveDistance(24_in);
        chassisA.turnAngle(15);

        // line up for far cap
        chassisA.moveDistance(-64_in);
        chassisA.turnAngle(280);

        // park
        chassisA.moveDistance(18_in);
        chassisA.moveDistance(48_in);

        // move to get far cap
        // intakeMotor_A.move_velocity(200);
        // flywheelMotor_A1.move_velocity(550);
        // chassisA.moveDistance(44_in);

        // // intake ball
        // while (!(triggerTL_A.get_new_press() || triggerTR_A.get_new_press()))
        // {
        //     pros::delay(20);
        // }
        // intakeMotor_A.move_velocity(0);

        // // aim for mid flag and shoot
        // chassisA.turnAngle(-280);
        // while (!(flywheelMotor_A1.get_actual_velocity() > 540))
        // {
        //     pros::delay(20);
        // }
        // intakeMotor_A.move_velocity(200);
        // pros::delay(500);
        // intakeMotor_A.move_velocity(0);

        // // turn for park
        // chassisA.turnAngle(-250);
        // chassisA.moveDistance(8_in);
        // chassisA.turnAngle(270);

        // // park
        // chassisA.moveDistance(30_in);
        break;
    case 0: // blue close, mid and top flag and park
        // setup
        liftMotor_A.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor_A1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor_A2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        chassisA.setMaxVelocity(150); // this might fix things

        // actual auton
        intakeMotor_A.move_velocity(200);
        chassisA.moveDistanceAsync(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        while (!(triggerBL_A.get_new_press() || triggerBR_A.get_new_press()) && !(tmp > 100)) // 2 sec timeout
        {
            pros::delay(20);
            tmp++;
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor_A.move_velocity(-200);
        while (!(triggerTL_A.get_new_press() || triggerTR_A.get_new_press()))
        {
            pros::delay(20);
        }
        intakeMotor_A.move_velocity(0);

        // there is now a ball in both positions
        flywheelMotor_A1.move_velocity(600);
        flywheelMotor_A2.move_velocity(600);
        chassisA.moveDistance(-36_in);
        // back and turn into shooting position
        chassisA.turnAngle(500);
        chassisA.moveDistance(-7_in);
        // shoot first ball when ready
        while (!(flywheelMotor_A1.get_actual_velocity() > 590))
        {
            pros::delay(20);
        }
        intakeMotor_A.move_velocity(200);
        pros::delay(500);
        intakeMotor_A.move_velocity(0);

        // second ball shot position
        chassisA.moveDistance(27_in);

        // shoot second ball
        while (!(flywheelMotor_A1.get_actual_velocity() > 590))
        {
            pros::delay(20);
        }
        intakeMotor_A.move_velocity(200);
        pros::delay(600);
        intakeMotor_A.move_velocity(0);
        flywheelMotor_A1.move_velocity(0);
        flywheelMotor_A2.move_velocity(0);

        // move to park
        chassisA.setMaxVelocity(200);
        chassisA.moveDistance(-45_in);

        // turn to drive onto platform
        chassisA.turnAngle(-550);

        // align with platform
        chassisA.moveDistance(16_in);
        
        // drive onto platform
        chassisA.moveDistance(25_in);
        chassisA.stop();
        break;

    case 1: // blue far
        break;

    case 2: // blue stack
        // setup
        liftMotor_A.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor_A1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor_A2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        chassisA.setMaxVelocity(150); // this might fix things

        // actual auton
        intakeMotor_A.move_velocity(200);
        chassisA.moveDistanceAsync(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        while (!(triggerBL_A.get_new_press() || triggerBR_A.get_new_press()) && !(tmp > 400)) // 2 sec timeout
        {
            pros::delay(20);
            tmp++;
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor_A.move_velocity(-200);
        while (!(triggerTL_A.get_new_press() || triggerTR_A.get_new_press()))
        {
            pros::delay(20);
        }
        intakeMotor_A.move_velocity(0);

        // there is now a ball in both positions, back for claw flipout
        chassisA.moveDistance(-18_in);

        // turn around to pick up cap and flipout
        chassisA.turnAngleAsync(1100);
        liftMotor_A.move_absolute(300, 200);
        pros::delay(800);
        liftMotor_A.move_absolute(-70, 100);
        pros::delay(600);

        // grab cap
        chassisA.moveDistance(-20_in);
        liftMotor_A.move_absolute(1200, 100);

        // drive and turn for post align
        chassisA.moveDistance(20_in);
        chassisA.turnAngle(-525);

        // drive into post
        liftMotor_A.move_absolute(1400, 150);
        chassisA.moveDistance(-24.5_in);

        // drop cap on post
        pros::delay(1000);
        liftMotor_A.move_absolute(800, 100);
        pros::delay(800);

        // back out
        chassisA.moveDistance(13_in);
        liftMotor_A.move_absolute(0, 150);

        // flip the bad bois top flag
        // chassisA.turnAngle(-240);
        // flywheelMotor_A1.move_velocity(590);
        // flywheelMotor_A2.move_velocity(590);

        // // shoot ball when ready
        // while (!(flywheelMotor_A1.get_actual_velocity() > 585))
        // {
        //     pros::delay(20);
        // }
        // intakeMotor_A.move_velocity(200);
        // pros::delay(500);
        // intakeMotor_A.move_velocity(0);

        // pros::delay(500);
        // flywheelMotor_A1.move_velocity(0);
        // flywheelMotor_A2.move_velocity(0);

        // parkk
        chassisA.turnAngle(-300);
        chassisA.moveDistance(17_in);
        chassisA.turnAngle(180);
        chassisA.moveDistance(24_in);
        chassisA.stop();
        break;
    case 3: // red close, mid and top flag and park
        // setup
        liftMotor_A.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor_A1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        chassisA.setMaxVelocity(130); // this might fix things

        // actual auton
        intakeMotor_A.move_velocity(200);
        chassisA.moveDistanceAsync(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        while (!(triggerBL_A.get_new_press() || triggerBR_A.get_new_press()) && !(tmp > 200)) // 1 sec timeout
        {
            pros::delay(20);
            tmp++;
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor_A.move_velocity(-200);
        while (!(triggerTL_A.get_new_press() || triggerTR_A.get_new_press()))
        {
            pros::delay(20);
        }
        intakeMotor_A.move_velocity(0);

        // there is now a ball in both positions
        flywheelMotor_A1.move_velocity(600);
        chassisA.moveDistance(-37_in);
        // back and turn into shooting position
        chassisA.turnAngle(-269);
        chassisA.moveDistance(-14_in);
        // shoot first ball when ready
        while (!(flywheelMotor_A1.get_actual_velocity() > 590))
        {
            pros::delay(20);
        }
        intakeMotor_A.move_velocity(200);
        pros::delay(500);
        intakeMotor_A.move_velocity(0);

        // second ball shot position
        chassisA.moveDistance(30_in);
        chassisA.setMaxVelocity(180);

        // shoot second ball
        while (!(flywheelMotor_A1.get_actual_velocity() > 590))
        {
            pros::delay(20);
        }
        intakeMotor_A.move_velocity(200);
        pros::delay(500);
        intakeMotor_A.move_velocity(0);
        flywheelMotor_A1.move_velocity(0);

        // move to park
        chassisA.moveDistance(-42_in);
        chassisA.turnAngle(230);
        chassisA.moveDistance(18_in);
        chassisA.moveDistance(32_in);
        break;

    case 4: // red elim setup, mid flags
        // setup
        liftMotor_A.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor_A1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        chassisA.setMaxVelocity(110); // this might fix things

        // actual auton
        intakeMotor_A.move_velocity(200);
        chassisA.moveDistanceAsync(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        while (!(triggerBL_A.get_new_press() || triggerBR_A.get_new_press()) && !(tmp > 200)) // 1 sec timeout
        {
            pros::delay(20);
            tmp++;
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor_A.move_velocity(-200);
        while (!(triggerTL_A.get_new_press() || triggerTR_A.get_new_press()))
        {
            pros::delay(20);
        }
        intakeMotor_A.move_velocity(0);

        // there is now a ball in both positions
        flywheelMotor_A1.move_velocity(475);
        chassisA.moveDistance(-37_in);

        // back and turn into shooting position
        chassisA.turnAngle(-274);
        chassisA.moveDistance(-7_in);

        // shoot first ball when ready
        while (!(flywheelMotor_A1.get_actual_velocity() > 475))
        {
            pros::delay(20);
        }
        intakeMotor_A.move_velocity(200);
        pros::delay(500);
        intakeMotor_A.move_velocity(0);

        // second ball shot
        flywheelMotor_A1.move_velocity(518);
        // chassisA.moveDistance(30_in);
        // chassisA.setMaxVelocity(180);

        // shoot second ball
        while (!(flywheelMotor_A1.get_actual_velocity() > 518))
        {
            pros::delay(20);
        }
        intakeMotor_A.move_velocity(200);
        pros::delay(500);
        intakeMotor_A.move_velocity(0);
        flywheelMotor_A1.move_velocity(0);

        // move to park
        chassisA.moveDistance(29_in);
        chassisA.turnAngle(230);
        chassisA.moveDistance(18_in);
        chassisA.moveDistance(32_in);
        break;

    case 5: // red far
        // setup
        liftMotor_A.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor_A1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        chassisA.setMaxVelocity(110); // this might fix things

        // actual auton
        intakeMotor_A.move_velocity(200);
        chassisA.moveDistanceAsync(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        while (!(triggerBL_A.get_new_press() || triggerBR_A.get_new_press()) && !(tmp > 200)) // 1 sec timeout
        {
            pros::delay(20);
            tmp++;
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor_A.move_velocity(-200);
        while (!(triggerTL_A.get_new_press() || triggerTR_A.get_new_press()))
        {
            pros::delay(20);
        }
        intakeMotor_A.move_velocity(0);

        // there is now a ball in both positions
        flywheelMotor_A1.move_velocity(585);

        // backup for shooting and park location
        chassisA.moveDistance(-8_in);
        chassisA.turnAngle(-260);

        // shoot first ball when ready
        while (!(flywheelMotor_A1.get_actual_velocity() > 580))
        {
            pros::delay(20);
        }
        intakeMotor_A.move_velocity(200);
        pros::delay(500);
        intakeMotor_A.move_velocity(0);
        flywheelMotor_A1.move_velocity(0);
        pros::delay(900);

        // prepare next shot
        flywheelMotor_A1.move_velocity(580);
        chassisA.moveDistance(5_in);
        chassisA.moveDistance(-5_in);

        while (!(flywheelMotor_A1.get_actual_velocity() > 575))
        {
            pros::delay(20);
        }
        // shoot (me)
        intakeMotor_A.move_velocity(200);
        pros::delay(500);
        intakeMotor_A.move_velocity(0);
        flywheelMotor_A1.move_velocity(0);

        // park
        chassisA.moveDistance(5_in);
        chassisA.moveDistance(26_in);

        break;
    default:
        break;
    }
}
