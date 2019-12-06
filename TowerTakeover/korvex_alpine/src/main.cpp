#include "main.h"
#include "korvexlib.h"
#include "okapi/api.hpp"

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

// base global defenitions
int autonSelection = 42; // hitchhikers anyone?

void telemetry() {
	// update
}

static lv_res_t redBtnmAction(lv_obj_t *btnm, const char *txt)
{
	printf("red button: %s released\n", txt);

	if (txt == "Left")
	{
		autonSelection = -1; // or whatever red left is
	}
	if (txt == "Right")
	{
		autonSelection = -2;
	}
	if (txt == "Rick")
	{
		autonSelection = -3;
	}

	return LV_RES_OK; // return OK because the button matrix is not deleted
}

static lv_res_t blueBtnmAction(lv_obj_t *btnm, const char *txt)
{
	printf("blue button: %s released\n", txt);

	if (txt == "Right")
	{
		autonSelection = 1;
	}
	if (txt == "Left")
	{
		autonSelection = 2;
	}
	if (txt == "Rick")
	{
		autonSelection = 3;
	}

	return LV_RES_OK;
}

static lv_res_t skillsBtnAction(lv_obj_t *btn)
{
	autonSelection = 0;
	return LV_RES_OK;
}

/*Create a button descriptor string array*/
static const char *btnmMap[] = {"Left", "Right", "Rick", ""};

