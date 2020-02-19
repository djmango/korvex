#include <fstream>
#include "main.h"
#include "korvexlib.h"

// chassis
auto chassis = ChassisControllerBuilder()
		.withMotors({CHASSIS_LEFT_REAR, CHASSIS_LEFT_FRONT}, {-CHASSIS_RIGHT_REAR, -CHASIIS_RIGHT_FRONT})
		.withGains(
			{0.001, 0.0004, 0.00001}, // Distance controller gains
			{0.0001, 0.0001, 0.000}, // Turn controller gains
			{0.002, 0.01, 0.000} // Angle controller gains (helps drive straight)
		)
		// green gearset, 4 inch wheel diameter, 8.125 inch wheelbase
		.withDimensions(AbstractMotor::gearset::green, {{4_in, 8.125_in}, imev5GreenTPR})
		.buildOdometry(); // build an odometry chassis

auto profileController = AsyncMotionProfileControllerBuilder()
    .withLimits({1.0, 1.8, 5.0}) //double maxVel double maxAccel double maxJerk 
    .withOutput(chassis)
    .buildMotionProfileController();

// motors
okapi::MotorGroup intakeMotors({-INTAKE_MOTOR_LEFT, INTAKE_MOTOR_RIGHT});
pros::Controller master;

okapi::Motor liftMotor(ARM_MOTOR, false, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::counts);
okapi::Motor trayMotor(LEVER_MOTOR, false, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::counts);

// controller
pros::Controller master;

// sensors
pros::Imu imu(IMU_PORT);

// base global defenitions
const int LIFT_STACKING_HEIGHT = 700; // the motor ticks above which we are stacking
enum class autonStates {
	off,
	redProtec,
	redUnprotec,
	redRick,
	blueProtec,
	blueUnprotec,
	blueRick,
	skills
};
autonStates autonSelection = autonStates::off;

enum class trayStates {
	returned,
	returning,
	extending,
};
trayStates trayState = trayStates::returned;

// create a button descriptor string array
static const char *btnmMap[] = {"Unprotec", "Protec", "Rick", ""};

// motion control system globals
int voltageCap; // voltageCap limits the change in velocity and must be global
int targetLeft;
int targetRight;
int targetTurn;
int targetTurnRelative;

