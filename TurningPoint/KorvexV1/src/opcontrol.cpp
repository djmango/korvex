#include "main.h"
#include "okapi/api.hpp"
using namespace okapi;

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

// ports for motors

// chassis motors
const int LEFT_MTR1 = 1;
const int LEFT_MTR2 = 2;
const int RIGHT_MTR1 = 3;
const int RIGHT_MTR2 = 4;

// lift and claw motor declerations
const int LIFT_MTR = 5;
const int CLAW_MTR = 6;

// flywheel and intake motors
const int FLY_MTR = 7;
const int INTAKE_MTR = 8;

void opcontrol()
{

	// factory constructs
	auto chassis = okapi::ChassisControllerFactory::create(
		{-LEFT_MTR1, -LEFT_MTR2},				  // Left motors
		{RIGHT_MTR1, RIGHT_MTR2},				  // Right motors
		okapi::AbstractMotor::gearset::green, // Torque gearset
		{4_in, 12.5_in}						  // 4 inch wheels, 12.5 inch wheelbase width
	);

	auto chassisControllers = okapi::AsyncControllerFactory::motionProfile(
		1.0,	// Maximum linear velocity of the Chassis in m/s
		2.0,	// Maximum linear acceleration of the Chassis in m/s/s
		10.0,   // Maximum linear jerk of the Chassis in m/s/s/s
		chassis // Chassis Controller
	);

	auto liftControl = AsyncControllerFactory::posIntegrated(LIFT_MTR);
	int liftGoal = 0;

	okapi::Controller controller;

	while (true)
	{
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
						 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
						 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);
		
		if (controller.getDigital(ControllerDigital::up))
		{
			liftGoal++;
			liftControl.setTarget(liftGoal);
		}
		else if (controller.getDigital(ControllerDigital::down))
		{
			liftGoal--;
			liftControl.setTarget(liftGoal);
		}
		chassis.tank(controller.getAnalog(ControllerAnalog::leftY),
					 controller.getAnalog(ControllerAnalog::rightY));
		pros::delay(20);
	}
}