void initialize()
{
	// lvgl theme

	lv_theme_t *th = lv_theme_alien_init(280, NULL); //Set a HUE value and keep font default
	lv_theme_set_current(th);

	// create a tab view object
	lv_obj_t *tabview;
	tabview = lv_tabview_create(lv_scr_act(), NULL);

	// add 4 tabs (the tabs are page (lv_page) and can be scrolled
	lv_obj_t *redTab = lv_tabview_add_tab(tabview, "Red");
	lv_obj_t *blueTab = lv_tabview_add_tab(tabview, "Blue");
	lv_obj_t *skillsTab = lv_tabview_add_tab(tabview, "Skills");
	lv_obj_t *telemetryTab = lv_tabview_add_tab(tabview, "Telemetry");

	// add content to the tabs

	// red tab

	// button matrix
	lv_obj_t *redBtnm = lv_btnm_create(redTab, NULL);
	lv_btnm_set_map(redBtnm, btnmMap);
	lv_btnm_set_action(redBtnm, redBtnmAction);
	lv_obj_set_size(redBtnm, 450, 50);
	lv_obj_set_pos(redBtnm, 0, 100);
	lv_obj_align(redBtnm, NULL, LV_ALIGN_CENTER, 0, 0);

	// blue tab
	lv_obj_t *blueBtnm = lv_btnm_create(blueTab, NULL);
	lv_btnm_set_map(blueBtnm, btnmMap);
	lv_btnm_set_action(blueBtnm, blueBtnmAction);
	lv_obj_set_size(blueBtnm, 450, 50);
	lv_obj_set_pos(blueBtnm, 0, 100);
	lv_obj_align(blueBtnm, NULL, LV_ALIGN_CENTER, 0, 0);

	// skills tab
	lv_obj_t *skillsBtn = lv_btn_create(skillsTab, NULL);
	lv_obj_t *label = lv_label_create(skillsBtn, NULL);
	lv_label_set_text(label, "Skills");
	lv_btn_set_action(skillsBtn, LV_BTN_ACTION_CLICK, skillsBtnAction);
	// lv_btn_set_state(skillsBtn, LV_BTN_STATE_TGL_REL);
	lv_obj_set_size(skillsBtn, 450, 50);
	lv_obj_set_pos(skillsBtn, 0, 100);
	lv_obj_align(skillsBtn, NULL, LV_ALIGN_CENTER, 0, 0);

	// for telemetry, make a function that updates the vals of temp= or whatever once every x seconds
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

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

int velCap = 100; //velCap limits the change in velocity and must be global
int targetLeft;
int targetRight;

void driveP(){
  chassisLeftBack.tare_position(); //reset base encoders
  chassisRightBack.tare_position();
  int errorLeft;
  int errorRight;

  // the touchables ;))))))))
  float kp = 0.075;
  float kpTurn = 0.2;
  int acc = 5;

  // the untouchables
  int voltageLeft = 0;
  int voltageRight = 0;
  int signLeft;
  int signRight;
  pros::delay(20); // dunno

  while(autonomous){
    errorLeft = targetLeft - chassisLeftBack.get_position(); // error is target minus actual value
    errorRight = targetRight - chassisRightBack.get_position();

    signLeft = errorLeft / abs(errorLeft); // + or - 1
    signRight = errorRight / abs(errorRight);

    if(signLeft == signRight){
      voltageLeft = errorLeft * kp; //intended voltage is error times constant
      voltageRight = errorRight * kp;
    }
    else{
      voltageLeft = errorLeft * kpTurn; //same logic with different turn value
      voltageRight = errorRight * kpTurn;
    }

    velCap = velCap + acc;  //slew rate
    if(velCap > 115){
      velCap = 115; //velCap cannot exceed 115
    }

    if(abs(voltageLeft) > velCap){ //limit the voltage
      voltageLeft = velCap * signLeft;
    }

    if(abs(voltageRight) > velCap){ //ditto
      voltageRight = velCap * signRight;
    }

    chassisLeftFront.move(voltageLeft); //set the motors to the intended speed
    chassisLeftBack.move(voltageLeft);
    chassisRightFront.move(voltageRight);
    chassisRightBack.move(voltageRight);

    pros::delay(20);
  }
}

void drive(int left, int right){
  targetLeft = targetLeft + left;
  targetRight = targetRight + right;
  velCap = 0; //reset velocity cap for slew rate
}

void autonomous() {
	chassisLeftFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	chassisLeftBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	chassisRightFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	chassisRightBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	intakeMotor1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	intakeMotor2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	switch (autonSelection) {
	case 0:
		// skillsss
		break;
	
	default:
		break;
	}
}

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

void opcontrol() {
	// motor setup
	intakeMotor1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	intakeMotor2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	
	// main loop
	while (true) {
		// basic lift control, with an intake reverse at the end of lift
		if (bumperRD.isPressed()) {
			liftMotor.move_voltage(-12000);
		} else if (bumperRU.isPressed()) {
			liftMotor.move_voltage(12000);
		} else if (bumperRD.changedToReleased() and liftMotor.get_position() > 100) {
			intakeMotor1.move_relative(-2000, 100);
			intakeMotor2.move_relative(-2000, 100);
		} else {
			liftMotor.move_voltage(0);
		}

		// basic intake control (maybe leave intake spinning during opcontrol at lower speed?)
		if (bumperLU.isPressed()) {
			intakeMotor1.move_voltage(12000);
			intakeMotor2.move_voltage(12000);
		} else if (bumperLD.isPressed()) {
			intakeMotor1.move_voltage(-12000);
			intakeMotor2.move_voltage(-12000);
		}
		else if (not shift.isPressed() and not (bumperLU.isPressed() or bumperLD.isPressed())) {
			intakeMotor1.move_voltage(0);
			intakeMotor2.move_voltage(0);
		}

		// tray control using shift key
		if (shift.isPressed()) { //shift key
			if (bumperLU.isPressed()) { // tray forward
				trayMotor.move_voltage(-12000);
				intakeMotor1.move_voltage(-12000);
				intakeMotor2.move_voltage(-12000);
			} else if (bumperLD.isPressed()) { // tray backward
				trayMotor.move_voltage(12000);
				// reverses rollers automatically
				intakeMotor1.move_voltage(12000);
				intakeMotor2.move_voltage(12000);
			}
		}
		else {
			trayMotor.move_voltage(0);
		}

		//drive control
		chassisLeftFront.move(masterController.getAnalog(okapi::ControllerAnalog::leftY) * 127);
		chassisLeftBack.move(masterController.getAnalog(okapi::ControllerAnalog::leftY) * 127);
		chassisRightFront.move(masterController.getAnalog(okapi::ControllerAnalog::rightY) * 127);
		chassisRightBack.move(masterController.getAnalog(okapi::ControllerAnalog::rightY) * 127);
		pros::delay(20);
	}
}