void driveP(int voltageMax) {
  chassis->getModel()->resetSensors(); // reset base encoders
  int errorLeft;
  int errorRight;
 
  // the touchables ;)))))))) touch me uwu :):):)
  float kp = 0.15;
  float acc = 5;
  float kpTurn = 0.7;
  float accTurn = 4;

  // the untouchables
  float voltageLeft = 0;
  float voltageRight = 0;
  int signLeft;
  int signRight;
  int errorCurrent = 0;
  int errorLast = 0;
  int sameErrCycles = 0;
  int same0ErrCycles = 0;
  int startTime = pros::millis();

  while(autonomous){
    errorLeft = targetLeft - chassis->getModel()->getSensorVals()[0]; // error is target minus actual value
    errorRight = targetRight - chassis->getModel()->getSensorVals()[1];
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
    chassis->getModel()->tank(voltageLeft/127, voltageRight/127);

	// timeout utility
	if (errorLast == errorCurrent) {
		if (errorCurrent <= 2) {
			same0ErrCycles +=1;
		}
		sameErrCycles += 1;
	}
	else {
		sameErrCycles = 0;
		same0ErrCycles = 0;
	}

	// exit paramaters
	if ((errorLast < 5 and errorCurrent < 5) or sameErrCycles >= 10) { // allowing for smol error or exit if we stay the same err for .2 second
		chassis->stop();
		std::cout << "task complete with error " << errorCurrent << " in " << (pros::millis() - startTime) << "ms" << std::endl;
		return;
	}
	
	// debug
	// std::cout << "error  " << errorCurrent << std::endl;
	// std::cout << "errorLeft  " << errorLeft << std::endl;
	// std::cout << "errorRight  " << errorRight << std::endl;
	// std::cout << "voltageLeft  " << voltageLeft << std::endl;
	// std::cout << "voltageRight  " << voltageRight << std::endl;

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
  float kp = 1.6;
  float ki = 0.7;
  float kd = 0.1;
  float acc = 20;

  // the untouchables
  float voltage = 0;
  float errorCurrent;
  float errorLast;
  int errorCurrentInt;
  int errorLastInt;
  int sameErrCycles = 0;
  int same0ErrCycles = 0;
  int p;
  float i;
  int d;
  int sign;
  float error;
  int startTime = pros::millis();

  while(autonomous){
    error = targetTurn - imu.get_rotation();
	errorCurrent = abs(error);
	errorCurrentInt = errorCurrent;
	sign = targetTurnRelative / abs(targetTurnRelative); // -1 or 1

	p = (error * kp);
	if (abs(error) < 10) { // if we are in range for I to be desireable
        i = ((i + error) * ki);
	}
	else
        i = 0;
	// if ((abs(error) - errorLast) < 0)
	d = (error - errorLast) * kd;
	
	voltage = p + i + d;
	voltageCap = voltageCap + acc;  // slew rate
    
	if(voltageCap > voltageMax){
      voltageCap = voltageMax; // voltageCap cannot exceed 115
    }

    if(abs(voltage) > voltageCap){ // limit the voltage
      voltage = voltageCap * sign;
    }

	// set the motors to the intended speed
    chassis->getModel()->tank(voltage/127, -voltage/127);

	// timeout utility
	if (errorLastInt == errorCurrentInt) {
		if (errorLast <= 2 and errorCurrent <= 2) { // saying that error less than 2 counts as 0
			same0ErrCycles +=1;
		}
		sameErrCycles += 1;
	}
	else {
		sameErrCycles = 0;
		same0ErrCycles = 0;
	}

	// exit paramaters
	if (same0ErrCycles >= 5 or sameErrCycles >= 60) { // allowing for smol error or exit if we stay the same err for .6 second
		chassis->stop();
		std::cout << "task complete with error " << errorCurrent << " in " << (pros::millis() - startTime) << "ms" << std::endl;
		return;
	}
	
	// debug
	// std::cout << "error " << errorCurrent << std::endl;
	// std::cout << "voltage " << voltage << std::endl;

	// for csv output, graphing the function
	// std::cout << pros::millis() << "," << error << "," << voltage << std::endl;

	// nothing goes after this
	errorLast = errorCurrent;
	errorLastInt = errorLast;
    pros::delay(10);
  }
}

void turn(int target, int voltageMax=115){
  targetTurn = target + imu.get_rotation();
  targetTurnRelative = target;
  voltageCap = 0; //reset velocity cap for slew rate
  turnP(voltageMax);
}

static lv_res_t autonBtnmAction(lv_obj_t *btnm, const char *txt) {
	if (lv_obj_get_free_num(btnm) == 100) { // reds
		if (txt == "Unprotec") autonSelection = autonStates::redUnprotec;
		else if (txt == "Protec") autonSelection = autonStates::redProtec;
		else if (txt == "Rick") autonSelection = autonStates::redRick;	
	}
	else if (lv_obj_get_free_num(btnm) == 101) { // blues
		if (txt == "Unprotec") autonSelection = autonStates::blueUnprotec;
		else if (txt == "Protec") autonSelection = autonStates::blueProtec;
		else if (txt == "Rick") autonSelection = autonStates::blueRick;
	}

	return LV_RES_OK; // return OK because the button matrix is not deleted
}

static lv_res_t skillsBtnAction(lv_obj_t *btn) {
	autonSelection = autonStates::skills;
	return LV_RES_OK;
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

void initialize() {
	// save some time
	imu.reset();
	std::cout << pros::millis() << ": calibrating imu..." << std::endl;

	// lvgl theme
	lv_theme_t *th = lv_theme_alien_init(360, NULL); //Set a HUE value and keep font default RED
	lv_theme_set_current(th);

	// create a tab view object
	std::cout << pros::millis() << ": creating gui..." << std::endl;
	lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), NULL);

	// add 4 tabs (the tabs are page (lv_page) and can be scrolled
	lv_obj_t *redTab = lv_tabview_add_tab(tabview, "Red");
	lv_obj_t *blueTab = lv_tabview_add_tab(tabview, "Blue");
	lv_obj_t *skillsTab = lv_tabview_add_tab(tabview, "Skills");
	lv_obj_t *telemetryTab = lv_tabview_add_tab(tabview, "Telemetry");

	// red tab
	lv_obj_t *redBtnm = lv_btnm_create(redTab, NULL);
	lv_btnm_set_map(redBtnm, btnmMap);
	lv_btnm_set_action(redBtnm, autonBtnmAction);
	lv_obj_set_size(redBtnm, 450, 50);
	lv_btnm_set_toggle(redBtnm, true, 3);
	lv_obj_set_pos(redBtnm, 0, 100);
	lv_obj_align(redBtnm, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_free_num(redBtnm, 100);

	// blue tab
	lv_obj_t *blueBtnm = lv_btnm_create(blueTab, NULL);
	lv_btnm_set_map(blueBtnm, btnmMap);
	lv_btnm_set_action(blueBtnm, autonBtnmAction);
	lv_obj_set_size(blueBtnm, 450, 50);
	lv_btnm_set_toggle(blueBtnm, true, 3);
	lv_obj_set_pos(blueBtnm, 0, 100);
	lv_obj_align(blueBtnm, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_free_num(blueBtnm, 101);

	// skills tab
	lv_obj_t *skillsBtn = lv_btn_create(skillsTab, NULL);
	lv_obj_t *label = lv_label_create(skillsBtn, NULL);
	lv_label_set_text(label, "Skills");
	lv_btn_set_action(skillsBtn, LV_BTN_ACTION_CLICK, skillsBtnAction);
	lv_obj_set_size(skillsBtn, 450, 50);
	lv_btnm_set_toggle(skillsBtn, true, 1);
	lv_obj_set_pos(skillsBtn, 0, 100);
	lv_obj_align(skillsBtn, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_free_num(skillsBtn, 102);

	// debug
	lv_obj_t *msgBox = lv_mbox_create(telemetryTab, NULL);
	lv_mbox_set_text(msgBox, "rick from r");
	lv_obj_align(msgBox, NULL, LV_ALIGN_CENTER, 0, 20);
	lv_mbox_set_anim_time(msgBox, 300);
	lv_mbox_start_auto_close(msgBox, 2000);

	std::cout << pros::millis() << ": finished creating gui!" << std::endl;
	while (imu.is_calibrating() and pros::millis() < 5000)
	{
		pros::delay(10);
	}
	if (pros::millis() < 5000) std::cout << pros::millis() << ": finished calibrating!" << std::endl;
	else std::cout << pros::millis() << ": calibration failed, moving on" << std::endl;

	std::cout << "\n" << pros::millis() << ": motor temps:" << std::endl;
	
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
	chassis->stop();
}

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
 * with the default pchassiosisioriority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

void flipout() {
	// this is on you fools.
}

void autonomous() {
	chassis->setState({0_in, 0_in, 0_deg});
	chassis->setMaxVelocity(200);
	chassis->getModel()->setBrakeMode(AbstractMotor::brakeMode::hold);
	float origAngle = imu.get_rotation();
	
	auto timer = TimeUtilFactory().create().getTimer();
	timer->placeMark();

	if (autonSelection == autonStates::off) autonSelection = autonStates::skills; // use debug if we havent selected any auton

	switch (autonSelection) {
	case autonStates::skills:
		// skills doesnt exist. i wrote a (working???) 9 cube during lunch!
		chassis->driveToPoint({10_cm, 0_cm});
		// std::cout << pros::millis() << ": state  " << chassis->getState().str() << std::endl;

		flipout();
		pros::delay(500);
		turn(-(origAngle + imu.get_rotation()));
		// grab 4 cubes
		intakeMotors.moveRelative(7200, 200);
		drive(1900, 1900, 50);
		// go around tower
		turn(45 - (origAngle + imu.get_rotation()));
		intakeMotors.moveRelative(4000, 200);
		drive(700, 700, 70);
		turn(-45 - (origAngle + imu.get_rotation()));
		drive(700, 700, 70);
		turn(-(origAngle + imu.get_rotation()));
		// grab next 4
		intakeMotors.moveRelative(7500, 200);
		drive(1900, 1900, 50);
		// turn for stack
		turn(60 - imu.get_rotation());
		// move to zone
		intakeMotors.moveRelative(-700, 50);
		drive(1500, 1500, 80);
		// stack
		intakeMotors.moveVelocity(-20);
		liftMotor.moveAbsolute(-20, 100);
		trayMotor.moveAbsolute(6200, 100);
		while (trayMotor.getPosition() < 6000) {
			pros::delay(20);
		}
		trayMotor.moveAbsolute(0, 100);
		intakeMotors.moveVelocity(-50);
		drive(250, 250);
		drive(-600, -600, 80);
		intakeMotors.moveVelocity(0);
		break;

	case autonStates::redUnprotec:
		// red unprotec 5 cube
		flipout();
		pros::delay(300);
		turn(-(origAngle + imu.get_rotation()));
		// grab 4 cubes
		intakeMotors.moveRelative(6800, 200);
		drive(1800, 1800, 70);
		// turn for stack
		turn(150 - imu.get_rotation()); // use absolute positioning
		// move to zone
		intakeMotors.moveRelative(-700, 50);
		drive(1500, 1500, 80);
		// stack
		intakeMotors.moveVelocity(-20);
		liftMotor.moveAbsolute(-20, 100);
		trayMotor.moveAbsolute(6200, 100);
		while (trayMotor.getPosition() < 6000) {
			pros::delay(20);
		}
		trayMotor.moveAbsolute(0, 100);
		intakeMotors.moveVelocity(-50);
		drive(250, 250);
		drive(-600, -600, 80);
		intakeMotors.moveVelocity(0);
		break;

	case autonStates::redProtec:
		// red protec 4 cube
		flipout();
		turn(-(origAngle + imu.get_rotation()));
		// grab 3 cubes
		intakeMotors.moveRelative(5000, 200);
		drive(1700, 1700, 70);
		intakeMotors.moveVoltage(0);
		// turn for final cube
		turn(-132 - imu.get_rotation());
		// grab final cube
		intakeMotors.moveRelative(5000, 200);
		drive(1300, 1300, 70);
		// move to zone
		turn(-136 - imu.get_rotation());
		drive(580, 580);
		// stack
		intakeMotors.moveRelative(-1300, 90);
		liftMotor.moveAbsolute(-20, 100);
		trayMotor.moveAbsolute(6200, 100);
		while (trayMotor.getPosition() < 4000) {
			pros::delay(20);
		}
		intakeMotors.moveVelocity(-30);
		while (trayMotor.getPosition() < 6000) {
			pros::delay(20);
		}
		trayMotor.moveAbsolute(0, 100);
		intakeMotors.moveVelocity(-50);
		drive(250, 250);
		drive(-800, -800, 80);
		intakeMotors.moveVelocity(0);
		break;
	
	case autonStates::redRick:
		// red protec 3 cube
		flipout();
		turn(-(origAngle + imu.get_rotation()));
		// grab 3 cubes
		intakeMotors.moveRelative(5000, 200);
		drive(900, 900, 70);
		intakeMotors.moveVoltage(0);
		// turn for final cube
		turn(-90 - imu.get_rotation());
		// grab final cube
		intakeMotors.moveRelative(4000, 200);
		drive(900, 900, 70);
		// move to zone
		turn(-132 - imu.get_rotation());
		drive(520, 520);
		// stack
		intakeMotors.moveRelative(-900, 200);
		liftMotor.moveAbsolute(-20, 100);
		trayMotor.moveAbsolute(6200, 100);
		while (trayMotor.getPosition() < 6000) {
			pros::delay(20);
		}
		trayMotor.moveAbsolute(0, 100);
		intakeMotors.moveVelocity(-50);
		drive(250, 250);
		drive(-600, -600, 80);
		intakeMotors.moveVelocity(0);
		break;
	
	case autonStates::blueUnprotec:
		// blue unprotec 5 cube
		flipout();
		pros::delay(300);
		turn(-(origAngle + imu.get_rotation()));
		// grab 4 cubes
		intakeMotors.moveRelative(6800, 200);
		drive(1800, 1800, 70);
		// turn for stack
		turn(-150 - imu.get_rotation()); // use absolute positioning
		// move to zone
		intakeMotors.moveRelative(-700, 50);
		drive(1500, 1500, 80);
		// stack
		intakeMotors.moveVelocity(-20);
		liftMotor.moveAbsolute(-20, 100);
		trayMotor.moveAbsolute(6200, 100);
		while (trayMotor.getPosition() < 6000) {
			pros::delay(20);
		}
		trayMotor.moveAbsolute(0, 100);
		intakeMotors.moveVelocity(-50);
		drive(250, 250);
		drive(-600, -600, 80);
		intakeMotors.moveVelocity(0);
		break;
	case autonStates::blueProtec:
		// blue protec 4 cube
		flipout();
		turn(-(origAngle + imu.get_rotation()));
		// grab 3 cubes
		intakeMotors.moveRelative(5000, 200);
		drive(1700, 1700, 80);
		intakeMotors.moveVoltage(0);
		// turn for final cube
		turn(130 - imu.get_rotation());
		// grab final cube
		intakeMotors.moveRelative(5400, 200);
		drive(1300, 1300, 70);
		// move to zone
		drive(550, 550);
		intakeMotors.moveRelative(-900, 90);
		// stack
		liftMotor.moveAbsolute(-20, 100);
		trayMotor.moveAbsolute(6200, 100);
		while (trayMotor.getPosition() < 3800) {
			pros::delay(20);
		}
		intakeMotors.moveVelocity(-20);
		while (trayMotor.getPosition() < 6000) {
			pros::delay(20);
		}
		trayMotor.moveAbsolute(0, 100);
		intakeMotors.moveVelocity(-50);
		drive(-800, -800, 80);
		intakeMotors.moveVelocity(0);
		break;
	case autonStates::blueRick:
		// blue protec 3 cube
		flipout();
		turn(-(origAngle + imu.get_rotation()));
		// grab 3 cubes
		intakeMotors.moveRelative(5000, 200);
		drive(900, 900, 70);
		intakeMotors.moveVoltage(0);
		// turn for final cube
		turn(90 - imu.get_rotation());
		// grab final cube
		intakeMotors.moveRelative(4000, 200);
		drive(900, 900, 70);
		// move to zone
		turn(132 - imu.get_rotation());
		drive(520, 520);
		// stack
		intakeMotors.moveRelative(-900, 200);
		liftMotor.moveAbsolute(-20, 100);
		trayMotor.moveAbsolute(6200, 100);
		while (trayMotor.getPosition() < 6000) {
			pros::delay(20);
		}
		trayMotor.moveAbsolute(0, 100);
		intakeMotors.moveVelocity(-50);
		drive(250, 250);
		drive(-600, -600, 80);
		intakeMotors.moveVelocity(0);
		break;
	
	default:
		break;
	}
	std::cout << pros::millis() << ": auton took " << timer->getDtFromMark().convert(second) << " seconds" << std::endl;
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


//where the arm is
int armPos = 0;

//chassis
void chassis_tank_drive(int left, int right) {
    const double chassis_movement_threshold = 0.05;
    chassis->getModel()->tank(right / 127.0, left / 127.0, chassis_movement_threshold);
}
//intake
void intake_drive(int intakeSpeed) {
	intakeMotors.moveVelocity(intakeSpeed);
}
//lever
void lever_drive(float lever_speed) {
	trayMotor.moveVelocity(lever_speed);
}

int armTarget = 0;
int armIterate = 0;

void arm_drive(int presetPos) {
	//so arm doesn't click w/ gear
	lever_drive(-200);
	pros::delay(50);
	lever_drive(0);
	armIterate = presetPos;
	if (armIterate >= ARM_PRESETS_LEN) {
		armIterate = ARM_PRESETS_LEN;
	}
	armTarget = ARM_PRESETS[armIterate];
	// master.print(0, 0, "Don't read this sentence %d", armTarget);
	liftMotor.moveAbsolute(armTarget, 200);
}

//presets
void arm_control() {
	int armTarget = 0;
	int armIterate = 0;

	if (master.get_digital_new_press(DIGITAL_RIGHT)) {
		//descore high
		arm_drive(3);
		armPos = 3;
	} else if (master.get_digital_new_press(DIGITAL_UP)) {
		//score high
		arm_drive(4);
		armPos = 4;
	} else if (master.get_digital_new_press(DIGITAL_LEFT)) {
		//descore low
		arm_drive(1);
		armPos = 1;
	} else if(master.get_digital_new_press(DIGITAL_DOWN)) {
		//score low
		arm_drive(2);
		armPos = 2;
	} else if (master.get_digital_new_press(DIGITAL_Y) == 1 && !armPos == 0) {
		//original pos
		armIterate = 0;
		armPos = 0;
		liftMotor.setBrakeMode(AbstractMotor::brakeMode::coast);
		liftMotor.moveVoltage(12000);
		pros::delay(500);
		liftMotor.moveVoltage(0);
	}
}

//manual drive for arm
void arm_control2() {

	if(master.get_digital(pros::E_CONTROLLER_DIGITAL_B)) {

		liftMotor.moveVoltage(-12000);
		liftMotor.setBrakeMode(AbstractMotor::brakeMode::hold);
		// pros::delay(50);
		// liftMotor.move_voltage(0);
	} else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_A)) {

		liftMotor.moveVoltage(5000);
		liftMotor.setBrakeMode(AbstractMotor::brakeMode::hold);
		// pros::delay(50);
		// liftMotor.move_voltage(0);

	} else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_X)) {

		liftMotor.setBrakeMode(AbstractMotor::brakeMode::coast);
		liftMotor.moveVoltage(12000);
		pros::delay(300);
		liftMotor.moveVoltage(0);

	}  else if (armPos == 0) {
		//so arm control 1&2 dont collide
		liftMotor.moveVoltage(0);
	}

}

