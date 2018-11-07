#include "main.h"

using namespace pros::literals;

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

void opcontrol()
{
	// declerations
	pros::Controller master(pros::E_CONTROLLER_MASTER);

	// chassis motor declerations
	pros::Motor leftMtr1(1);
	pros::Motor leftMtr2(2);
	pros::Motor rightMtr1(3);
	pros::Motor rightMtr2(4);

	// lift and claw motor declerations
	pros::Motor liftMtr(5);
	pros::Motor clawMtr(6);

	// flywheel and intake motors
	pros::Motor flyMtr(7);
	pros::Motor intakeMtr(8);

	// declare motor speed holders, for lack of a better term
	int left = 0;
	int right = 0;
	int lift = 0;
	int claw = 0;
	int fly = 0;
	int intake = 0;

	// additional motor options
	liftMtr.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD); // makes motor hold position
	while (true)
	{
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
						 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
						 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);
		int left = master.get_analog(ANALOG_LEFT_Y);
		int right = master.get_analog(ANALOG_RIGHT_Y);

		// lift control
		if (master.get_digital(DIGITAL_UP))
		{
			lift = 127;
		}
		else if (master.get_digital(DIGITAL_DOWN)){
			lift = -127;
		}
		else {
			lift = 0;
		}

		// set motor speeds
		leftMtr1.move(left);
		leftMtr2.move(left);
		rightMtr1.move(right);
		rightMtr2.move(right);
		liftMtr.move(lift);
		clawMtr.move(claw);
		flyMtr.move(fly);
		intakeMtr = intake;
		pros::delay(20);
	}
}
