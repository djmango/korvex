#include "main.h"
#include "korvexlib.h"
#include "okapi/api.hpp"
using namespace okapi;

// chassis
auto chassis = ChassisControllerBuilder()
		.withMotors({LEFT_MTR2, LEFT_MTR1}, {-RIGHT_MTR2, -RIGHT_MTR1})
		.withGains(
			{0.002, 0.01, 0.000}, // Distance controller gains
			{0.000, 0.0, 0.000}, // Turn controller gains
			{0.002, 0.01, 0.000} // Angle controller gains (helps drive straight)
		)
		// green gearset, 4 1/8 inch wheel diameter, 8 1/8 inch wheelbase
		.withDimensions(AbstractMotor::gearset::green, {{4_in, 8.125_in}, imev5GreenTPR})
		.withOdometry() // use the same scales as the chassis (above)
		.buildOdometry(); // build an odometry chassis

auto profileController = AsyncMotionProfileControllerBuilder()
    .withLimits({1.0, 1.5, 5.0}) //double maxVel double maxAccel double maxJerk 
    .withOutput(chassis)
    .buildMotionProfileController();


// base global defenitions
int autonSelection = 42; // hitchhikers anyone?
const int LIFT_STACKING_HEIGHT = 700; // the motor ticks above which we are stacking

