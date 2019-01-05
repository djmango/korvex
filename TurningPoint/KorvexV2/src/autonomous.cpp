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

// auto chassis = ChassisControllerFactory::create( // .002 .0005 .002
//     {LEFT_MTR1, LEFT_MTR2},                      // Left motors
//     {-RIGHT_MTR1, -RIGHT_MTR2},                  // Right motors

//     IterativePosPIDController::Gains{0.004, 0.000, 0.004}, // distance args
//     IterativePosPIDController::Gains{0.0035, 0.00, 0.001},  // angle args (keeps robot straight)
//     IterativePosPIDController::Gains{0.002, 0.00, 0.0005}, // turn args

//     AbstractMotor::gearset::green, // normal gearset
//     {4_in, 12.5_in}                // 4 inch wheels, 12.5 inch wheelbase width
// );

void autonomous()
{
    int autonStart = pros::millis(); // note the start time
    chassis.resetSensors();
    // -1 = skills
    // 0 = blue close, mid and top flag and park
    // 1 = blue close, all flags and park
    // 2 = blue far, opponent descore
    // 3 = red close, mid and top flag and park
    int auton = 5;
    // int auton = autonSelection;
    // std::cout << autonSelection << std::endl;
    int tmp = 0;
    switch (auton)
    {
    case -2: // test
        liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        chassis.setMaxVelocity(200); // this might fix things

        // actual auton
        chassis.moveDistance(8_in);
        // chassis.waitUntilSettled();
        // chassis.moveDistance(-8_in);
        // chassis.turnAngle(350);
        // chassis.waitUntilSettled();
        // chassis.turnAngle(-350);
        chassis.stop();
        break;
    case -1: // skills
        // setup
        liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        chassis.setMaxVelocity(100); // this might fix things

        // actual auton
        intakeMotor.move_velocity(200);
        chassis.moveDistanceAsync(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && !(tmp > 200)) // 1 sec timeout
        {
            pros::delay(20);
            tmp++;
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor.move_velocity(-200);
        while (!(triggerTL.get_new_press() || triggerTR.get_new_press()))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(0);

        // there is now a ball in both positions
        flywheelMotor1.move_velocity(600);
        flywheelMotor2.move_velocity(600);
        chassis.moveDistance(-39_in);
        // back and turn into shooting position
        chassis.turnAngle(-267);
        chassis.moveDistance(-15_in);
        // shoot first ball when ready
        while (!(flywheelMotor1.get_actual_velocity() > 590))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(200);
        pros::delay(500);
        intakeMotor.move_velocity(0);

        // second ball shot position
        chassis.moveDistance(33_in);

        // shoot second ball
        while (!(flywheelMotor1.get_actual_velocity() > 590))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(200);
        pros::delay(500);
        intakeMotor.move_velocity(0);
        flywheelMotor1.move_velocity(0);
        flywheelMotor2.move_velocity(0);

        // run into bot flag
        chassis.turnAngle(-25);
        chassis.moveDistance(24_in);
        chassis.turnAngle(15);

        // line up for far cap
        chassis.moveDistance(-64_in);
        chassis.turnAngle(280);

        // park
        chassis.moveDistance(18_in);
        chassis.moveDistance(48_in);

        // move to get far cap
        // intakeMotor.move_velocity(200);
        // flywheelMotor1.move_velocity(550);
        // chassis.moveDistance(44_in);

        // // intake ball
        // while (!(triggerTL.get_new_press() || triggerTR.get_new_press()))
        // {
        //     pros::delay(20);
        // }
        // intakeMotor.move_velocity(0);

        // // aim for mid flag and shoot
        // chassis.turnAngle(-280);
        // while (!(flywheelMotor1.get_actual_velocity() > 540))
        // {
        //     pros::delay(20);
        // }
        // intakeMotor.move_velocity(200);
        // pros::delay(500);
        // intakeMotor.move_velocity(0);

        // // turn for park
        // chassis.turnAngle(-250);
        // chassis.moveDistance(8_in);
        // chassis.turnAngle(270);

        // // park
        // chassis.moveDistance(30_in);
        break;
    case 0: // blue close, mid and top flag and park
        // setup
        liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        chassis.setMaxVelocity(150); // this might fix things

        // actual auton
        intakeMotor.move_velocity(200);
        chassis.moveDistanceAsync(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && !(tmp > 100)) // 2 sec timeout
        {
            pros::delay(20);
            tmp++;
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor.move_velocity(-200);
        while (!(triggerTL.get_new_press() || triggerTR.get_new_press()))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(0);

        // there is now a ball in both positions
        flywheelMotor1.move_velocity(600);
        flywheelMotor2.move_velocity(600);
        chassis.moveDistance(-36_in);
        // back and turn into shooting position
        chassis.turnAngle(500);
        chassis.moveDistance(-7_in);
        // shoot first ball when ready
        while (!(flywheelMotor1.get_actual_velocity() > 590))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(200);
        pros::delay(500);
        intakeMotor.move_velocity(0);

        // second ball shot position
        chassis.moveDistance(27_in);

        // shoot second ball
        while (!(flywheelMotor1.get_actual_velocity() > 590))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(200);
        pros::delay(600);
        intakeMotor.move_velocity(0);
        flywheelMotor1.move_velocity(0);
        flywheelMotor2.move_velocity(0);

        // move to park
        chassis.setMaxVelocity(200);
        chassis.moveDistance(-45_in);

        // turn to drive onto platform
        chassis.turnAngle(-550);

        // align with platform
        chassis.moveDistance(16_in);

        // drive onto platform
        chassis.moveDistanceAsync(25_in);
        // TODO: make it so you read milis on auton start and if at anytime we are about to reach 15 run the stop()
        while (!(pros::millis() > autonStart + 14950))
        {
            pros::delay(20);
        }
        chassis.stop();
        break;

    case 1: // blue front heavy
        // setup
        liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        chassis.setMaxVelocity(150); // this might fix things

        // actual auton
        intakeMotor.move_velocity(200);
        chassis.moveDistanceAsync(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && !(tmp > 100)) // 2 sec timeout
        {
            pros::delay(20);
            tmp++;
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor.move_velocity(-200);
        while (!(triggerTL.get_new_press() || triggerTR.get_new_press()))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(0);

        // there is now a ball in both positions
        flywheelMotor1.move_velocity(600);
        flywheelMotor2.move_velocity(600);
        chassis.moveDistance(-38_in);
        // back and turn into shooting position
        chassis.turnAngle(490);
        chassis.moveDistance(-7_in);
        // shoot first ball when ready
        while (!(flywheelMotor1.get_actual_velocity() > 590))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(200);
        pros::delay(500);
        intakeMotor.move_velocity(0);

        // second ball shot position
        chassis.moveDistance(27_in);

        // shoot second ball
        while (!(flywheelMotor1.get_actual_velocity() > 590))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(200);
        pros::delay(600);
        intakeMotor.move_velocity(0);
        flywheelMotor1.move_velocity(0);
        flywheelMotor2.move_velocity(0);

        // hit third flag
        chassis.turnAngle(80);
        chassis.moveDistance(26_in);
        chassis.turnAngle(-40);

        // its parking time
        chassis.setMaxVelocity(200);
        chassis.moveDistance(-70_in);

        // turn to drive onto platform
        chassis.turnAngle(-550);

        // align with platform
        chassis.moveDistance(17_in);
        intakeMotor.move_velocity(200);

        // drive onto platform
        chassis.moveDistanceAsync(30_in);

        // back for cap flip
        // chassis.moveDistance(-18_in);

        // turn for cap flip
        // chassis.turnAngle(-550);

        // // flip cap
        // intakeMotor.move_velocity(-200);
        // chassis.moveDistance(10_in);
        // chassis.moveDistance(-10_in);

        // // turn for park
        // chassis.turnAngle(-600);
        // chassis.moveDistance(30_in);

        while (!(pros::millis() > autonStart + 14950))
        {
            pros::delay(20);
        }
        // chassis.stop(); this only makes sense if we have a stopblock
        break;

    case 2: // blue stack
        // setup
        liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        chassis.setMaxVelocity(150); // this might fix things

        // actual auton
        intakeMotor.move_velocity(200);
        chassis.moveDistanceAsync(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && !(tmp > 400)) // 2 sec timeout
        {
            pros::delay(20);
            tmp++;
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor.move_velocity(-200);
        while (!(triggerTL.get_new_press() || triggerTR.get_new_press()))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(0);

        // there is now a ball in both positions, back for claw flipout
        chassis.moveDistance(-6_in);

        // turn and park
        chassis.turnAngle(560);
        chassis.moveDistance(8_in);
        chassis.moveDistance(23_in);

        // turn for shoot
        flywheelMotor1.move_velocity(550);
        flywheelMotor2.move_velocity(550);
        chassis.turnAngle(-210);
        chassis.turnAngleAsync(0);

        // shoot ball when ready
        while (!(flywheelMotor1.get_actual_velocity() > 545))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(200);
        pros::delay(500);
        intakeMotor.move_velocity(0);

        pros::delay(500);
        flywheelMotor1.move_velocity(0);
        flywheelMotor2.move_velocity(0);

        // turn for cap flip
        // intakeMotor.move_velocity(-200);
        // chassis.turnAngle(-580);

        // // drive to cap
        // chassis.moveDistance(10_in);
        // chassis.turnAngle(340);

        // // flip
        // chassis.setMaxVelocity(140);
        // chassis.moveDistance(24_in);
        // pros::delay(1000);

        // // back and turn for park
        // chassis.setMaxVelocity(200);
        // intakeMotor.move_velocity(0);
        // chassis.moveDistance(-10_in);
        // chassis.turnAngle(620);

        // chassis.moveDistance(25_in);
        // chassis.moveDistanceAsync(15_in);

        // turn around to pick up cap and flipout
        // chassis.turnAngleAsync(1100);
        // liftMotor.move_absolute(300, 200);
        // pros::delay(800);
        // liftMotor.move_absolute(-70, 100);
        // pros::delay(600);

        // // grab cap
        // chassis.moveDistance(-20_in);
        // liftMotor.move_absolute(1200, 100);

        // // drive and turn for post align
        // chassis.moveDistance(20_in);
        // chassis.turnAngle(-525);

        // // drive into post
        // liftMotor.move_absolute(1400, 150);
        // chassis.moveDistance(-24.5_in);

        // // drop cap on post
        // pros::delay(1000);
        // liftMotor.move_absolute(800, 100);
        // pros::delay(800);

        // // back out
        // chassis.moveDistance(13_in);
        // liftMotor.move_absolute(0, 150);

        // flip the bad bois top flag
        // chassis.turnAngle(-240);
        // flywheelMotor1.move_velocity(590);
        // flywheelMotor2.move_velocity(590);

        // // shoot ball when ready
        // while (!(flywheelMotor1.get_actual_velocity() > 585))
        // {
        //     pros::delay(20);
        // }
        // intakeMotor.move_velocity(200);
        // pros::delay(500);
        // intakeMotor.move_velocity(0);

        // pros::delay(500);
        // flywheelMotor1.move_velocity(0);
        // flywheelMotor2.move_velocity(0);

        // parkk
        // chassis.turnAngle(-300);
        // chassis.moveDistance(17_in);
        // chassis.turnAngle(180);
        // chassis.moveDistanceAsync(24_in);
        // while ((pros::millis() - 14950 < autonStart))
        // {
        //     pros::delay(20);
        // }
        // chassis.stop();
        break;
    case 3: // red close, mid and top flag and park
        break;

    case 4: // red front heavy
        // setup
        liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        chassis.setMaxVelocity(150); // this might fix things

        // actual auton
        intakeMotor.move_velocity(200);
        chassis.moveDistanceAsync(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && !(tmp > 100)) // 2 sec timeout
        {
            pros::delay(20);
            tmp++;
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor.move_velocity(-200);
        while (!(triggerTL.get_new_press() || triggerTR.get_new_press()))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(0);

        // there is now a ball in both positions
        flywheelMotor1.move_velocity(600);
        flywheelMotor2.move_velocity(600);
        chassis.moveDistance(-38_in);
        // back and turn into shooting position
        chassis.turnAngle(-575);
        chassis.moveDistance(-7_in);
        chassis.turnAngleAsync(-10);
        // shoot first ball when ready
        while (!(flywheelMotor1.get_actual_velocity() > 590))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(200);
        pros::delay(500);
        intakeMotor.move_velocity(0);

        // second ball shot position
        chassis.moveDistance(27_in);

        // shoot second ball
        while (!(flywheelMotor1.get_actual_velocity() > 590))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(200);
        pros::delay(600);
        intakeMotor.move_velocity(-200);
        flywheelMotor1.move_velocity(0);
        flywheelMotor2.move_velocity(0);

        // hit third flag
        chassis.turnAngle(-70);
        chassis.moveDistance(26_in);
        chassis.turnAngle(50);

        // its parking time
        chassis.setMaxVelocity(200);
        chassis.moveDistance(-67_in);

        // turn to drive onto platform
        chassis.turnAngle(520);

        // align with platform
        chassis.moveDistance(17_in);
        intakeMotor.move_velocity(200);

        // drive onto platform
        chassis.moveDistanceAsync(30_in);
        break;

    case 5: // red far
            // setup
        liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheelMotor2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        chassis.setMaxVelocity(150); // this might fix things

        // actual auton
        intakeMotor.move_velocity(200);
        chassis.moveDistanceAsync(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && !(tmp > 400)) // 2 sec timeout
        {
            pros::delay(20);
            tmp++;
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        intakeMotor.move_velocity(-200);
        while (!(triggerTL.get_new_press() || triggerTR.get_new_press()))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(0);

        // there is now a ball in both positions, back for claw flipout
        chassis.moveDistance(-7_in);

        // turn and park
        chassis.turnAngle(-560);
        chassis.moveDistance(7_in);
        chassis.moveDistance(23_in);

        // turn for shoot
        flywheelMotor1.move_velocity(550);
        flywheelMotor2.move_velocity(550);
        chassis.turnAngle(220);
        chassis.turnAngleAsync(0);

        // shoot ball when ready
        while (!(flywheelMotor1.get_actual_velocity() > 545))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(200);
        pros::delay(500);
        intakeMotor.move_velocity(0);
        pros::delay(500);
        // TODO: enable auton switch, add double shot to other auton
        // second ball
        flywheelMotor1.move_velocity(480);
        flywheelMotor2.move_velocity(480);
        // shoot ball when ready
        while (!(flywheelMotor1.get_actual_velocity() > 475))
        {
            pros::delay(20);
        }
        intakeMotor.move_velocity(200);
        pros::delay(500);
        intakeMotor.move_velocity(0);

        flywheelMotor1.move_velocity(0);
        flywheelMotor2.move_velocity(0);
        break;
    default:
        break;
    }
}
