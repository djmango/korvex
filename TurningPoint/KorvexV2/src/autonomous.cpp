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

void gyroTurn(int target, int timeoutVal)
{

    // tune vals
    float kP = 0.002;
    float kI = 0.005;
    float kD = 0.0004;
    int iDeadband = 400; // 0 for disable
    int tolerance = 0;

    // vars
    target = target + gyro.get(); // relative
    float p = 0;
    float i = 0;
    float d = 0;
    int gyroVal = 0;
    int error = 0;
    int lastError = 0;
    int drive = 0;
    bool timeout = false;
    bool stable = false;
    int startTime = pros::millis();

    while (!timeout && !stable)
    {

        // calculate error
        gyroVal = gyro.get();
        std::cout << "gyro " << gyro.get() << std::endl;
        error = (target - gyroVal);

        // calculate pid
        p = (error * kP);
        if (abs(error) < iDeadband || iDeadband == 0)
            i = ((i + error) * kI);
        else
            i = 0;
        d = ((error - lastError) * kD);

        // store last error
        lastError = error;

        // calculate drive
        drive = (p + i + d);

        // cap vals
        if (drive > 1)
            drive = 1;
        if (drive < -1)
            drive = -1;

        chassis.rotate(drive);

        if (startTime + timeoutVal < pros::millis()) // timeout
            timeout = true;

        if (abs(error) - tolerance <= 0 && abs(lastError) - tolerance <= 0)
            stable = true;
        
        pros::delay(10);
    }
    chassis.rotate(0);
}

