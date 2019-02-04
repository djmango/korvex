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

// 5 for 5 shooting positions, close, middle, platform, full, and cross
// 3 for 0, second, and third flag
const int FLY_PRESETS[5][3] = {
	{0, 600, 490}, // close (inveted cuz anish wanted it)
	{0, 500, 600}, // middle
	{0, 480, 490}, // platform
	{0, 470, 540}, // full
	{0, 490, 580}  // cross
};
const int FLY_PRESETS_LEN = 3; // lowered cuz big boy driver onl used 3

// lift position presets, different heights for different poles
// lift presets respectivley: rest, [0] low intake, low stack, [1] high intake, high stack
// intaking the cap means we want the claw level with the cap, stacking it means it needs to be a bit above
const int LIFT_PRESETS[2][3] = {
	{0, 700, 1300}, // low pole
	{0, 1400, 2100} // high pole
};
const int LIFT_PRESETS_LEN = 2; // 0 is the first iterate

const int CAPFLIP_PRESETS[4] = {
	0, -445, -560, -700};
const int CAPFLIP_PRESETS_LEN = 3;

// globals
int flywheelTarget = 0;
int flywheelOutput = 0;

bool isFlySpunUp = false;
void isFlySpunUpCheck(void *)
{
	int err = 0;
	int averageErr = 0;
	int cycles = 0;
	while (true)
	{
		cycles++;
		err = flywheelTarget - flywheelMotor.get_actual_velocity();
		averageErr = (averageErr + err) / 2;
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
	// lift stuff
	int liftIterate = 0;  // whichever preset-set (shooting position) we are on
	int liftPosition = 0; // 0 = low pole, 1 = high pole
	int liftTarget = 0;   // this worked well for flywheel so why

	// capflip stuff
	int capflipTarget = 0;
	int capflipIterate = 0;

	// descore stuff
	descoreMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	int descoreRelease = false; // holder for check if we are currently not using descore mech, meaning it must stay in current pos

	// flywheel task startup
	flywheelMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	int flywheelIterate = 0;  // whichever preset-set (shooting position) we are on
	int flyArmed = 0;		  // 0 = not armed, 1 is one ball shoot, 2 is two ball shoot
	int shootingPosition = 0; // 0 = close, 1 = mid, 2 = platform, 3 = full, 4 = cross
	pros::Task isFlySpunUpCheckLoop(isFlySpunUpCheck, (void *)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "FlywheelSpunUpCheck");

	// intake stuff
	intakeMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	bool intakeToggle = false;		// for user toggle
	bool ballTriggerBottom = false; // these are for detecting if the intake
	bool ballTriggerTop = false;	// triggers are activated

	// check for balls
	if (triggerBL.get_new_press() || triggerBR.get_new_press())
	{
		ballTriggerBottom = true;
	}
	if (triggerTL.get_new_press() || triggerTR.get_new_press())
	{
		ballTriggerTop = true;
	}

	// we got time
	int cycles = 0;			  // cycle counter
	int cyclesHold = 0;		  // temp thing for counting
	int flywheeLastTarg = 0;  // the last target we had the flywheel set to
	int flywheelOffCycle = 0; // the cycle when we turned the flywheel off

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

		// capflip control
		if (controllerPros.get_digital_new_press(DIGITAL_RIGHT)) // new cycle press, go to next preset
		{
			capflipIterate++;
			if (capflipIterate >= CAPFLIP_PRESETS_LEN) // dont wanna go above
			{
				capflipIterate = CAPFLIP_PRESETS_LEN;
			}
			capflipTarget = CAPFLIP_PRESETS[capflipIterate];
			controllerPros.print(0, 0, "Cap Tar: %d", capflipTarget);
			capflipMotor.move_absolute(capflipTarget, 150);
		}
		else if (controllerPros.get_digital_new_press(DIGITAL_Y)) // reset capflip
		{
			capflipIterate = 0;
			capflipMotor.move_absolute(0, 150);
		}

		// descore control
		if (controllerPros.get_digital(DIGITAL_UP)) // if we get a new up press
		{
			descoreMotor.move_velocity(200);
			descoreRelease = false;
		}
		else if (controllerPros.get_digital(DIGITAL_DOWN)) // if we get a new down press
		{
			descoreMotor.move_velocity(-200);
			descoreRelease = false;
		}
		else if (descoreRelease == false)
		{
			descoreRelease = true;
			descoreMotor.move_absolute(descoreMotor.get_position(), 100);
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

		// intake
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

		// auto shoot controller (not the actual functions btw you should switch it to functions cuz rn it sucks so uh TODO: do the thing)
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
		if (controllerPros.get_digital_new_press(DIGITAL_B))
		{

			switch (shootingPosition) // here we look at the current position, and change it to the next in line, if max is hit we reset
			{
			case 0: // close up, change to mid
				shootingPosition++;
				controllerPros.print(2, 0, "Pos %d: Mid  ", shootingPosition);
				break;

			case 1: // mid, change to platform
				shootingPosition++;
				controllerPros.print(2, 0, "Pos %d: Plat ", shootingPosition);
				break;

			case 2: // platform, change to full
				shootingPosition++;
				controllerPros.print(2, 0, "Pos %d: Full ", shootingPosition);
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
			// flywheelIterate = 2;
			// flywheelTarget = FLY_PRESETS[shootingPosition][flywheelIterate];
			flywheelTarget = 600;
			flywheelMotor.move_velocity(flywheelTarget);
			chassis.tank(0, 0);
			// wait for spinup for first shot
			while (isFlySpunUp == false && (cyclesHold + 25 > cycles))
			{
				// to make sure we dont get stuck
				chassis.tank(controller.getAnalog(ControllerAnalog::leftY),
							 controller.getAnalog(ControllerAnalog::rightY));
				cycles++;
				pros::delay(20);
			}
			intakeMotor.move_velocity(200);
			// wait for first ball to get shot
			cyclesHold = cycles;
			// im replacing the delays with delay-loops to allow chassis control
			while (cyclesHold + 20 > cycles)
			{
				chassis.tank(controller.getAnalog(ControllerAnalog::leftY),
							 controller.getAnalog(ControllerAnalog::rightY));
				cycles++;
				pros::delay(20);
			}

			isFlySpunUp = false;
			intakeMotor.move_velocity(0);
			controllerPros.print(0, 0, "Shot 1st ball..");

			// spindown
			// flywheelMotor.move_velocity(-600);
			// cyclesHold = cycles;
			// while (cyclesHold + 30 > cycles)
			// {
			// 	chassis.tank(controller.getAnalog(ControllerAnalog::leftY),
			// 				 controller.getAnalog(ControllerAnalog::rightY));
			// 	cycles++;
			// 	pros::delay(20);
			// }

			// change flywheel power
			// flywheelIterate = 1;
			// flywheelTarget = FLY_PRESETS[shootingPosition][flywheelIterate];
			flywheelTarget = 490;
			flywheelMotor.move_velocity(flywheelTarget);

			// wait for spinup
			cyclesHold = cycles;
			while (isFlySpunUp == false && (cyclesHold + 70 > cycles))
			{
				// to make sure we dont get stuck
				chassis.tank(controller.getAnalog(ControllerAnalog::leftY),
							 controller.getAnalog(ControllerAnalog::rightY));
				cycles++;
				pros::delay(20);
			}
			// shoot 2nd ball
			intakeMotor.move_velocity(200);
			// wait for second ball to get shot
			chassis.tank(0, 0);
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

		// chassis control
		chassis.tank(controller.getAnalog(ControllerAnalog::leftY),
					 controller.getAnalog(ControllerAnalog::rightY));

		// chassis freeze combo
		if (controller.getDigital(ControllerDigital::down) && controller.getDigital(ControllerDigital::Y))
		{ // if both bottom buttons pushed
			// freeeeze
			chassis.moveDistanceAsync(0);
		}

		// debug
		// std::cout << "fly: " << flywheelMotor.get_actual_velocity() << std:cout:endl;
		// std::cout << chassis.getSensorVals()[0] << std::endl;
		// std::cout << chassis.getSensorVals()[1] << std::endl;
		std::cout << capflipMotor.get_position() << std::endl;

		// update motors
		flywheelMotor.move_velocity(flywheelTarget); // this is a temp solution, works well enough for me
		// liftControllerPID.setTarget(liftTarget);

		// prevent flywheel jams
		// use last fly targ, if not 0 and current is 0 then we are stopping so initiate the intake halt
		if (flywheeLastTarg != 0 && flywheelTarget == 0)
		{							   // the flywheel just got set to 0
			flywheelOffCycle = cycles; // store when we turned the flywheel off
		}

		if (flywheelOffCycle > cycles - 50 && (triggerTL.get_value() || triggerTR.get_value()) && cycles > 100) // cycles over 100 bcuz false positive at start
		{																										// stop the intake if flywheel is spinning down
			// 50 is temp, do whatever the stop time is in miliseconds for flywheel divided by 20
			intakeMotor.move_velocity(0);
		}

		// storage of whatever
		flywheeLastTarg = flywheelTarget;

		cycles++;
		pros::delay(20);
	}
}
