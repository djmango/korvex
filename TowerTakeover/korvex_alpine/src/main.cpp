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

// create a button descriptor string array
static const char *btnmMap[] = {"Left", "Right", "Rick", ""};

static lv_res_t redBtnmAction(lv_obj_t *btnm, const char *txt)
{
	printf("red button: %s released\n", txt);

	if (txt == "Unprotec")
	{
		autonSelection = -1; // or whatever red left is
	}
	if (txt == "Protec")
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

	if (txt == "Unprotec")
	{
		autonSelection = 1;
	}
	if (txt == "Protec")
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

static lv_res_t ddlist_action(lv_obj_t * ddlist)
{

    char sel_str[32];
    lv_ddlist_get_selected_str(ddlist, sel_str);
    printf("new option: %s \n", sel_str);

    return LV_RES_OK; /*Return OK if the drop down list is not deleted*/
}

void initialize()
{
	// lvgl theme

	// lv_theme_t *th = lv_theme_alien_init(280, NULL); //Set a HUE value and keep font default
	lv_theme_t *th = lv_theme_night_init(210, NULL); //Set a HUE value and a Font for the Night Theme
	lv_theme_set_current(th);

	// create a tab view object
	lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), NULL);

	// add 4 tabs (the tabs are page (lv_page) and can be scrolled
	lv_obj_t *telemetryTab = lv_tabview_add_tab(tabview, "Telemetry");
	lv_obj_t *redTab = lv_tabview_add_tab(tabview, "Red");
	lv_obj_t *blueTab = lv_tabview_add_tab(tabview, "Blue");
	lv_obj_t *skillsTab = lv_tabview_add_tab(tabview, "Skills");

	// telemetry tab

	// drop down list
	lv_obj_t * ddl1 = lv_ddlist_create(lv_scr_act(), NULL);
	lv_ddlist_set_options(ddl1, "Apple\n"
								"Banana\n"
								"Orange\n"
								"Melon\n"
								"Grape\n"
								"Raspberry");
	lv_obj_align(ddl1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 80);
	lv_ddlist_set_sb_mode(ddl1, LV_SB_MODE_ON);
	lv_obj_set_free_num(ddl1, 1);               /*Set a unique ID*/
	lv_ddlist_set_action(ddl1, ddlist_action);

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

	// debug
	lv_obj_t *msgBox = lv_mbox_create(telemetryTab, NULL);
	lv_mbox_set_text(msgBox, "rick from r");
	lv_mbox_set_anim_time(msgBox, 100);
	lv_mbox_start_auto_close(msgBox, 1000);

	// wait for calibrate
	imu.reset();
	pros::delay(100);
	while (imu.is_calibrating())
	{
		std::cout << pros::millis() << ": calibrating imu" << std::endl;
		pros::delay(20);
	}
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

int voltageCap = 100; // voltageCap limits the change in velocity and must be global
int targetLeft;
int targetRight;
int targetTurn;
int targetTurnRelative;

int motorTemps[10];