void autonomous()
{
    int autonStart = pros::millis(); // note the start time
    chassis.resetSensors();
    gyro.reset();
    int auton = 0;

    if (autonSelection != 10)
    {
        auton = autonSelection; // this is to enable auton selector
    }

    switch (auton)
    {
    case 5: // test
        chassis.setMaxVelocity(130);
        chassis.turnAngle(270);
        break;
    case 0: // skills
        // setup
        chassis.setMaxVelocity(130);

        // actual auton
        intakeMotor.move_velocity(200);
        chassis.moveDistance(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        timeHold = pros::millis();
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && (timeHold + 2000 > pros::millis()))
        {
            pros::delay(20);
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        pros::delay(100);
        intakeMotor.move_relative(-500, 200);
        // there is now a ball in both positions

        // back and turn for descore
        chassis.moveDistance(-4_in);
        flywheelController.moveVelocity(520);
        chassis.setMaxVelocity(180);
        chassis.turnAngle(-51_deg);
        chassis.setMaxVelocity(130);
        pros::delay(500);
        intakeMotor.move_relative(1500, 200);
        pros::delay(300);

        // second ball
        flywheelController.moveVelocity(555);
        pros::delay(1500);
        intakeMotor.move_relative(1000, 200);
        pros::delay(500);
        flywheelController.moveVelocity(0);

        // move and turn for scrape
        chassis.turnAngle(55_deg);
        chassis.moveDistance(-10_in);
        chassis.turnAngle(270); // things are weird so 270 = 90_deg
        chassis.moveDistance(22_in);
        chassis.turnAngle(-85_deg);
        chassis.moveDistance(6_in);

        // scrape
        capflipMotor.move_absolute(-565, 200);
        intakeMotor.move_velocity(200);
        pros::delay(300);
        chassis.moveDistanceAsync(-10_in);

        // wait for first ball to get to top pos
        while (!(triggerTL.get_new_press() || triggerTR.get_new_press()))
        {
            pros::delay(20);
        }

        // wait for second ball to get to bot pos, might not have worked so 2 sec timeout
        timeHold = pros::millis();
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && (timeHold + 2000 > pros::millis()))
        {
            pros::delay(20);
        }
        if (triggerBL.get_new_press() || triggerBR.get_new_press()) // if we got a second ball, let it pull up a bit
            pros::delay(100);
        intakeMotor.move_relative(-500, 200);

        // we now have atleast one ball (i could do adaptave auton cuz we know how many balls we have but dont have time nor energy to do it)
        capflipMotor.move_absolute(0, 200);
        chassis.turnAngle(270);

        // move for front flags, start with closest to red
        chassis.moveDistance(38_in);
        chassis.turnAngle(270);
        flywheelController.moveVelocity(540);
        chassis.moveDistance(68_in);

        // shoot closest pole to red
        pros::delay(500);
        intakeMotor.move_relative(1500, 200);
        pros::delay(300);

        // second ball
        flywheelController.moveVelocity(580);
        pros::delay(1500);
        intakeMotor.move_relative(1000, 200);
        pros::delay(500);
        flywheelController.moveVelocity(0);

        break;
    case 1: // blue close, full post, capflip and park
        break;

    case 2: // blue descore (far and cap only)
        // setup
        chassis.setMaxVelocity(130);

        // actual auton
        intakeMotor.move_velocity(200);
        chassis.moveDistance(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        timeHold = pros::millis();
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && (timeHold + 2000 > pros::millis()))
        {
            pros::delay(20);
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        pros::delay(100);
        intakeMotor.move_relative(-400, 200);

        // there is now a ball in both positions, shoot
        chassis.moveDistance(-4_in);
        flywheelController.moveVelocity(520);
        chassis.turnAngle(61_deg);
        pros::delay(2000); // wait for them to shoot
        intakeMotor.move_relative(1500, 200);
        pros::delay(300);

        // second ball
        flywheelController.moveVelocity(545);
        pros::delay(1000);
        intakeMotor.move_relative(1000, 200);
        pros::delay(500);
        flywheelController.moveVelocity(0);

        // CAPFLIP TIME! SUPRISUWU
        chassis.setMaxVelocity(180);
        chassis.turnAngle(-60_deg);
        chassis.moveDistance(7_in);
        chassis.turnAngle(-90_deg);
        capflipMotor.move_absolute(-700, 200);

        // flip and come back
        chassis.moveDistance(10_in);
        capflipMotor.move_absolute(0, 200);

        chassis.driveVector(0.5, 0.8);
        pros::delay(500);
        // chassis.turnAngle(-180_deg);
        // chassis.moveDistance(15_in);

        // // move and turn for park
        // chassis.turnAngle(-90_deg);
        // chassis.moveDistance(-10_in);
        chassis.turnAngle(270);

        // park
        chassis.moveDistance(7_in);
        chassis.moveDistance(26_in);

        break;
    case 3: // blue descore (far only) works!!!
        // setup
        chassis.setMaxVelocity(130);

        // actual auton
        intakeMotor.move_velocity(200);
        chassis.moveDistance(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        timeHold = pros::millis();
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && (timeHold + 2000 > pros::millis()))
        {
            pros::delay(20);
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        pros::delay(100);
        intakeMotor.move_relative(-500, 200);
        // there is now a ball in both positions

        // back and turn for descore
        chassis.moveDistance(-4_in);
        flywheelController.moveVelocity(520);
        // gyroTurn(600, 1000);
        chassis.setMaxVelocity(180);
        chassis.turnAngle(59_deg);
        chassis.setMaxVelocity(130);
        pros::delay(3000); // wait for them to shoot
        intakeMotor.move_relative(1500, 200);
        pros::delay(300);

        // second ball
        flywheelController.moveVelocity(540);
        pros::delay(1500);
        intakeMotor.move_relative(1000, 200);
        pros::delay(500);
        flywheelController.moveVelocity(0);

        // move and turn for park
        chassis.turnAngle(-59_deg);
        chassis.moveDistance(-5_in);
        chassis.turnAngle(270);

        // park
        chassis.moveDistance(7_in);
        chassis.moveDistance(26_in);
        break;
    case -1: // red close, mid and top flag and park
        break;

    case -2: // red full post and park only
        break;

    case -3: // red descore (far only)
        // setup
        chassis.setMaxVelocity(130);

        // actual auton
        intakeMotor.move_velocity(200);
        chassis.moveDistance(37_in); // going to cap with ball under it

        // wait until we intake ball to bot
        timeHold = pros::millis();
        while (!(triggerBL.get_new_press() || triggerBR.get_new_press()) && (timeHold + 2000 > pros::millis()))
        {
            pros::delay(20);
        }

        // theres a ball at the top, we want to pull it down back to the trigger
        pros::delay(100);
        intakeMotor.move_relative(-500, 200);
        // there is now a ball in both positions

        // back and turn for descore
        chassis.moveDistance(-4_in);
        flywheelController.moveVelocity(520);
        // gyroTurn(600, 1000);
        chassis.setMaxVelocity(180);
        chassis.turnAngle(-51_deg);
        chassis.setMaxVelocity(130);
        pros::delay(3000); // wait for them to shoot
        intakeMotor.move_relative(1500, 200);
        pros::delay(300);

        // second ball
        flywheelController.moveVelocity(555);
        pros::delay(1500);
        intakeMotor.move_relative(1000, 200);
        pros::delay(500);
        flywheelController.moveVelocity(0);

        // move and turn for park
        chassis.turnAngle(51_deg);
        chassis.moveDistance(-3_in);
        chassis.turnAngle(-89_deg);

        // park
        chassis.moveDistance(10_in);
        chassis.moveDistance(28_in);
        break;
    default:
        break;
    }
}
