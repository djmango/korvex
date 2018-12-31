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

// TODO: make user controls more ez pz
// TODO: pathfinder auton

// module constants

// 5 for 5 shooting positions, close, middle, platform, full, and cross
// 3 for 0, second, and third flag
const int FLY_PRESETS[5][3] = {
	{0, 600, 600}, // close
	{0, 420, 600}, // middle
	{0, 400, 490}, // platform
	{0, 470, 540}, // full
	{0, 490, 580}  // cross
};
const int FLY_PRESETS_LEN = 2;

// lift position presets, different heights for different poles
// lift presets respectivley: rest, [0] low intake, low stack, [1] high intake, high stack
// intaking the cap means we want the claw level with the cap, stacking it means it needs to be a bit above
const int LIFT_PRESETS[2][3] = {
	{0, 800, 1400}, // low pole
	{0, 1400, 2100} // high pole
};
const int LIFT_PRESETS_LEN = 2; // 0 is the first iterate
const int LIFT_MAX_VEL = 150;

// globals
int flywheelTarget = 0;
int flywheelOutput = 0;

// im lazy
pros::Motor flywheelMotor1(FLY_MTR1, pros::E_MOTOR_GEARSET_06, true);
pros::Motor flywheelMotor2(FLY_MTR2, pros::E_MOTOR_GEARSET_06, true);

bool isFlySpunUp = false;
void isFlySpunUpCheck(void *)
{
	int err = 0;
	int averageErr = 0;
	int cycles = 0;
	while (true)
	{
		cycles++;
		err = flywheelTarget - flywheelMotor1.get_actual_velocity();
		averageErr = (averageErr + err) / 2;
		// printf("avg fly err %d\n", averageErr);
		// std::cout << "\nerr: " << err;
		// std::cout << "\navg err: " << averageErr;

		if (cycles >= 40 || flywheelTarget == 0) // every 2 seconds, or whenever we arent trying to spin
		{
			averageErr = 0;
			cycles = 0;
		}

		// if the average error from the past half second is below 10, we are good
		// how could it be spun up if its not supposed to be spinning?
		if (abs(averageErr) < 5 && flywheelTarget != 0)
		{
			// printf("we spun up\n");
			isFlySpunUp = true;
		}
		else
		{
			isFlySpunUp = false;
		}
		pros::delay(50);
	}
}

