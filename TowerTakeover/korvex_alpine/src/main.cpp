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

	if (txt == "Left")
	{
		autonSelection = 1;
	}
	if (txt == "Right")
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

static lv_res_t tempUpdateAction(lv_obj_t *btn)
{
	// lv_table_set_cell_value(tempTable, 0, 0, "eels");
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
	lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), NULL);

	// add 4 tabs (the tabs are page (lv_page) and can be scrolled
	lv_obj_t *telemetryTab = lv_tabview_add_tab(tabview, "Telemetry");
	lv_obj_t *redTab = lv_tabview_add_tab(tabview, "Red");
	lv_obj_t *blueTab = lv_tabview_add_tab(tabview, "Blue");
	lv_obj_t *skillsTab = lv_tabview_add_tab(tabview, "Skills");

	// add content to the tabs
	// telemtetry tab

	// temperature table
	lv_obj_t *tempTable = lv_table_create(telemetryTab, NULL);
	lv_table_set_row_cnt(tempTable, 4);
	lv_table_set_col_cnt(tempTable, 2);

	// temperature update
	lv_obj_t *tempUpdate = lv_btn_create(telemetryTab, NULL);
	lv_btn_set_action(tempUpdate, LV_BTN_ACTION_CLICK,  tempUpdateAction);
	lv_obj_set_size(tempUpdate, 150, 50);
	lv_obj_set_pos(tempUpdate, 0, 100);
	lv_obj_t *tempLabel = lv_label_create(tempUpdate, NULL);
	lv_label_set_text(tempLabel, "rick from r");
	lv_obj_align(tempUpdate, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);


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

	// telemetry update task
	// pros::Task (tempUpdate, (lv_obj_t*)"tempTable", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "tempUpdateTask");
}