void driveP(int voltageMax) {
  chassisLeftBack.tare_position(); // reset base encoders
  chassisRightBack.tare_position();
  int errorLeft;
  int errorRight;
 
  // the touchables ;)))))))) touch me uwu :):):)
  float kp = 0.15;
  float acc = 3.5;
  float kpTurn = 0.7;
  float accTurn = 4;

  // the untouchables
  int voltageLeft = 0;
  int voltageRight = 0;
  int signLeft;
  int signRight;
  int errorCurrent = 0;
  int errorLast = 0;
  int sameErrCycles = 0;
  int same0ErrCycles = 0;
  pros::delay(20); // dunno

  while(autonomous){
    errorLeft = targetLeft - chassisLeftBack.get_position(); // error is target minus actual value
    errorRight = targetRight - chassisRightBack.get_position();
	errorCurrent = (abs(errorRight) + abs(errorLeft)) / 2;

    signLeft = errorLeft / abs(errorLeft); // + or - 1
    signRight = errorRight / abs(errorRight);

    if(signLeft == signRight){
      voltageLeft = errorLeft * kp; // intended voltage is error times constant
      voltageRight = errorRight * kp;
	  voltageCap = voltageCap + acc;  // slew rate
    }
    else{
      voltageLeft = errorLeft * kpTurn; // same logic with different turn value
      voltageRight = errorRight * kpTurn;
	  voltageCap = voltageCap + accTurn;  // turn slew rate
    }
    
    if(voltageCap > voltageMax){
      voltageCap = voltageMax; // voltageCap cannot exceed 115
    }

    if(abs(voltageLeft) > voltageCap){ // limit the voltage
      voltageLeft = voltageCap * signLeft;
    }

    if(abs(voltageRight) > voltageCap){ // ditto
      voltageRight = voltageCap * signRight;
    }

	// set the motors to the intended speed
    chassisLeftFront.move(voltageLeft); 
    chassisLeftBack.move(voltageLeft);
    chassisRightFront.move(voltageRight);
    chassisRightBack.move(voltageRight);

	// timeout utility
	if (errorLast == errorCurrent) {
		if (errorLast < 2 and errorCurrent < 2) {
			same0ErrCycles +=1;
		}
		sameErrCycles += 1;
	}
	else {
		sameErrCycles = 0;
		same0ErrCycles = 0;
	}

	// exit paramaters
	if (same0ErrCycles >= 30 or sameErrCycles >= 100) { // allowing for smol error or exit if we stay the same err for 1 second
		chassisLeftFront.move_velocity(0); 
		chassisLeftBack.move_velocity(0);
		chassisRightFront.move_velocity(0);
		chassisRightBack.move_velocity(0);
		std::cout << "task complete with error " << errorCurrent << std::endl;
		return;
	}
	
	// debug
	if (sameErrCycles == 0) {
		std::cout << "error  " << errorCurrent << std::endl;
		std::cout << "voltageLeft  " << voltageLeft << std::endl;
	}

	// nothing goes after this
	errorLast = errorCurrent;
    pros::delay(20);
  }
}

void drive(int left, int right, int voltageMax=115){
  targetLeft = left;
  targetRight = right;
  voltageCap = 0; //reset velocity cap for slew rate
  driveP(voltageMax);
}

void turnP(int voltageMax) {
 
  // the touchables ;)))))))) touch me uwu :):):)
  float kp = 2;
  float acc = 1.6;

  // the untouchables
  int voltage = 0;
  int errorCurrent = 0;
  int errorLast = 0;
  int sameErrCycles = 0;
  int same0ErrCycles = 0;
  int sign;
  double error;
  pros::delay(20); // dunno

  while(autonomous){
    error = targetTurn - imu.get_rotation();
	errorCurrent = abs(error);
	sign = targetTurnRelative / abs(targetTurnRelative); // -1 or 1

	voltage = (error * kp); // intended voltage is error times constant
	voltageCap = voltageCap + acc;  // slew rate
    
    if(voltageCap > voltageMax){
      voltageCap = voltageMax; // voltageCap cannot exceed 115
    }

    if(abs(voltage) > voltageCap){ // limit the voltage
      voltage = voltageCap * sign;
    }

	// set the motors to the intended speed
    chassisLeftFront.move(voltage); 
    chassisLeftBack.move(voltage);
    chassisRightFront.move(voltage * -1);
    chassisRightBack.move(voltage * -1);

	// timeout utility
	if (errorLast == errorCurrent) {
		if (errorLast < 2 and errorCurrent < 2) {
			same0ErrCycles +=1;
		}
		sameErrCycles += 1;
	}
	else {
		sameErrCycles = 0;
		same0ErrCycles = 0;
	}

	// exit paramaters
	if (same0ErrCycles >= 300 or sameErrCycles >= 100) { // allowing for smol error or exit if we stay the same err for 1 second
		chassisLeftFront.move_velocity(0); 
		chassisLeftBack.move_velocity(0);
		chassisRightFront.move_velocity(0);
		chassisRightBack.move_velocity(0);
		std::cout << "task complete with error " << errorCurrent << std::endl;
		return;
	}
	
	// debug
	if (sameErrCycles == 0) {
		std::cout << "error  " << errorCurrent << std::endl;
		std::cout << "voltage " << voltage << std::endl;
	}

	// nothing goes after this
	errorLast = errorCurrent;
    pros::delay(10);
  }
}