void opcontrol()
{

	// factory constructs
	auto chassis = okapi::ChassisControllerFactory::create(
		{LEFT_MTR1, LEFT_MTR2},				  // Left motors
		{-RIGHT_MTR1, -RIGHT_MTR2},			  // Right motors
		okapi::AbstractMotor::gearset::green, // Normal gearset
		{4_in, 12.5_in}						  // 4 inch wheels, 12.5 inch wheelbase width
	);

	auto chassisControllers = okapi::AsyncControllerFactory::motionProfile(
		1.0,	// Maximum linear velocity of the Chassis in m/s
		2.0,	// Maximum linear acceleration of the Chassis in m/s/s
		10.0,   // Maximum linear jerk of the Chassis in m/s/s/s
		chassis // Chassis Controller
	);

	pros::Motor liftMotor(LIFT_MTR, pros::E_MOTOR_GEARSET_18, false);
	int liftIterate = 0;
	int liftPosition = 0; // 0 = low pole, 1 = high pole
	int liftTarget = 0;   // this worked well for flywheel so why not

	// flywheel task startup
	flywheelMotor1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	flywheelMotor2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	int flywheelIterate = 0;
	int flyArmed = 0;		  // 0 = not armed, 1 is one ball shoot, 2 is two ball shoot
	int shootingPosition = 0; // 0 = close, 1 = mid, 2 = platform, 3 = full, 4 = cross
	// pros::Task flyPIDLoop(flyPID, (void *)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "FlywheelPID");
	pros::Task isFlySpunUpCheckLoop(isFlySpunUpCheck, (void *)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "FlywheelSpunUpCheck");

	// intake controller
	pros::Motor intakeMotor(INTAKE_MTR, pros::E_MOTOR_GEARSET_18, true);
	intakeMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	bool intakeToggle = false;		// for user toggle
	bool ballTriggerBottom = false; // these are for detecting if the intake
	bool ballTriggerTop = false;	// triggers are activated

	// intake triggers
	pros::ADIDigitalIn triggerBL(TRIGGER_BL);
	pros::ADIDigitalIn triggerBR(TRIGGER_BR);
	pros::ADIDigitalIn triggerTL(TRIGGER_TL);
	pros::ADIDigitalIn triggerTR(TRIGGER_TR);

	// claw controller
	pros::Motor clawMotor(CLAW_MTR, pros::E_MOTOR_GEARSET_18, false);
	clawMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	okapi::Controller controller;
	pros::Controller controllerPros(pros::E_CONTROLLER_MASTER); // default api for more functions

	// we got time
	int cycles = 0;		// cycle counter
	int cyclesHold = 0; // temp thing for counting
	int flywheeLastTarg = 0;
	int flywheelOffCycle = 0;
	while (true)
	{
		// lift control

		// position changer
		if (controllerPros.get_digital_new_press(DIGITAL_LEFT))
		{
			if (liftPosition != 1)
			{
				liftPosition = 1; // this statement is legit foolproof idk how it'd break
				controllerPros.print(2, 0, "High Pole");
			}
			else
			{
				liftPosition = 0;
				controllerPros.print(2, 0, "Low Pole");
			}
		}

		// goin thru presets
		if (controllerPros.get_digital_new_press(DIGITAL_UP)) // if we get a new up press
		{
			liftIterate++;
			if (liftIterate >= LIFT_PRESETS_LEN) // dont wanna go above
			{
				liftIterate = LIFT_PRESETS_LEN;
			}
			liftTarget = LIFT_PRESETS[liftPosition][liftIterate];
			controllerPros.print(0, 0, "Lift Tar: %d", liftTarget);
		}
		else if (controllerPros.get_digital_new_press(DIGITAL_DOWN)) // if we get a new down press and we are not at min preset
		{
			liftIterate--;

			// if we are at minimum, allow the robot to go further if spammed
			if (liftIterate < 0)
			{
				liftIterate = 0;
				if (liftTarget <= 0)
				{
					liftTarget = liftTarget - 20;
				}
				else
				{
					liftTarget = 0;
				}
				std::cout << "\n tar" << liftTarget;
			}

			// otherwise normal preset operation
			else
			{
				liftTarget = LIFT_PRESETS[liftPosition][liftIterate];
			}
			controllerPros.print(0, 0, "Lift Tar: %d", liftTarget);
		}

		// intake control

		// check sensors
		// get new press from either side of bottom, ensure intake is on and there is no ball already there to remove false positives
		// we also do not want to stop the intake if theres no ball at the top, as the default position should be top
		if ((triggerBL.get_new_press() || triggerBR.get_new_press()) && intakeToggle == true && ballTriggerBottom == false && ballTriggerTop == true)
		{
			// printf("bot triggered\n");
			controllerPros.print(2, 0, "Ball Bot");
			controllerPros.rumble(". .");

			// if theres a ball at the top, we want to pull it down back to the trigger
			intakeMotor.move_velocity(-200);
			// this is kinda a fuck it solution, just pulls down until we dont trigger the bot
			intakeToggle = false;
			ballTriggerBottom = true;
		}

		// get new press from either side of top, ensure intake is on and there is no ball already there to remove false positives
		if ((triggerTL.get_new_press() || triggerTR.get_new_press()) && intakeToggle == true && ballTriggerTop == false)
		{
			// printf("top triggered\n");
			controllerPros.print(2, 0, "Ball Top");
			controllerPros.rumble(". -");
			intakeMotor.move_velocity(0);
			intakeToggle = false;
			ballTriggerTop = true;
		}

		// make sure to update values on sensor state change
		// ensure that neither sensor is pushed, if so, tell the bot that the ball is no longer in position
		if (!triggerBL.get_value() && !triggerBR.get_value() && intakeToggle == false)
		{
			// printf("bot not triggered\n");
			ballTriggerBottom = false;
		}
		if (!triggerTL.get_value() && !triggerTR.get_value() && intakeToggle == false)
		{
			// printf("top not triggered\n");
			ballTriggerTop = false;
		}

		// actual user control
		if (controllerPros.get_digital_new_press(DIGITAL_L1)) // toggle on intake
		{
			if (intakeToggle == true)
			{
				intakeMotor.move_velocity(0);
				intakeToggle = false;
			}
			else
			{
				intakeMotor.move_velocity(200);
				intakeToggle = true;
			}
		}
		if (controllerPros.get_digital(DIGITAL_L2)) // reverse intake
		{
			intakeMotor.move_velocity(-200);
			intakeToggle = false;
		}
		else if (intakeToggle == false && ballTriggerBottom == false && ballTriggerTop == false)
		{
			intakeMotor.move_velocity(0);
		}

		// auto shoot thingy
		if (controllerPros.get_digital_new_press(DIGITAL_B))
		{
			flyArmed = 0;
			controllerPros.print(0, 0, "Fly Not Armed");
		}
		if (controllerPros.get_digital_new_press(DIGITAL_A))
		{
			controllerPros.print(0, 0, "Shooting 1 Ball When Spun Up..");
			flyArmed = 1;
		}
		if (controllerPros.get_digital_new_press(DIGITAL_X))
		{
			controllerPros.print(0, 0, "Shooting 2 Balls When Spun Up..");
			flyArmed = 2;
		}

		// shooting position toggler
		if (controllerPros.get_digital_new_press(DIGITAL_Y))
		{

			switch (shootingPosition) // here we look at the current position, and change it to the next in line, if max is hit we reset
			{
			case 0: // close up, change to mid
				shootingPosition++;
				controllerPros.print(2, 0, "Pos %d: Mid", shootingPosition);
				break;

			case 1: // mid, change to platform
				shootingPosition++;
				controllerPros.print(2, 0, "Pos %d: Plat", shootingPosition);
				break;

			case 2: // platform, change to full
				shootingPosition++;
				controllerPros.print(2, 0, "Pos %d: Full", shootingPosition);
				break;

			case 3: // full, change to cross
				shootingPosition++;
				controllerPros.print(2, 0, "Pos %d: Cross", shootingPosition);
				break;

			case 4: // cross, change to close up
				// reset to 0 cuz we hit max
				shootingPosition = 0;
				controllerPros.print(2, 0, "Pos %d: Close", shootingPosition);
				break;

			default: // this should never run but im a meh dev so just in case
				// reset to 0 cuz we hit something weird
				shootingPosition = 0;
				controllerPros.print(2, 0, "Pos %d: Close", shootingPosition);
				break;
			}

			// since we are moving shooting positions, turn off flywheel
			flyArmed = 0;
			controllerPros.print(0, 0, "Fly Not Armed");
			flywheelIterate = 0;
			flywheelTarget = FLY_PRESETS[shootingPosition][flywheelIterate];
		}

		// flywheel control
		if (controllerPros.get_digital_new_press(DIGITAL_R1))
		{
			flywheelIterate++;
			if (flywheelIterate >= FLY_PRESETS_LEN)
			{
				flywheelIterate = FLY_PRESETS_LEN;
			}
			flywheelTarget = FLY_PRESETS[shootingPosition][flywheelIterate];
			controllerPros.print(1, 0, "%d, Tar: %d", flywheelIterate, flywheelTarget);
		}
		else if (controllerPros.get_digital_new_press(DIGITAL_R2))
		{
			flywheelIterate--;
			if (flywheelIterate < 0)
			{
				flywheelIterate = 0;
			}
			flywheelTarget = FLY_PRESETS[shootingPosition][flywheelIterate];
			controllerPros.print(1, 0, "%d, Tar: %d", flywheelIterate, flywheelTarget);
		}

		// these are async so all user input is ignored
		if (flyArmed == 1 && isFlySpunUp == true && flywheelTarget != 0) // shoot one ball
		{
			chassis.tank(0, 0);
			intakeMotor.move_velocity(200);
			intakeToggle = true;
			ballTriggerTop = false; // we are shooting this ball so its gone

			// if there is a ball on bottom
			// we should keep the intake going until the bottom ball goes to top pos
			if (ballTriggerBottom == true)
			{
				cyclesHold = cycles;
				while (isFlySpunUp == false && !(cyclesHold + 50 < cycles))
				{
					// to make sure we dont get stuck
					cycles++;
					pros::delay(20);
				}
			}
			else
			{
				pros::delay(500);
			}
			// disarm flywheel
			flyArmed = 0;
			controllerPros.print(0, 0, "Fly Not Armed");
			intakeMotor.move_velocity(0);
			intakeToggle = false;
			flywheelIterate = 0;
			flywheelTarget = FLY_PRESETS[shootingPosition][flywheelIterate];
		}
		if (flyArmed == 2 && isFlySpunUp == true && flywheelTarget != 0) // lower then upper
		{
			// freeze chassis
			flywheelTarget = FLY_PRESETS[shootingPosition][1];
			flywheelMotor1.move_velocity(flywheelTarget);
			flywheelMotor2.move_velocity(flywheelTarget);
			chassis.tank(0, 0);
			intakeMotor.move_velocity(200);
			// wait for first ball to get shot
			pros::delay(500);
			isFlySpunUp = false;
			intakeMotor.move_velocity(0);
			controllerPros.print(0, 0, "Shot 1st ball..");
			// increase  flywheel power
			flywheelTarget = FLY_PRESETS[shootingPosition][2];
			flywheelMotor1.move_velocity(flywheelTarget);
			flywheelMotor2.move_velocity(flywheelTarget);
			// wait for spinup
			pros::delay(1000);
			cyclesHold = cycles;
			while (isFlySpunUp == false && !(cyclesHold + 10 < cycles))
			{
				// to make sure we dont get stuck
				cycles++;
				pros::delay(100);
			}
			// shoot 2nd ball
			intakeMotor.move_velocity(200);
			// wait for second ball to get shot
			pros::delay(500);

			// cleanup
			ballTriggerTop = false; // we are shooting the balls so they gone
			ballTriggerBottom = false;
			// disarm the flywheel
			flyArmed = 0;
			controllerPros.print(0, 0, "Fly Not Armed");
			intakeMotor.move_velocity(0);
			flywheelIterate = 0;
			flywheelTarget = FLY_PRESETS[shootingPosition][flywheelIterate];
		}

		// debug
		// std::cout << "\nMotor Position: " << liftMotor.get_position();
		// std::cout << "\nfly: " << flywheelMotor1.get_actual_velocity();

		// update motors
		flywheelMotor1.move_velocity(flywheelTarget); // this is a temp solution, works well enough for me
		flywheelMotor2.move_velocity(flywheelTarget);

		// use last fly targ, if not 0 and current is 0 then we are stopping so initiate the lift halt
		liftMotor.move_absolute(liftTarget, LIFT_MAX_VEL);
		if (flywheeLastTarg != 0 && flywheelTarget == 0)
		{							   // the flywheel just got set to 0
			flywheelOffCycle = cycles; // store when we turned the flywheel off
		}

		if (flywheelOffCycle > cycles - 300 && (triggerTL.get_value() || triggerTR.get_value()))
		{ // stop the intake if flywheel is spinning down
			// 50 is temp, do whatever the stop time is in miliseconds for flywheel divided by 20
			intakeMotor.move_velocity(0);
		}

		flywheeLastTarg = flywheelTarget;

		// chassis control
		chassis.tank(controller.getAnalog(ControllerAnalog::leftY),
					 controller.getAnalog(ControllerAnalog::rightY));
		cycles++;
		pros::delay(20);
	}
}
