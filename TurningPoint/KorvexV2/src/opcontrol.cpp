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
const int FLY_PRESETS[2][3] = {
	{0, 580, 430}, // close
	{0, 520, 480}, // full
};
const int FLY_PRESETS_LEN = 2; // make sure we dont go over our set length

const int CAPFLIP_PRESETS[4] = {
	0, -450, -565, -700};
const int CAPFLIP_PRESETS_LEN = 3;

// globals
int flywheelTarget = 0;
int flywheelAvgErr = 0;

bool isFlySpunUp = false;
void isFlySpunUpCheck(void *)
{
	int err = 0;
	int averageErr = 0;
	int cycles = 0;
	while (true)
	{
		cycles++;
		err = flywheelTarget - flywheelController.getActualVelocity();
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
		flywheelAvgErr = averageErr; // give global this so we can make sure we arent burning the flywheel
		pros::delay(50);
	}
}

void opcontrol()
{

	// capflip stuff
	int capflipTarget = 0;
	int capflipIterate = 0;

	// flywheel task startup
	flywheelController.setGearing(okapi::AbstractMotor::gearset::blue);
	flywheelController.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
	int flywheelIterate = 0;  // whichever preset-set (shooting position) we are on
	int flyArmed = 0;		  // 0 = not armed, 1 is one ball shoot, 2 is two ball shoot
	int shootingPosition = 0; // 0 = close, 1 = full
	pros::Task isFlySpunUpCheckLoop(isFlySpunUpCheck, (void *)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "FlywheelSpunUpCheck");

	// intake stuff
	intakeMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	bool intakeToggle = false;		// for user toggle
	bool ballTriggerBottom = false; // these are for detecting if the intake
	bool ballTriggerTop = false;	// triggers are activated

	// we got time
	int cycles = 0;			  // cycle counter
	int cyclesHold = 0;		  // temp thing for counting
	int flywheelLastTarg = 0; // the last target we had the flywheel set to
	int flywheelOffCycle = 0; // the cycle when we turned the flywheel off

	while (true)
	{

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

		// intake control

		// check sensors
		// get new press from either side of bottom, ensure intake is on and there is no ball already there to remove false positives
		// we also do not want to stop the intake if theres no ball at the top, as the default position should be top
		if ((triggerBL.get_new_press() || triggerBR.get_new_press()) && intakeToggle == true && ballTriggerBottom == false && ballTriggerTop == true)
		{
			// printf("bot triggered\n");
			controllerPros.rumble("-");

			// if theres a ball at the top we want to pull it down back to the trigger
			// let the bot ball go a little higher
			intakeMotor.move_relative(400, 200);
			cyclesHold = cycles;
			while (cyclesHold + 20 > cycles)
			{
				chassis.tank((controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y) * 0.00787401574),
							 controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) * 0.00787401574);
				cycles++;
				pros::delay(20);
			}
			intakeMotor.move_relative(-500, 200);

			// start flywheel and the ball gets like 'sucked down' so we need to pull it down a bit further than the normal loop would allow
			cyclesHold = cycles;
			while (cyclesHold + 30 > cycles)
			{
				chassis.tank((controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y) * 0.00787401574),
							 controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) * 0.00787401574);
				cycles++;
				pros::delay(20);
			}
			flywheelIterate = 1;
			flywheelTarget = FLY_PRESETS[shootingPosition][flywheelIterate];
			flywheelController.moveVelocity(flywheelTarget);
			intakeToggle = false;
			ballTriggerBottom = true;
		}

		// get new press from either side of top, ensure intake is on and there is no ball already there to remove false positives
		if ((triggerTL.get_new_press() || triggerTR.get_new_press()) && intakeToggle == true && ballTriggerTop == false)
		{
			// printf("top triggered\n");
			controllerPros.rumble(".");
			intakeMotor.move_velocity(0);
			intakeToggle = false;
			ballTriggerTop = true;
		}

		// make sure to update values on sensor state change
		// ensure that neither sensor is pushed, if so, tell the bot that the ball is no longer in position
		if (!triggerBL.get_value() && !triggerBR.get_value() && intakeToggle == false)
		{
			ballTriggerBottom = false;
		}
		if (!triggerTL.get_value() && !triggerTR.get_value() && intakeToggle == false)
		{
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

		// flywheel preset switcher (revamped again)
		if (controllerPros.get_digital_new_press(DIGITAL_LEFT)) // toggle fly preset
		{
			if (shootingPosition != 0)
			{ // set to close shot
				shootingPosition = 0;
				controllerPros.rumble("...");
			}
			else
			{ // set to full court
				shootingPosition = 1;
				controllerPros.rumble("--");
			}
		}

		// auto shoot controller (not the actual functions btw you should switch it to functions cuz rn it sucks so uh TODO: do the thing)
		if (controllerPros.get_digital_new_press(DIGITAL_B)) // macro time
		{
			flyArmed = 3;
		}
		if (controllerPros.get_digital_new_press(DIGITAL_A)) // shooting 1 ball
		{
			flyArmed = 1;
		}
		if (controllerPros.get_digital_new_press(DIGITAL_X)) // shooting 2 balls
		{
			flyArmed = 2;
		}

		// flywheel control (switch high or mid flag)
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
				pros::delay(300);
			}
			// disarm flywheel
			flyArmed = 0;
			intakeMotor.move_velocity(0);
			intakeToggle = false;
			flywheelIterate = 0;
			flywheelTarget = FLY_PRESETS[shootingPosition][flywheelIterate];
		}
		if (flyArmed == 2 && isFlySpunUp == true && flywheelTarget != 0) // upper then lower
		{
			flywheelIterate = 1;
			flywheelTarget = FLY_PRESETS[shootingPosition][flywheelIterate];
			flywheelController.moveVelocity(flywheelTarget);
			chassis.tank(0, 0);
			// wait for spinup for first shot
			cyclesHold = cycles;
			while (isFlySpunUp == false && (cyclesHold + 25 > cycles))
			{
				// to make sure we dont get stuck
				chassis.tank((controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y) * 0.00787401574),
							 controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) * 0.00787401574);
				cycles++;
				pros::delay(20);
			}
			intakeMotor.move_relative(2000, 200);
			// wait for first ball to get shot
			cyclesHold = cycles;
			// im replacing the delays with delay-loops to allow chassis control
			while (cyclesHold + 5 > cycles)
			{
				chassis.tank((controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y) * 0.00787401574),
							 controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) * 0.00787401574);
				cycles++;
				pros::delay(20);
			}
			chassis.tank(0, 0);
			isFlySpunUp = false;
			// shot first ball

			// aggro spindown
			flywheelTarget = -600;
			flywheelController.moveVelocity(flywheelTarget);
			cyclesHold = cycles;
			while (cyclesHold + 10 > cycles)
			{
				chassis.tank((controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y) * 0.00787401574),
							 controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) * 0.00787401574);
				cycles++;
				pros::delay(20);
			}

			// change flywheel power
			flywheelIterate = 2;
			flywheelTarget = FLY_PRESETS[shootingPosition][flywheelIterate];
			flywheelController.moveVelocity(flywheelTarget);
			isFlySpunUp == false;
			pros::delay(200);

			// wait for spinup
			cyclesHold = cycles;
			while (isFlySpunUp == false && (cyclesHold + 50 > cycles))
			{
				// to make sure we dont get stuck
				chassis.tank((controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y) * 0.00787401574),
							 controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) * 0.00787401574);
				cycles++;
				pros::delay(20);
			}
			// shoot 2nd ball
			intakeMotor.move_relative(800, 200);
			// wait for second ball to get shot
			chassis.tank(0, 0);
			pros::delay(200);

			// cleanup
			ballTriggerTop = false; // we are shooting the balls so they gone
			ballTriggerBottom = false;
			// disarm the flywheel
			flyArmed = 0;
			flywheelIterate = 0;
			flywheelTarget = FLY_PRESETS[shootingPosition][flywheelIterate];
		}
		if (flyArmed == 3 && isFlySpunUp == true && flywheelTarget != 0) // both flags 600 macro
		{
			flywheelIterate = 1;
			flywheelTarget = FLY_PRESETS[shootingPosition][flywheelIterate];
			chassis.tank(0, 0);
			// wait for spinup for first shot
			cyclesHold = cycles;
			while (isFlySpunUp == false && (cyclesHold + 25 > cycles))
			{
				// to make sure we dont get stuck
				chassis.tank((controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y) * 0.00787401574),
							 controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) * 0.00787401574);
				cycles++;
				pros::delay(20);
			}
			intakeMotor.move_relative(2000, 200);
			// wait for first ball to get shot
			isFlySpunUp = false;
			pros::delay(100);
			chassis.moveDistance(32_in);

			// shoot 2nd ball
			intakeMotor.move_relative(1000, 200);
			// wait for second ball to get shot
			chassis.tank(0, 0);
			pros::delay(200);

			// cleanup
			ballTriggerTop = false; // we are shooting the balls so they gone
			ballTriggerBottom = false;
			// disarm the flywheel
			flyArmed = 0;
			flywheelIterate = 0;
			flywheelTarget = FLY_PRESETS[shootingPosition][flywheelIterate];
		}

		// debug
		// std::cout << chassis.getSensorVals()[0] << std::endl;
		// std::cout << chassis.getSensorVals()[1] << std::endl;
		// std::cout << capflipMotor.get_position() << std::endl;
		// std::cout << "temp "<< flywheelController.getTemperature() << std::endl;
		// std::cout << "eff "<< flywheelController.getEfficiency() << std::endl;
		// std::cout << "temp " << intakeMotor.get_temperature() << std::endl;
		// std::cout << "eff " << intakeMotor.get_efficiency() << std::endl;
		// if (triggerBR.get_value())
		// {
		// 	printf("BR\n");
		// }
		// if (triggerBL.get_value())
		// {
		// 	printf("BL\n");
		// }
		// if (triggerTR.get_value())
		// {
		// 	printf("TR\n");
		// }
		// if (triggerTL.get_value())
		// {
		// 	printf("TL\n");
		// }

		// chassis control
		chassis.tank((controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y) * 0.00787401574),
					 controllerPros.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) * 0.00787401574);

		// final flywheel stuff
		flywheelController.moveVelocity(flywheelTarget);

		// prevent flywheel jams
		// use last fly targ, if not 0 and current is 0 then we are stopping so initiate the intake halt
		if (flywheelLastTarg != 0 && flywheelTarget == 0)
		{							   // the flywheel just got set to 0
			flywheelOffCycle = cycles; // store when we turned the flywheel off
		}

		if (flywheelOffCycle > cycles - 80 && (triggerTL.get_value() || triggerTR.get_value()) && cycles > 100) // cycles over 100 bcuz false positive at start
		{																										// stop the intake if flywheel is spinning down
			// 50 is temp, do whatever the stop time is in miliseconds for flywheel divided by 20
			intakeMotor.move_velocity(0);
		}

		// storage of whatever
		flywheelLastTarg = flywheelTarget;

		cycles++;
		pros::delay(20);
	}
}