//for chassis
void chassis_control() {
	int left_power = master.get_analog(ANALOG_LEFT_Y);
	int right_power = master.get_analog(ANALOG_RIGHT_Y);
	chassis_tank_drive(left_power, right_power);
}

//intake
void intake_control() {
	if(master.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) {
		intake_drive(-200);
	} else if(master.get_digital(pros::E_CONTROLLER_DIGITAL_L2) && armPos == 0) {
		intake_drive(190);
	} else if(master.get_digital(pros::E_CONTROLLER_DIGITAL_L2) && armPos > 0) {
		//slower for scoring in towers
		pros::delay(15);
		intake_drive(96);
	} else {
		intake_drive(0);
	}
}
int leverEndPos = -6550;
int leverStartPos= 138;
int tray_speed = 12.5;

void lever_control() {
	if(master.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
		intake_drive(24);
		if(trayMotor.getPosition() < -3550) {
			tray_speed = 10;
		} else if(trayMotor.getPosition() >= -3550) {
			tray_speed = 200;
		} 
		trayMotor.moveAbsolute(leverEndPos, pros::E_CONTROLLER_DIGITAL_R1*tray_speed);
		//intake pushes out lever slightly
	} else if(master.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
		trayMotor.moveAbsolute(leverStartPos, pros::E_CONTROLLER_DIGITAL_R1*200);
	} else {
		trayMotor.moveVelocity(0);
	}
}


void opcontrol() {
	chassis->stop();
	chassis->getModel()->setBrakeMode(AbstractMotor::brakeMode::coast);
	chassis->setMaxVelocity(200);

	// main loop
	while (true) {

		//chassis stuff
		chassis_control();

		//intake
		intake_control();

		//lever stuff
		lever_control();

		// lift from korvex
		arm_control();
		
		arm_control2();
		pros::delay(20);
	}
}