void turn(int target, int voltageMax=115){
  targetTurn = target + imu.get_rotation();
  targetTurnRelative = target;
  voltageCap = 0; //reset velocity cap for slew rate
  turnP(voltageMax);
}

void autonomous() {
	// motor setup
	chassisLeftFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	chassisLeftBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	chassisRightFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	chassisRightBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	intakeMotor1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	intakeMotor2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	// debug
	autonSelection = 0; // -2
	std::cout << "auton  " << autonSelection << std::endl;

	switch (autonSelection) {
	case 0:
		// skillsss doesnt exist.
		turn(90);
		turn(-45);
		turn(45);
		turn(-90);
		break;

	case -1:
		// red unprotec

		// flip. out.
		liftMotor.move_absolute(1700, 100);
		while (liftMotor.get_position() < 1700) { // wait until we initiate flipout
			pros::delay(20);
		}
		liftMotor.move_absolute(-200, 100);
		pros::delay(400);
		// move forward and intake and get the 4 laid in a line
		intakeMotor1.move_velocity(100);
		intakeMotor2.move_velocity(100);
		drive(2500, 2500, 40);
		intakeMotor1.move_relative(300, 100);
		intakeMotor2.move_relative(300, 100);
		liftMotor.move_voltage(0);
		// back
		drive(-1300, -1300, 80);
		// turn for stack
		drive(600, -600, 60);
		intakeMotor1.move_relative(-500, 100);
		intakeMotor2.move_relative(-500, 100);
		// drive to stack
		drive(1400, 1400, 60);
		// stack
		intakeMotor1.move_velocity(-15);
		intakeMotor2.move_velocity(-15);
		trayMotor.move_absolute(6200, 100);
		while (trayMotor.get_position() < 6100) {
			pros::delay(20);
		}
		trayMotor.move_absolute(0, 100);
		drive(-800, -800);
		break;

	case -2:
		// red protec

		// flip. out.
		liftMotor.move_absolute(1700, 100);
		while (liftMotor.get_position() < 1700) { // wait until we initiate flipout
			pros::delay(20);
		}
		liftMotor.move_absolute(-200, 100);
		pros::delay(400);
		// move forward and intake and hope you get the b i g s t a c k
		intakeMotor1.move_velocity(100);
		intakeMotor2.move_velocity(100);
		drive(2200, 2200, 60);
		intakeMotor1.move_velocity(0);
		intakeMotor2.move_velocity(0);
		liftMotor.move_voltage(0);
		// turn for last cube
		drive(-550, 550, 80);
		intakeMotor1.move_velocity(30);
		intakeMotor2.move_velocity(30);
		// move and intake last cube
		drive(1400, 1400);
		intakeMotor1.move_velocity(100);
		intakeMotor2.move_velocity(100);
		// turn for stack
		drive(-140, 140);
		// stack
		intakeMotor1.move_relative(-700, 100);
		intakeMotor2.move_relative(-700, 100);
		drive(1200, 1200, 70);
		intakeMotor1.move_velocity(-15);
		intakeMotor2.move_velocity(-15);
		trayMotor.move_absolute(6200, 100);
		while (trayMotor.get_position() < 6100) {
			pros::delay(20);
		}
		trayMotor.move_absolute(0, 100);
		drive(-1000, -1000);
		break;
	
	case 1:
		// blue unprotec

		// flip. out.
		liftMotor.move_absolute(1700, 100);
		while (liftMotor.get_position() < 1700) { // wait until we initiate flipout
			pros::delay(20);
		}
		liftMotor.move_absolute(-200, 100);
		pros::delay(400);
		// move forward and intake and get the 4 laid in a line
		intakeMotor1.move_velocity(100);
		intakeMotor2.move_velocity(100);
		drive(2500, 2500, 40);
		intakeMotor1.move_relative(300, 100);
		intakeMotor2.move_relative(300, 100);
		liftMotor.move_voltage(0);
		// back
		drive(-1300, -1300, 80);
		// turn for stack
		drive(-600, 600, 60);
		intakeMotor1.move_relative(-500, 100);
		intakeMotor2.move_relative(-500, 100);
		// drive to stack
		drive(1400, 1400, 60);
		// stack
		intakeMotor1.move_velocity(-15);
		intakeMotor2.move_velocity(-15);
		trayMotor.move_absolute(6200, 100);
		while (trayMotor.get_position() < 6100) {
			pros::delay(20);
		}
		trayMotor.move_absolute(0, 100);
		drive(-800, -800);
		break;
	case 2:
		// blue protec

		break;
	
	default:
		break;
	}
}

