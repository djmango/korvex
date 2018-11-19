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

// TODO: limit switch presets
// TODO: switch to 2nd controller
// TODO: advanced lcd feedback

// module constants

const int FLY_PRESETS[7] = {0, 100, 200, 300, 400, 500, 600};
// const int FLY_SLEW[9] = {200, 175, 150, 125, 100, 75, 50, 25, 0};
const int LIFT_PRESETS[8] = {0, 20, 25, 35, 45, 60, 80, 100};
int flywheelTarget = 0;
int flywheelOutput = 0;

// im lazy
pros::Motor flywheelPros(FLY_MTR, pros::E_MOTOR_GEARSET_06, true);
void flyPID(void *)
{
	// define
	float kP = 2;
	float kI = 0;
	float kD = 1;
	int error = 0;
	int prev_error = 0;
	int velocity = 0;
	int output = 0;
	int p = 0;
	int i = 0;
	int d = 0;
	while (true)
	{
		velocity = flywheelPros.get_actual_velocity();

		error = flywheelTarget - velocity;
		// pid calc

		// proportional
		p = (error * kP);

		// integral
		if (abs(error) < (flywheelTarget / 2) && kI != 0) // if the error is greater than half of target
			i = ((i + error) * kI);
		else
			i = 0;
		
		// derivative
		d = ((error - prev_error) * kD);
		// store last error
		prev_error = error;

		// calculate output
		flywheelOutput = (p + i + d);
		printf("fly output %d\n", flywheelOutput);

		// scale
		flywheelOutput = flywheelOutput * 0.212; // 600 * 0.212 = 127
		if (flywheelOutput < 0 || flywheelTarget == 0)
		{
			flywheelOutput = 0;
		}
		if (flywheelOutput > 127) {
			flywheelOutput = 127;
		}

		// apply output
		printf("fly err %d\n", error);
		printf("fly target %d\n", flywheelTarget);
		pros::delay(50);
	}
}

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
	int liftIterate = 0;

	// flywheel and intake controllers

	int flywheelIterate = 0;
	// auto flywheelMotorOkapi = okapi::Motor(FLY_MTR, true, okapi::AbstractMotor::gearset::blue);
	// auto flywheelControl = AsyncControllerFactory::velPID(flywheelMotorOkapi, .0001, 0.01);
	pros::Task flyPIDLoop(flyPID, (void *)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "FlywheelPID");
	auto intakeControl = AsyncControllerFactory::velIntegrated(INTAKE_MTR);
	bool intakeToggle = false; // for user toggle

	okapi::Controller controller;
	pros::Controller controllerPros(pros::E_CONTROLLER_PARTNER); // default api for more functions

	while (true)
	{
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
						 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
						 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);
		// lift control
		if (controllerPros.get_digital_new_press(DIGITAL_UP) && liftIterate <= 8)
		{
			liftIterate++;
			liftControl.setTarget(LIFT_PRESETS[liftIterate]);
		}
		else if (controllerPros.get_digital_new_press(DIGITAL_DOWN) && liftIterate >= 0)
		{
			liftIterate--;
			liftControl.setTarget(LIFT_PRESETS[liftIterate]);
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
		else if (controllerPros.get_digital_new_press(DIGITAL_A))
		{
			intakeControl.setTarget(-200);
		}
		else if (intakeToggle == false)
		{
			intakeControl.setTarget(0);
		}

		// flywheel control
		if (controllerPros.get_digital_new_press(DIGITAL_R1))
		{
			flywheelIterate++;
			if (flywheelIterate > 7)
			{
				flywheelIterate = 7;
			}
			// controllerPros.print(0, 0, "FlyPreset: %d", flywheelIterate);
			flywheelTarget = FLY_PRESETS[flywheelIterate];
			controllerPros.print(1, 0, "FlyTarget: %d", flywheelTarget);
		}
		else if (controllerPros.get_digital_new_press(DIGITAL_R2) && flywheelIterate > 0)
		{
			flywheelIterate--;
			if (flywheelIterate < 0)
			{
				flywheelIterate = 0;
			}
			// controllerPros.print(0, 0, "FlyPreset: %d", flywheelIterate);
			flywheelTarget = FLY_PRESETS[flywheelIterate];
			controllerPros.print(1, 0, "FlyTarget: %d", flywheelTarget);
		}
		flywheelPros.move(flywheelOutput);
		// chassis control
		chassis.tank(controller.getAnalog(ControllerAnalog::leftY),
					 controller.getAnalog(ControllerAnalog::rightY));
		pros::delay(20);
	}
}