// create a button descriptor string array
static const char *btnmMap[] = {"Left", "Right", "Rick", ""};

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
		if (errorCurrent < 2) {
			same0ErrCycles +=1;
		}
		sameErrCycles += 1;
	}
	else {
		sameErrCycles = 0;
		same0ErrCycles = 0;
	}

	// exit paramaters
	if ((errorLast < 4 and errorCurrent < 4) or sameErrCycles >= 10) { // allowing for smol error or exit if we stay the same err for .2 second
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
		if (errorLast < 2 and errorCurrent < 2) { // saying that error less than 2 counts as 0
			same0ErrCycles +=1;
		}
		sameErrCycles += 1;
	}
	else {
		sameErrCycles = 0;
		same0ErrCycles = 0;
	}

	// exit paramaters
	if (same0ErrCycles >= 5 or sameErrCycles >= 30) { // allowing for smol error or exit if we stay the same err for .3 second
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

void flipout() { // a blocking flipout function
	trayMotor.move_absolute(1000, 100);
	liftMotor.move_velocity(100);
	while (liftMotor.get_position() < 2000) { // wait until we initiate flipout
		pros::delay(20);
	}
	liftMotor.move_absolute(-200, 100);
	while (liftMotor.get_position() > 400) { // wait until we initiate flipout
		pros::delay(20);
	}
	intakeMotors.moveVelocity(200);
	trayMotor.move_absolute(0, 100);
}

void traySlew(bool forward) {
	if (forward) {
		if (trayMotor.get_position() > 4000) trayMotor.move_velocity(40);
		else trayMotor.move_velocity(100);
	}
	else {
		if (trayMotor.get_position() < 1000) trayMotor.move_velocity(-60);
		else trayMotor.move_velocity(-100);
	}
}

void generatePaths() { // all paths stored here
	// 8 cube s curve, made for red side (reverse for blue)
	profileController->generatePath({
				{0_ft, 0_ft, 0_deg},
				{2.2_ft, 3.5_ft, 0_deg}},
				"redRickSCurve" 
			);
	
	profileController->generatePath({
				{0_ft, 0_ft, 0_deg},
				{4.2_ft, 0_ft, 0_deg}},
				"balls" 
			);
}

static lv_res_t autonBtnmAction(lv_obj_t *btnm, const char *txt)
{
	if (lv_obj_get_free_num(btnm) == 100) {
		if (txt == "Unprotec") autonSelection = -1;
		else if (txt == "Protec") autonSelection = -2;
		else if (txt == "Rick") autonSelection = -3;	
	}
	else if (lv_obj_get_free_num(btnm) == 101) {
		if (txt == "Unprotec") autonSelection = 1;
		else if (txt == "Protec") autonSelection = 2;
		else if (txt == "Rick") autonSelection = 3;
	}

	return LV_RES_OK; // return OK because the button matrix is not deleted
}

static lv_res_t skillsBtnAction(lv_obj_t *btn)
{
	autonSelection = 0;
	return LV_RES_OK;
}

static lv_res_t ddlist_action(lv_obj_t * ddlist)
{

    char selectedMotor[32];
	char motorTemp[2];
    lv_ddlist_get_selected_str(ddlist, selectedMotor);

	if (selectedMotor == "Intake") {
		char motorTemp = intakeMotors.getTemperature();
	}

	lv_obj_t *msgBox = lv_mbox_create(ddlist, NULL);
	lv_mbox_set_text(msgBox, selectedMotor);
	lv_obj_align(msgBox, ddlist, LV_ALIGN_CENTER, 0, 0);
	lv_mbox_set_anim_time(msgBox, 100);
	lv_mbox_start_auto_close(msgBox, 5000);

    return LV_RES_OK; /*Return OK if the drop down list is not deleted*/
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

void initialize()
{
	// save some time
	imu.reset();

	// lvgl theme

	lv_theme_t *th = lv_theme_alien_init(360, NULL); //Set a HUE value and keep font default RED
	lv_theme_set_current(th);

	// create a tab view object
	lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), NULL);

	// add 4 tabs (the tabs are page (lv_page) and can be scrolled
	lv_obj_t *redTab = lv_tabview_add_tab(tabview, "Red");
	lv_obj_t *blueTab = lv_tabview_add_tab(tabview, "Blue");
	lv_obj_t *skillsTab = lv_tabview_add_tab(tabview, "Skills");
	lv_obj_t *telemetryTab = lv_tabview_add_tab(tabview, "Telemetry");

	// telemetry tab

	// drop down list
	lv_obj_t * ddl1 = lv_ddlist_create(telemetryTab, NULL);
	lv_ddlist_set_options(ddl1, "Intake\n"
								"Chassis\n"
								"Lift\n"
								"Tray\n"
								"Rick");
	lv_obj_align(ddl1, NULL, LV_ALIGN_CENTER, -170, 0);
	lv_ddlist_set_sb_mode(ddl1, LV_SB_MODE_ON);
	lv_ddlist_set_action(ddl1, ddlist_action);

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
	lv_btnm_set_toggle(skillsBtn, true, 3);
	lv_obj_set_pos(skillsBtn, 0, 100);
	lv_obj_align(skillsBtn, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_free_num(skillsBtn, 102);

	// debug
	lv_obj_t *msgBox = lv_mbox_create(telemetryTab, NULL);
	lv_mbox_set_text(msgBox, "rick from r");
	lv_obj_align(msgBox, NULL, LV_ALIGN_CENTER, 0, 20);
	lv_mbox_set_anim_time(msgBox, 300);
	lv_mbox_start_auto_close(msgBox, 2000);

	// wait for calibrate
	pros::delay(100);
	generatePaths();
	std::cout << pros::millis() << ": calibrating imu..." << std::endl;
	while (imu.is_calibrating())
	{
		pros::delay(10);
	}
	std::cout << pros::millis() << ": finished calibrating!" << std::endl;
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


void autonomous() {
	chassis->setState({0_in, 0_in, 0_deg});
	float origAngle;
	auto chassisState = chassis->getState().str();
	
	// motor setup
	intakeMotors.setBrakeMode(AbstractMotor::brakeMode::hold);
	liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	if (autonSelection == 42) autonSelection = -3; // use debug if we havent selected any auton
	std::cout << "auton  " << autonSelection << std::endl;

	switch (autonSelection) {
	case 0:
		// skills doesnt exist.
		profileController->setTarget("balls", false);
		profileController->waitUntilSettled();
		profileController->setTarget("balls", true);
		profileController->waitUntilSettled();
		drive(1000, 1000);
		drive(-1000, -1000);

		// origAngle = imu.get_rotation();
		// profileController->generatePath({
		// 	{0_ft, 0_ft, 0_deg},  // Profile starting position, this will normally be (0, 0, 0)
		// 	{2_ft, 1_ft, 0_deg}},
		// 	"A" // Profile name
		// );
		// // to turn to a true angle after a s curve, use the initial imu reading as a base
		// profileController->setTarget("A");
		// profileController->waitUntilSettled();
		// profileController->setTarget("A", true, true);
		// profileController->waitUntilSettled();
		// turn((-90 - (origAngle + imu.get_rotation()))); // should turn 90 relative to position before s curve
		// chassisState = chassis->getState().str();
		break;

	case -1:
		// red unprotec
		// flip. out.
		flipout();
		// move forward and intake and get the 4 laid in a line
		intakeMotors.moveVelocity(200);
		drive(2500, 2500, 80);
		intakeMotors.moveVelocity(0);
		liftMotor.move_voltage(0);
		// back
		drive(-1300, -1300, 80);
		// turn for stack
		drive(600, -600, 80);
		// drive to stack
		intakeMotors.moveRelative(-700, 150);
		drive(1350, 1350, 80);
		// stack
		intakeMotors.moveVelocity(-20);
		liftMotor.move_absolute(-200, 100);
		trayMotor.move_absolute(6200, 100);
		while (trayMotor.get_position() < 6150) {
			pros::delay(20);
		}
		trayMotor.move_absolute(0, 100);
		drive(-800, -800, 50);
		break;

	case -2:
		// red protec
		break;
	
	case -3:
		// red unprotec 8 cube (rick)
		// flip. out.
		origAngle = imu.get_rotation();
		flipout();
		turn(-(origAngle + imu.get_rotation()));
		// move forward and intake and get the 4 laid in a line
		intakeMotors.moveVelocity(200);
		// drive(2400, 2400, 80);
		profileController->setTarget("balls", false);
		profileController->waitUntilSettled();
		intakeMotors.moveVelocity(0);
		liftMotor.move_voltage(0);
		// s curve to back and line up with next 4
		chassis->setMaxVelocity(150);
		turn(-(origAngle + imu.get_rotation()));
		
		profileController->setTarget("redRickSCurve", true);
		profileController->waitUntilSettled();
		// turn for next 4
		turn(-(origAngle + imu.get_rotation()));
		// intake next 4
		intakeMotors.moveVelocity(200);
		drive(2000, 2000, 80);
		intakeMotors.moveRelative(100, 100);
		turn(-(origAngle + imu.get_rotation())); // set heading to as close to 0 degrees as possible
		// point turn to face unprotec zone and drive to it
		turn(125);
		drive(2700, 2700);
		// stack
		intakeMotors.moveVelocity(-15);
		trayMotor.move_absolute(6200, 100);
		while (trayMotor.get_position() < 6100) {
			pros::delay(20);
		}
		intakeMotors.moveVelocity(15);
		trayMotor.move_absolute(0, 100);
		drive(-800, -800);
		intakeMotors.moveVelocity(0);
		break;
	
	case 1:
		// blue unprotec
		// flip. out.
		liftMotor.move_velocity(100);
		while (liftMotor.get_position() < 2000) { // wait until we initiate flipout
			pros::delay(20);
		}
		liftMotor.move_absolute(-200, 100);
		pros::delay(600);
		// move forward and intake and get the 4 laid in a line
		intakeMotors.moveVelocity(200);
		drive(2500, 2500, 80);
		intakeMotors.moveVelocity(0);
		liftMotor.move_voltage(0);
		// back
		drive(-1300, -1300, 80);
		// turn for stack
		drive(-600, 600, 80);
		// drive to stack
		intakeMotors.moveRelative(-700, 150);
		drive(1350, 1350, 80);
		// stack
		intakeMotors.moveVelocity(-20);
		liftMotor.move_absolute(-200, 100);
		trayMotor.move_absolute(6200, 100);
		while (trayMotor.get_position() < 6150) {
			pros::delay(20);
		}
		trayMotor.move_absolute(0, 100);
		drive(-800, -800, 50);
		break;
	case 2:
		// blue protec

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
	chassis->stop();
	chassis->getModel()->setBrakeMode(AbstractMotor::brakeMode::coast);

	// motor setup
	trayMotor.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

	// main loop
	while (true) {
		// basic lift control
		if (liftUp.isPressed()) liftMotor.move_velocity(100);
		else if (liftDown.isPressed()) liftMotor.move_velocity(-100);
		else if (liftMotor.get_position() < LIFT_STACKING_HEIGHT and liftMotor.get_position() > LIFT_STACKING_HEIGHT - 300) liftMotor.move_voltage(-2000); // basically to force the lift down but not burn the motor, shut off the motor when we stabalize at 0
		else if (liftMotor.get_position() < LIFT_STACKING_HEIGHT and liftMotor.get_efficiency() != 0) liftMotor.move_voltage(-3000);
		else liftMotor.move(0);

		// advanced intake control, with goal-oriented assists
		if (intakeIn.isPressed() and liftMotor.get_position() > LIFT_STACKING_HEIGHT) intakeMotors.moveVelocity(100); // if we are dumping into tower, redue intake velocity as not to shoot the cube halfway accross the field
		else if (intakeIn.isPressed()) intakeMotors.moveVelocity(200);
		else if (intakeOut.isPressed() and liftMotor.get_position() > LIFT_STACKING_HEIGHT) intakeMotors.moveVelocity(-100);
		else if (intakeOut.isPressed() or intakeShift.isPressed()) intakeMotors.moveVelocity(-200);
		else if (not shift.isPressed()) intakeMotors.moveVoltage(0);

		// tray control using shift key
		if (shift.isPressed()) { //shift key
			if (intakeIn.isPressed()) { // tray forward
				traySlew(true);
				// if tray and intake are interacting, move the intake
				intakeMotors.moveVelocity(-13);
		
			} else if (intakeOut.isPressed()) { // tray backward
				traySlew(false);
				intakeMotors.moveVelocity(13);
			}
			else if (not (intakeIn.isPressed() or intakeOut.isPressed())) intakeMotors.moveVoltage(0);
		}
		// adjust tray based on lift position
		else if (liftMotor.get_position() > LIFT_STACKING_HEIGHT) trayMotor.move_absolute(700, 100);
		else if (liftMotor.get_position() <= LIFT_STACKING_HEIGHT and trayMotor.get_position() < 2000) trayMotor.move_absolute(0, 100);
		else trayMotor.move(0);

		// tray stacking mods
		if (trayMotor.get_position() > 2000) { // brake and slow and intake coast when we stacking
			chassis->setMaxVelocity(150);
			liftMotor.move_absolute(-150, 100);
			intakeMotors.setBrakeMode(AbstractMotor::brakeMode::coast);
			chassis->getModel()->setBrakeMode(AbstractMotor::brakeMode::hold);
		}
		else { // return to normal after we stacked
			chassis->setMaxVelocity(200);
			intakeMotors.setBrakeMode(AbstractMotor::brakeMode::hold);
			chassis->getModel()->setBrakeMode(AbstractMotor::brakeMode::coast);
		}

		// lift brake mod
		if (liftMotor.get_position() > LIFT_STACKING_HEIGHT	) liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
		else liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

		chassis->getModel()->tank(masterController.getAnalog(ControllerAnalog::leftY),
								masterController.getAnalog(ControllerAnalog::rightY));

		// debug
		std::cout << pros::millis() << ": lift " << liftMotor.get_efficiency() << std::endl;
		// std::cout << pros::millis() << ": left " << chassis->getModel()->getSensorVals()[0] << std::endl;
		pros::delay(20);
	}
}