void traySlew(bool forward) {
	if (forward) {
		// motion profile/slew: https://mycurvefit.com/share/0198b8c4-edce-4161-8198-b30318545d7c
		// 6200 is max (in theory, possible to go higher)
		double x = trayMotor.get_position();
		double speed = std::round(100 + 0.001102151*x - 0.000001478495* std::pow(x, 2));
		trayMotor.move_velocity(speed);

		std::cout << x << ": speed " << speed << std::endl;
	}
	else {
		if (trayMotor.get_position() < 1000) {
			trayMotor.move_velocity(-60);
		}
		else {
			trayMotor.move_velocity(-100);
		}
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

	// local vars
	int chassisModifier = 127;

	// main loop
	while (true) {
		// basic lift control, with an intake reverse at the end of lift
		if (bumperRD.isPressed()) {
			liftMotor.move_velocity(-100);
		} else if (bumperRU.isPressed()) {
			liftMotor.move_velocity(100);
		} else {
			liftMotor.move_voltage(0);
		}

		// basic intake control (maybe leave intake spinning during opcontrol at lower speed?)
		if (bumperLU.isPressed()) {
			intakeMotor1.move_velocity(100);
			intakeMotor2.move_velocity(100);
		} else if (bumperLD.isPressed()) {
			intakeMotor1.move_velocity(-100);
			intakeMotor2.move_velocity(-100);
		}
		else if (not shift.isPressed() and not (bumperLU.isPressed() or bumperLD.isPressed())) {
			intakeMotor1.move_velocity(0);
			intakeMotor2.move_velocity(0);
		}

		// dont brake intake if tray is up
		if (trayMotor.get_position() > 2000) {
			intakeMotor1.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
			intakeMotor2.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
		}
		else {
			intakeMotor1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
			intakeMotor2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
		}

		// tray control using shift key
		if (shift.isPressed()) { //shift key
			if (bumperLU.isPressed()) { // tray forward
				traySlew(true);
				// if tray and intake are interacting, move the intake
				intakeMotor1.move_velocity(-15);
				intakeMotor2.move_velocity(-15);
		
			} else if (bumperLD.isPressed()) { // tray backward
				traySlew(false);
				intakeMotor1.move_velocity(35);
				intakeMotor2.move_velocity(35);
			}
			else if (not (bumperLU.isPressed() or bumperLD.isPressed())) {
				intakeMotor1.move_velocity(0);
				intakeMotor2.move_velocity(0);
			}
		}
		else {
			trayMotor.move_voltage(0);
		}

		// chassis mod
		if (shift.isPressed()) { // brake and slow when we stacking
			chassisLeftFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
			chassisLeftBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
			chassisRightFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
			chassisRightBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
			chassisModifier = 80;
		}
		else { // return to normal after we stacked
			chassisLeftFront.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
			chassisLeftBack.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
			chassisRightFront.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
			chassisRightBack.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
			chassisModifier = 127;
		}
		
		chassisLeftFront.move(masterController.getAnalog(okapi::ControllerAnalog::leftY) * chassisModifier);
		chassisLeftBack.move(masterController.getAnalog(okapi::ControllerAnalog::leftY) * chassisModifier);
		chassisRightFront.move(masterController.getAnalog(okapi::ControllerAnalog::rightY) * chassisModifier);
		chassisRightBack.move(masterController.getAnalog(okapi::ControllerAnalog::rightY) * chassisModifier);

		// debug
		std::cout << pros::millis() << ": angle " << imu.get_rotation() << std::endl;
		pros::delay(20);
	}
}