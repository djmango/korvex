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
// TODO: advanced lcd feedback

// module constants

// flywheel velocity presets, each represents an trajectory arc of the ball
const int FLY_PRESETS[7] = {0, 100, 200, 300, 400, 500, 600};
const int FLY_PRESETS_LEN = sizeof(FLY_PRESETS) / sizeof(FLY_PRESETS[0]);

// lift position presets, different heights for different poles
const int LIFT_PRESETS[8] = {0, 20, 25, 35, 45, 60, 80, 100};
const int LIFT_PRESETS_LEN = sizeof(LIFT_PRESETS) / sizeof(LIFT_PRESETS[0]);

// globals
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

	// flywheel task startup
	int flywheelIterate = 0;
	pros::Task flyPIDLoop(flyPID, (void *)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "FlywheelPID");
	
	// intake controller
	auto intakeControl = AsyncControllerFactory::velIntegrated(INTAKE_MTR);
	bool intakeToggle = false; // for user toggle
	bool ballTriggerBottom = false; // these are for detecting if the intake
	bool ballTriggerTop = false; // triggers are activated
	pros::ADIDigitalIn triggerBL(TRIGGER_BL);
	pros::ADIDigitalIn triggerBR(TRIGGER_BR);
	pros::ADIDigitalIn triggerTL(TRIGGER_TL);
	pros::ADIDigitalIn triggerTR(TRIGGER_TR);

	okapi::Controller controller;
	pros::Controller controllerPros(pros::E_CONTROLLER_PARTNER); // default api for more functions

	// we got time
	int cycles = 0; // cycle counter
	int cyclesHold = 0; // temp thing for counting
	pros::lcd::print(0, "I got time\n");
	pros::lcd::print(1, "Me when I got time\n");
	pros::lcd::print(2, "This is so me when I got time\n");
	while (true)
	{
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
						 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
						 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);
		// lift control
		if (controllerPros.get_digital_new_press(DIGITAL_UP) && liftIterate <= LIFT_PRESETS_LEN) // if we get a new up press and we are not at max preset
		{
			liftIterate++;
			liftControl.setTarget(LIFT_PRESETS[liftIterate]);
		}
		else if (controllerPros.get_digital_new_press(DIGITAL_DOWN) && liftIterate >= 0) // if we get a new down press and we are not at min preset
		{
			liftIterate--;
			liftControl.setTarget(LIFT_PRESETS[liftIterate]);
		}

		// intake control
		// check sensors

		// get new press from either side of bottom, ensure intake is on and there is no ball already there to remove false positives
		// we also do not want to stop the intake if theres no ball at the top, as the default position should be top
		if ((triggerBL.get_new_press() || triggerBR.get_new_press()) && intakeToggle == true && ballTriggerBottom == false && ballTriggerTop == true)
		{
			intakeControl.setTarget(0);
			intakeToggle = false;
			ballTriggerBottom = true;
		}

		// get new press from either side of top, ensure intake is on and there is no ball already there to remove false positives
		if ((triggerTL.get_new_press() || triggerTR.get_new_press()) && intakeToggle == true && ballTriggerTop == false)
		{
			intakeControl.setTarget(0);
			intakeToggle = false;
			ballTriggerTop = true;
		}

		// make sure to update values on sensor state change
		// ensure that neither sensor is pushed, if so, tell the bot that the ball is no longer in position
		if (!triggerBL.get_value() && !triggerBR.get_value())
		{
			ballTriggerBottom = false;
		}
		if (!triggerTL.get_value() && !triggerTR.get_value())
		{
			ballTriggerTop = false;
		}
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
			if (flywheelIterate > FLY_PRESETS_LEN)
			{
				flywheelIterate = FLY_PRESETS_LEN;
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
		cycles++;
		pros::delay(20);
	}
}
