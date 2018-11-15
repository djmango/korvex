#include "main.h"
#include "okapi/api.hpp"
#include "korvexlib.h"
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

// module constants

const int FLY_PRESETS[5] = {0, 50, 100, 150, 200};
const int FLY_SLEW[9] = {200, 175, 150, 125, 100, 75, 50, 25, 0};

void opcontrol()
{

	// factory constructs
	auto chassis = okapi::ChassisControllerFactory::create(
		{LEFT_MTR1, LEFT_MTR2},				  // Left motors
		{-RIGHT_MTR1, -RIGHT_MTR2},			  // Right motors
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

	// flywheel and intake controllers
	pros::Motor flywheelPros(FLY_MTR, pros::E_MOTOR_GEARSET_06, true);
	// auto flywheelControl = AsyncControllerFactory::velPID(-FLY_MTR, .0004, 0.005);
	int flywheelIterate = 0;
	flywheelTarget = 0;
	auto intakeControl = AsyncControllerFactory::velIntegrated(-INTAKE_MTR);
	bool intakeToggle = false; // for user toggle

	okapi::Controller controller;
	pros::Controller controllerPros(pros::E_CONTROLLER_MASTER); // default api for more functions
	
	while (true)
	{
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
						 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
						 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);
		// lift control
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

		// intake control
		if (controllerPros.get_digital_new_press(DIGITAL_L1)) // using pros api for new press check
		{
			if (intakeToggle == true)
			{
				intakeControl.setTarget(0);
				intakeToggle = false;
			}
			else
			{
				intakeControl.setTarget(200);
				intakeToggle = true;
			}
		}
		else if (controller.getDigital(ControllerDigital::A))
		{
			intakeControl.setTarget(-200);
		}
		else if (intakeToggle == false)
		{
			intakeControl.setTarget(0);
		}

		// flywheel control
		if (controllerPros.get_digital_new_press(DIGITAL_R1) && flywheelIterate <= 5)
		{
			flywheelIterate++;
			controllerPros.print(0, 0, "FlyPreset: %d", flywheelIterate);
			flywheelTarget = FLY_PRESETS[flywheelIterate];
		}
		else if (controllerPros.get_digital_new_press(DIGITAL_R2) && flywheelIterate > 0)
		{
			flywheelIterate--;
			controllerPros.print(0, 0, "FlyPreset: %d", flywheelIterate);
			flywheelTarget = FLY_PRESETS[flywheelIterate];
		}

		// chassis control
		chassis.tank(controller.getAnalog(ControllerAnalog::leftY),
					 controller.getAnalog(ControllerAnalog::rightY));
		pros::delay(20);
	}
}