void tempUpdate(lv_obj_t* tempTable) {
	lv_table_set_cell_value(tempTable, 0, 0, "rick");
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
		sameErrCycles += 1;
	}
	else {
		sameErrCycles = 0;
	}


	// exit paramaters
	if ((errorCurrent < 2 and errorLast < 2) or sameErrCycles >= 25) { // allowing for smol error or exit if we stay the same err for .5 second
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
	// autonSelection = -1;
	std::cout << "auton  " << autonSelection << std::endl;

	switch (autonSelection) {
	case 0:
		// skillsss doesnt exist.
		chassisLeftFront.move_velocity(-100);
		chassisLeftBack.move_velocity(-100);
		chassisRightFront.move_velocity(-100);
		chassisRightBack.move_velocity(-100);
		pros::delay(1500);
		chassisLeftFront.move_velocity(100);
		chassisLeftBack.move_velocity(100);
		chassisRightFront.move_velocity(100);
		chassisRightBack.move_velocity(100);
		pros::delay(1500);
		chassisLeftFront.move_velocity(0);
		chassisLeftBack.move_velocity(0);
		chassisRightFront.move_velocity(0);
		chassisRightBack.move_velocity(0);
		break;

	case -1:
		// red left
		
		// flip. out.
		liftMotor.move_absolute(1600, 100);
		pros::delay(1300);
		liftMotor.move_absolute(0, 100);
		pros::delay(300);
		// move forward and intake and hope you get the b i g s t a c k
		intakeMotor1.move_velocity(100);
		intakeMotor2.move_velocity(100);
		drive(2200, 2200, 60);
		intakeMotor1.move_velocity(20);
		intakeMotor2.move_velocity(20);
		// turn for last cube
		drive(-535, 535);
		intakeMotor1.move_velocity(100);
		intakeMotor2.move_velocity(100);
		// move and intake last cube
		drive(1400, 1400);
		intakeMotor1.move_velocity(0);
		intakeMotor2.move_velocity(0);
		// turn for stack
		drive(-120, 120);
		drive(1200, 1200);
		// stack
		intakeMotor1.move_velocity(-15);
		intakeMotor2.move_velocity(-15);
		trayMotor.move_absolute(-4500, 100);
		pros::delay(1200);
		drive(-1000, -1000);
		break;

	case -2:
		// red right
		
		// flip. out.
		liftMotor.move_absolute(1400, 100);
		pros::delay(1200);
		liftMotor.move_absolute(0, 100);
		pros::delay(300);
		// move forward and intake and hope you get the b i g s t a c k
		intakeMotor1.move_velocity(100);
		intakeMotor2.move_velocity(100);
		drive(2200, 2200, 60);
		intakeMotor1.move_velocity(50);
		intakeMotor2.move_velocity(50);
		// back
		drive(-2000, -2000);
		// turn for stack
		intakeMotor1.move_velocity(0);
		intakeMotor2.move_velocity(0);
		drive(400, -400);
		drive(1500, 1500);
		// stack
		intakeMotor1.move_velocity(-15);
		intakeMotor2.move_velocity(-15);
		trayMotor.move_absolute(-4500, 100);
		pros::delay(1200);
		drive(-1000, -1000);
		break;
	
	case 1:
		// blue left
		
		// flip. out.
		liftMotor.move_absolute(1400, 100);
		pros::delay(1200);
		liftMotor.move_absolute(0, 100);
		pros::delay(300);
		// move forward and intake and hope you get the b i g s t a c k
		intakeMotor1.move_velocity(100);
		intakeMotor2.move_velocity(100);
		drive(2200, 2200, 60);
		intakeMotor1.move_velocity(20);
		intakeMotor2.move_velocity(20);
		// back
		drive(-2000, -2000);
		// turn for stack
		intakeMotor1.move_velocity(0);
		intakeMotor2.move_velocity(0);
		drive(-400, 400);
		drive(1500, 1500);
		// stack
		intakeMotor1.move_velocity(-15);
		intakeMotor2.move_velocity(-15);
		trayMotor.move_absolute(-4500, 100);
		pros::delay(1200);
		drive(-1000, -1000);
		break;
	case 2:
		// blue right
		
		// flip. out.
		liftMotor.move_absolute(1600, 100);
		pros::delay(1300);
		liftMotor.move_absolute(0, 100);
		pros::delay(300);
		// move forward and intake and hope you get the b i g s t a c k
		intakeMotor1.move_velocity(100);
		intakeMotor2.move_velocity(100);
		drive(2200, 2200, 60);
		intakeMotor1.move_velocity(20);
		intakeMotor2.move_velocity(20);
		// turn for last cube
		drive(535, -535);
		intakeMotor1.move_velocity(100);
		intakeMotor2.move_velocity(100);
		// move and intake last cube
		drive(1400, 1400);
		intakeMotor1.move_velocity(0);
		intakeMotor2.move_velocity(0);
		// turn for stack
		drive(120, -120);
		drive(1200, 1200);
		// stack
		intakeMotor1.move_velocity(-15);
		intakeMotor2.move_velocity(-15);
		trayMotor.move_absolute(-4500, 100);
		pros::delay(1200);
		drive(-1000, -1000);
		break;
		
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
	
	// debug
	
	// main loop
	while (true) {
		// basic lift control, with an intake reverse at the end of lift
		if (bumperRD.isPressed()) {
			liftMotor.move_velocity(-100);
		} else if (bumperRU.isPressed()) {
			liftMotor.move_velocity(100);
		} else if (bumperRD.changedToReleased() and liftMotor.get_position() > 100) {
			intakeMotor1.move_relative(-2000, 100);
			intakeMotor2.move_relative(-2000, 100);
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

		// tray control using shift key
		if (shift.isPressed()) { //shift key
			if (bumperLU.isPressed()) { // tray forward
				trayMotor.move_velocity(-100);
				// if tray and intake are interacting, move the intake
				intakeMotor1.move_velocity(-15);
				intakeMotor2.move_velocity(-15);
		
			} else if (bumperLD.isPressed()) { // tray backward
				trayMotor.move_velocity(100);
				// if tray and intake are interacting, move the intake
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

		if (shift.isPressed()) { // brake when we stacking
			chassisLeftFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
			chassisLeftBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
			chassisRightFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
			chassisRightBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

		}
		else { // return to normal after we stacked
			chassisLeftFront.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
			chassisLeftBack.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
			chassisRightFront.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
			chassisRightBack.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
		}

		// debug
		// std::cout << pros::millis() << ": angle " << liftMotor.get_position() << std::endl;

		//drive control
		chassisLeftFront.move(masterController.getAnalog(okapi::ControllerAnalog::leftY) * 127);
		chassisLeftBack.move(masterController.getAnalog(okapi::ControllerAnalog::leftY) * 127);
		chassisRightFront.move(masterController.getAnalog(okapi::ControllerAnalog::rightY) * 127);
		chassisRightBack.move(masterController.getAnalog(okapi::ControllerAnalog::rightY) * 127);
		pros::delay(20);
	}
}