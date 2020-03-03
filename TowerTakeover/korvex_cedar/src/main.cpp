#include <fstream>
#include "main.h"
#include "korvexlib.h"

// chassis
// auto chassis = ChassisControllerBuilder() // three tracking wheels
// 		.withMotors({LEFT_MTR2, LEFT_MTR1}, {-RIGHT_MTR2, -RIGHT_MTR1})
// 		// green gearset, 4 inch wheel diameter, 8.125 inch wheelbase
// 		.withDimensions(AbstractMotor::gearset::green, {{4_in, 8.125_in}, imev5GreenTPR})
// 		.withSensors(
// 			ADIEncoder{'A', 'B'}, // left encoder in ADI ports A & B
// 			ADIEncoder{'E', 'F'},  // right encoder in ADI ports E & F
// 			ADIEncoder{'C', 'D', true}  // middle encoder in ADI ports C & D, reversed
// 		)
// 		// specify the tracking wheels diameter (2.75 in), track (4.6 in), and TPR (360)
// 		// specify the middle encoder distance (9.25 in) and diameter (2.75 in)
// 		.withOdometry({{2.75_in, 4.6_in, 9.25_in, 2.75_in}, quadEncoderTPR})
// 		.buildOdometry(); // build an odometry chassis

auto chassis = ChassisControllerBuilder() // two tracking wheels
		.withMotors({LEFT_MTR2, LEFT_MTR1}, {-RIGHT_MTR2, -RIGHT_MTR1})
		// green gearset, 4 inch wheel diameter, 8.125 inch wheelbase
		.withDimensions(AbstractMotor::gearset::green, {{4_in, 8.125_in}, imev5GreenTPR})
		.withSensors(
			ADIEncoder{'A', 'B'}, // left encoder in ADI ports A & B
			ADIEncoder{'E', 'F'}  // right encoder in ADI ports E & F
		)
		// specify the tracking wheels diameter (2.75 in), track (4.6 in), and TPR (360)
		.withOdometry({{2.75_in, 4.6_in}, quadEncoderTPR})
		.buildOdometry(); // build an odometry chassis

auto profileController = AsyncMotionProfileControllerBuilder()
    .withLimits({1.0, 1.8, 5.0}) //double maxVel double maxAccel double maxJerk 
    .withOutput(chassis)
    .buildMotionProfileController();

// motors
pros::Motor liftMotor(LIFT_MTR, pros::E_MOTOR_GEARSET_36, false);
okapi::Motor trayMotor(TRAY_MTR, false, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::counts);
okapi::MotorGroup intakeMotors({INTAKE_MTR1, -INTAKE_MTR2});

// controller
Controller masterController;
ControllerButton liftUp(ControllerDigital::R1);
ControllerButton liftDown(ControllerDigital::R2);
ControllerButton intakeIn(ControllerDigital::L1);
ControllerButton intakeOut(ControllerDigital::L2);
ControllerButton intakeShift(ControllerDigital::right);
ControllerButton flipoutBtn(ControllerDigital::left);
ControllerButton shift(ControllerDigital::Y);
ControllerButton trayReturn(ControllerDigital::X);
ControllerButton trayReturnAlt(ControllerDigital::A);
ControllerButton cubeReturn(ControllerDigital::B);

// sensors
pros::Imu imu(IMU_PORT);
pros::ADIAnalogIn line(LINE_PORT); // line sensor on tray, for cube detection
pros::ADIEncoder trackingLeft(1, 2);
pros::ADIEncoder trackingRight(5, 6);
pros::ADIEncoder trackingStrafe(3, 4, true);

// base global defenitions
const int LIFT_STACKING_HEIGHT = 700; // the motor ticks above which we are stacking
enum class autonStates { // the possible auton selections
	off,
	redProtec,
	redUnprotec,
	redRick,
	blueProtec,
	blueUnprotec,
	blueRick,
	skills
};
autonStates autonSelection = autonStates::off; // the current auton selection

enum class trayStates { // the possible tray states
	returned,
	returning,
	extending,
};
trayStates trayState = trayStates::returned; // the current tray state

enum class cubeStates { // the cube(line) sensor states, in this order
	uncovered,
	covered,
	setting,
	settingCovered,
	finished
};
cubeStates cubeState = cubeStates::covered;

// odom debug global
bool odomDebug = false;

// create a button descriptor string array
static const char *btnmMap[] = {"Unprotec", "Protec", "Rick", ""};

void driveP(int targetLeft, int targetRight, int voltageMax=115, bool debugLog=false) {

	// the touchables ;)))))))) touch me uwu :):):)
	float kp = 0.15;
	float acc = 5;
	float kpTurn = 0.7;
	float accTurn = 4;

	// the untouchables
	float voltageLeft = 0;
	float voltageRight = 0;
	int errorLeft;
	int errorRight;
	int voltageCap = 0;
	int signLeft;
	int signRight;
	int errorCurrent = 0;
	int errorLast = 0;
	int sameErrCycles = 0;
	int same0ErrCycles = 0;
	int startTime = pros::millis();
	targetLeft = targetLeft + chassis->getModel()->getSensorVals()[0];
	targetRight = targetRight + chassis->getModel()->getSensorVals()[1];

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
		
		if(voltageCap > voltageMax) voltageCap = voltageMax; // voltageCap cannot exceed 115

		if(abs(voltageLeft) > voltageCap) voltageLeft = voltageCap * signLeft; // limit the voltage
		if(abs(voltageRight) > voltageCap) voltageRight = voltageCap * signRight;// ditto

		// set the motors to the intended speed
		chassis->getModel()->tank(voltageLeft/127, voltageRight/127); // dont feel like retuning soo we divide by 127

		// timeout utility
		if (errorLast == errorCurrent) {
			if (errorCurrent <= 2) same0ErrCycles +=1; // less than 2 ticks is "0" error
			sameErrCycles += 1;
		}
		else {
			sameErrCycles = 0;
			same0ErrCycles = 0;
		}

		// exit paramaters
		if ((errorLast < 5 and errorCurrent < 5) or sameErrCycles >= 10) { // allowing for smol error or exit if we stay the same err for .2 second
			chassis->stop();
			std::cout << pros::millis() << "task complete with error " << errorCurrent << " in " << (pros::millis() - startTime) << "ms" << std::endl;
			return;
		}
		
		// debug
		if (debugLog) {
			std::cout << pros::millis() << "error  " << errorCurrent << std::endl;
			std::cout << pros::millis() << "errorLeft  " << errorLeft << std::endl;
			std::cout << pros::millis() << "errorRight  " << errorRight << std::endl;
			std::cout << pros::millis() << "voltageLeft  " << voltageLeft << std::endl;
			std::cout << pros::millis() << "voltageRight  " << voltageRight << std::endl;
		}

		// nothing goes after this
		errorLast = errorCurrent;
		pros::delay(20);
	}
}

void driveQ(QLength targetX, QLength targetY, bool backwards=false, float voltageMax=115, bool debugLog=false) {

	// tune for straights
	float kp = 0.02;
	float ki = 0.02;
	float kd = 0.03;
	float acc = 5;

	// tune for turns
	float kpTurn = 0.0;
	float kiTurn = 0.03;
	float kdTurn = 0.0;

	// the untouchables
	float error; // distance to target
	float errorLast = 0; // error in the last loop
	float errorTheta; // targetTheta - robotTheta
	float errorLastTheta = 0; // errorTheta in the last loop
	float p; // proportional straight
	float i; // integral straight
	float d; // derivative straight
	float pTurn; // proportional turn
	float iTurn; // integral turn
	float dTurn; // derivative turn
	float voltageLeft;
	float voltageRight;
	float voltage; // calculated straight voltage
	float xDif = targetX.convert(centimeter) - chassis->getState().x.convert(centimeter); // target.x - robot.x
	float yDif = targetY.convert(centimeter) - chassis->getState().y.convert(centimeter); // target.y - robot.y
	float xOrig = chassis->getState().x.convert(centimeter); // original x coord
	float yOrig = chassis->getState().y.convert(centimeter); // original y coord
	float targetTheta = std::atan2(yDif,xDif)*180 / M_PI; // angle from origin to target
	float distanceTotal = std::sqrt(std::pow((targetX.convert(centimeter) - xOrig), 2) + std::pow((targetY.convert(centimeter) - yOrig), 2)); // total distance we need to travel
	float distanceOrig; // distance from original position
	int sameErrCycles = 0; // number of cycles we have stayed the same error
	int same0ErrCycles = 0; // number of cycles we have stayed at 0 error
	int startTime = pros::millis();

	if (backwards) targetTheta = std::atan(yDif/xDif)*180 / M_PI;
	voltageMax = voltageMax/127; // normalize the voltageMax

	while(autonomous) {

		// get difference in x and y, robot distance from target
		xDif = targetX.convert(centimeter) - chassis->getState().x.convert(centimeter);
		yDif = targetY.convert(centimeter) - chassis->getState().y.convert(centimeter);

		// get difference in x and y, robot distance from move start, to detect overshoot
		distanceOrig = std::sqrt(std::pow((chassis->getState().x.convert(centimeter) - xOrig), 2) + std::pow((chassis->getState().y.convert(centimeter) - yOrig), 2));

		// get distance to target, ie error
		error = std::sqrt(std::pow(xDif, 2) + std::pow(yDif, 2));

		p = (error * kp);
		if (abs(error) <= 10) i = ((i + error) * ki); // if we are in range for I to be desireable
		else i = 0;
		d = (error - errorLast) * kd;
		
		// set voltage
		voltage = p + i + d;

		if (voltage > voltageMax) {voltage = voltageMax;}
		if (distanceOrig > distanceTotal) {voltage = -voltage;} // if we have passed our point
		if (backwards) {voltage = -voltage;} // if we are driving backwards

		voltageLeft = voltage;
		voltageRight = voltage;

		// figure out if we are turned away from our target
		errorTheta = targetTheta - imu.get_rotation();

		// calculate voltage change for left/right
		pTurn = (error * kpTurn);
		iTurn = ((iTurn + errorTheta) * kiTurn); // if we are in range for I to be desireable
		dTurn = (errorTheta - errorLastTheta) * kdTurn;

		voltageLeft = voltageLeft + (pTurn + iTurn + dTurn);
		voltageRight = voltageRight + -(pTurn + iTurn + dTurn);


		// set the motors to the intended speed
		chassis->getModel()->tank(voltageLeft, voltageRight);

		// timeout utility
		if (std::round(errorLast) == std::round(error)) {
			if (abs(error) <= 3) same0ErrCycles +=1; // less than 3 cm is "0" error
			sameErrCycles += 1;
		}
		else {
			sameErrCycles = 0;
			same0ErrCycles = 0;
		}

		// exit paramaters
		if ((same0ErrCycles > 15) or sameErrCycles >= 20) { // exit if we stay the same 0err for .3 sec or same err for .4 second
			chassis->stop();
			std::cout << pros::millis() << "task complete with error " << error << "cm, in " << (pros::millis() - startTime) << "ms" << std::endl;
			return;
		}

		// debug
		if (debugLog) {
			std::cout << pros::millis() << ": error  " << error << std::endl;
			std::cout << pros::millis() << ": errorTheta  " << errorTheta << std::endl;
			std::cout << pros::millis() << ": targetTheta  " << targetTheta << std::endl;
			std::cout << pros::millis() << ": voltageLeft " << voltageLeft << std::endl;
			std::cout << pros::millis() << ": voltageRight  " << voltageRight << std::endl;
			// std::cout << pros::millis() << ": xDif  " << xDif << std::endl;
			// std::cout << pros::millis() << ": yDif  " << yDif << std::endl;
			std::cout << pros::millis() << ": dist from orig  " << distanceOrig << std::endl;
		}

		// nothing goes after this
		errorLast = error;
		errorLastTheta = errorTheta;
		pros::delay(20);
	}
}

void turnP(int targetTurn, int voltageMax=127, bool debugLog=false) {
 
	// the touchables ;)))))))) touch me uwu :):):)
	float kp = 1.6;
	float ki = 0.8;
	float kd = 0.45;

	// the untouchables
	int voltageCap = 0;
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

	while(autonomous) {
		error = targetTurn - imu.get_rotation();
		errorCurrent = abs(error);
		errorCurrentInt = errorCurrent;
		sign = targetTurn / abs(targetTurn); // -1 or 1

		p = (error * kp);
		if (abs(error) < 10) { // if we are in range for I to be desireable
			i = ((i + error) * ki);
		}
		else
			i = 0;
		d = (error - errorLast) * kd;
		
		voltage = p + i + d;

		if(abs(voltage) > voltageMax) voltage = voltageMax * sign;

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
			std::cout << pros::millis() << "task complete with error " << errorCurrent << " in " << (pros::millis() - startTime) << "ms" << std::endl;
			return;
		}
		
		// debug
		// std::cout << pros::millis() << "error " << errorCurrent << std::endl;
		// std::cout << pros::millis() << "voltage " << voltage << std::endl;

		// for csv output, graphing the function
		if (debugLog) std::cout << pros::millis() << "," << error << "," << voltage << std::endl;

		// nothing goes after this
		errorLast = errorCurrent;
		errorLastInt = errorLast;
		pros::delay(10);
	}
}

void turnQ(QLength targetX, QLength targetY, bool backwards=false, bool debugLog=false) {
	// tune for turns
	float kp = 0.02;
	float ki = 0.018;
	float kd = 0.037;

	// the untouchables
	float errorTheta; // targetTheta - robotTheta
	float errorLastTheta = 0; // errorTheta in the last loop
	float p; // proportional
	float i; // integral
	float d; // derivative
	float voltage; // calculated voltage
	float xDif = targetX.convert(centimeter) - chassis->getState().x.convert(centimeter); // target.x - robot.x
	float yDif = targetY.convert(centimeter) - chassis->getState().y.convert(centimeter); // target.y - robot.y
	float targetTheta = std::atan2(yDif,xDif)*180 / M_PI; // angle from robot to target, our goal angle
	int sameErrCycles = 0; // number of cycles we have stayed the same error
	int same0ErrCycles = 0; // number of cycles we have stayed at 0 error
	int startTime = pros::millis();

	if (backwards) targetTheta = std::atan(yDif/xDif)*180 / M_PI;

	while(autonomous) {

		// get difference in x and y, robot distance from target
		errorTheta = targetTheta - imu.get_rotation();

		p = (errorTheta * kp);
		if (abs(errorTheta) < 10) i = ((i + errorTheta) * ki); // if we are in range for I to be desireable
		else i = 0;
		d = (errorTheta - errorLastTheta) * kd;
		
		voltage = p + i + d;

		// set the motors to the intended speed
		chassis->getModel()->tank(voltage, -voltage);

		// timeout utility
		if (std::round(errorLastTheta) == std::round(errorTheta)) {
			if (abs(errorTheta) <= 4) same0ErrCycles +=1; // less than 4 deg is "0" error
			sameErrCycles += 1;
		}
		else {
			sameErrCycles = 0;
			same0ErrCycles = 0;
		}

		// exit paramaters
		if ((same0ErrCycles > 5) or sameErrCycles >= 15) { // exit if we stay the same 0err for .1 sec or same err for .3 second
			chassis->stop();
			std::cout << pros::millis() << "task complete with error " << errorTheta << "deg, in " << (pros::millis() - startTime) << "ms" << std::endl;
			return;
		}

		// debug
		// if (debugLog) {
		// 	std::cout << pros::millis() << ": errorTheta  " << errorTheta << std::endl;
		// 	std::cout << pros::millis() << ": targetTheta  " << targetTheta << std::endl;
		// }
		if (debugLog) std::cout << pros::millis() << "," << errorTheta << "," << voltage*50 << std::endl;

		// nothing goes after this
		errorLastTheta = errorTheta;
		pros::delay(20);
	}
}

void driveTo(QLength targetX, QLength targetY, bool backwards=false, int voltageMax=115, bool debugLog=false) {
	float targetTheta;
	if (backwards) targetTheta = std::atan((targetY.convert(centimeter) - chassis->getState().y.convert(centimeter))/(targetX.convert(centimeter) - chassis->getState().x.convert(centimeter)))*180 / M_PI;
	else targetTheta = std::atan2((targetY.convert(centimeter) - chassis->getState().y.convert(centimeter)), (targetX.convert(centimeter) - chassis->getState().x.convert(centimeter)))*180 / M_PI;
	if (abs(targetTheta - imu.get_rotation()) > 20) {turnQ(targetX, targetY, backwards, debugLog);} // only turn if the degree error is greater than 20 deg
	driveQ(targetX, targetY, backwards, voltageMax, debugLog);
}

void traySlew(bool forward) {
	if (forward) {
		if (trayMotor.getPosition() > 4500) trayMotor.moveVelocity(40);
		else trayMotor.moveVelocity(100);
	}
	else {
		if (trayMotor.getPosition() < 1000) trayMotor.moveVelocity(-60);
		else trayMotor.moveVelocity(-100);
	}
}

void generatePaths() { // all motion profile paths stored here, no real error correction in these
	// 8 cube s curve, mirror for red
	profileController->generatePath({
				{0_ft, 0_ft, 0_deg},
				{40_in, 10_in, 0_deg}},
				"9cCurve1" 
			);
}

// just update calculated theta to actual theta using the imu
void odomImuSupplement (void*) {
	while (true) {
		chassis->setState({chassis->getState().x, chassis->getState().y, (((imu.get_rotation()*M_PI)/180) * okapi::radian)});
		if (odomDebug) std::cout << pros::millis() << ": pos  " << chassis->getState().str() << std::endl;
		pros::delay(20);
	}

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

	masterController.rumble("..");
	return LV_RES_OK; // return OK because the button matrix is not deleted
}

static lv_res_t skillsBtnAction(lv_obj_t *btn) {
	masterController.rumble("..");
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
	line.calibrate();
	std::cout << pros::millis() << ": calibrating line tracker..." << std::endl;

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

	// generate paths
	std::cout << pros::millis() << ": generating paths..." << std::endl;
	generatePaths();
	std::cout << pros::millis() << ": finished generating paths..." << std::endl;

	// wait for calibrate
	while (imu.is_calibrating() and pros::millis() < 3000) pros::delay(20);
	if (pros::millis() < 3000) std::cout << pros::millis() << ": finished calibrating!" << std::endl;
	else std::cout << pros::millis() << ": calibration failed, moving on" << std::endl;

	// start imu implementation to odom
	pros::Task odomImuSupplementTask(odomImuSupplement, (void*)NULL, TASK_PRIORITY_DEFAULT-1, TASK_STACK_DEPTH_DEFAULT, "odomImuSUpplement");
	std::cout << pros::millis() << " odomImuSupplement state:" << odomImuSupplementTask.get_state();

	// log motor temps
	std::cout << pros::millis() << "\n" << pros::millis() << ": motor temps:" << std::endl;
	std::cout << pros::millis() << ": lift: " << liftMotor.get_temperature() << std::endl;
	std::cout << pros::millis() << ": tray: " << trayMotor.getTemperature() << std::endl;
	std::cout << pros::millis() << ": intake: " << intakeMotors.getTemperature() << std::endl;
	
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
 * Management System or the VEX Competition Switch.
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
	chassis->setState({0_cm, 0_cm, 0_deg});
	chassis->setMaxVelocity(200);
	chassis->getModel()->setBrakeMode(AbstractMotor::brakeMode::hold);
	float origAngle = imu.get_rotation();
	
	// motor setup
	intakeMotors.setBrakeMode(AbstractMotor::brakeMode::hold);
	liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	
	auto timer = TimeUtilFactory().create().getTimer();
	timer->placeMark();

	if (autonSelection == autonStates::off) autonSelection = autonStates::redUnprotec; // use debug if we havent selected any auton

	switch (autonSelection) {
	case autonStates::skills:
		// skills doesnt exist
		break;

	case autonStates::redUnprotec:
		// red unprotec 7 cube
		// flipout
		chassis->getModel()->tank(0.3, 0.3);
		intakeMotors.moveVelocity(200);
		liftMotor.move_absolute(400, 200);
		while(line.get_value_calibrated_HR() > 46000) pros::delay(20); // wait for the cube to get to position
		intakeMotors.moveRelative(1300, 200);
		while(intakeMotors.getPositionError() > 10) pros::delay(20);
		intakeMotors.moveRelative(-1000, 200);
		pros::delay(200);
		while(intakeMotors.getPositionError() > 5) pros::delay(20);
		pros::delay(200);
		liftMotor.move_absolute(100, 50);
		// grab first 3 cubes
		intakeMotors.moveRelative(4500, 200);
		driveTo(28_in, 0_in, false, 70);
		// drive for next line of cubes
		intakeMotors.moveRelative(-300, 200);
		driveTo(8_in, 24_in, true);
		// grab the 4 line
		turnQ(46_in, 24_in); // this is seperate so that intake doesnt cause noise
		intakeMotors.moveRelative(7800, 200);
		driveQ(46_in, 24_in, false, 80);
		// move to zone
		turnQ(9_in, 45_in);
		intakeMotors.moveRelative(-700, 50);
		trayMotor.moveAbsolute(5000, 60);
		driveTo(9_in, 45_in);
		trayMotor.moveAbsolute(6200, 100);
		// stack
		intakeMotors.moveVelocity(-20);
		liftMotor.move_absolute(-20, 100);
		while (trayMotor.getPosition() < 6100) pros::delay(20);
		trayMotor.moveAbsolute(0, 100);
		intakeMotors.moveVelocity(-50);
		driveP(250, 250);
		driveP(-600, -600, 80);
		intakeMotors.moveVelocity(0);
		break;

	case autonStates::redProtec:
		// red protec 4 cube
		// grab 3 cubes
		intakeMotors.moveRelative(5000, 200);
		driveP(1700, 1700, 70);
		intakeMotors.moveVoltage(0);
		// turn for final cube
		turnP(-132);
		// grab final cube
		intakeMotors.moveRelative(5000, 200);
		driveP(1300, 1300, 70);
		// move to zone
		turnP(-136);
		driveP(580, 580);
		// stack
		intakeMotors.moveRelative(-1300, 90);
		liftMotor.move_absolute(-20, 100);
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
		driveP(250, 250);
		driveP(-800, -800, 80);
		intakeMotors.moveVelocity(0);
		break;
	
	case autonStates::redRick:
		// red protec 3 cube
		// grab 3 cubes
		intakeMotors.moveRelative(5000, 200);
		driveP(900, 900, 70);
		intakeMotors.moveVoltage(0);
		// turn for final cube
		turnP(-90);
		// grab final cube
		intakeMotors.moveRelative(4000, 200);
		driveP(900, 900, 70);
		// move to zone
		turnP(-132);
		driveP(520, 520);
		// stack
		intakeMotors.moveRelative(-900, 200);
		liftMotor.move_absolute(-20, 100);
		trayMotor.moveAbsolute(6200, 100);
		while (trayMotor.getPosition() < 6000) {
			pros::delay(20);
		}
		trayMotor.moveAbsolute(0, 100);
		intakeMotors.moveVelocity(-50);
		driveP(250, 250);
		driveP(-600, -600, 80);
		intakeMotors.moveVelocity(0);
		break;
	
	case autonStates::blueUnprotec:
		// blue unprotec 7 cube
		// flipout
		chassis->getModel()->tank(0.3, 0.3);
		intakeMotors.moveVelocity(200);
		liftMotor.move_absolute(400, 200);
		while(line.get_value_calibrated_HR() > 46000) pros::delay(20); // wait for the cube to get to position
		intakeMotors.moveRelative(1300, 200);
		while(intakeMotors.getPositionError() > 10) pros::delay(20);
		intakeMotors.moveRelative(-1000, 200);
		pros::delay(200);
		while(intakeMotors.getPositionError() > 5) pros::delay(20);
		pros::delay(200);
		liftMotor.move_absolute(100, 50);
		// grab first 3 cubes
		intakeMotors.moveRelative(4500, 200);
		driveTo(28_in, 0_in, false, 70);
		// drive for next line of cubes
		intakeMotors.moveRelative(-300, 200);
		driveTo(8_in, -24_in, true);
		// grab the 4 line
		turnQ(46_in, -24_in); // this is seperate so that intake doesnt cause noise
		intakeMotors.moveRelative(7800, 200);
		driveQ(46_in, -24_in, false, 80);
		// move to zone
		turnQ(9_in, -45_in);
		intakeMotors.moveRelative(-700, 50);
		trayMotor.moveAbsolute(4000, 60);
		driveTo(9_in, -45_in);
		trayMotor.moveAbsolute(6200, 100);
		// stack
		intakeMotors.moveVelocity(-20);
		liftMotor.move_absolute(-20, 100);
		while (trayMotor.getPosition() < 6100) pros::delay(20);
		trayMotor.moveAbsolute(0, 100);
		intakeMotors.moveVelocity(-50);
		driveP(250, 250);
		driveP(-600, -600, 80);
		intakeMotors.moveVelocity(0);
		break;
	case autonStates::blueProtec:
		// blue protec 4 cube
		// grab 3 cubes
		intakeMotors.moveRelative(5000, 200);
		driveP(1700, 1700, 80);
		intakeMotors.moveVoltage(0);
		// turn for final cube
		turnP(130);
		// grab final cube
		intakeMotors.moveRelative(5400, 200);
		driveP(1300, 1300, 70);
		// move to zone
		driveP(550, 550);
		intakeMotors.moveRelative(-900, 90);
		// stack
		liftMotor.move_absolute(-20, 100);
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
		driveP(-800, -800, 80);
		intakeMotors.moveVelocity(0);
		break;
	case autonStates::blueRick:
		// blue protec 3 cube
		// grab 3 cubes
		intakeMotors.moveRelative(5000, 200);
		driveP(900, 900, 70);
		intakeMotors.moveVoltage(0);
		// turn for final cube
		turnP(90);
		// grab final cube
		intakeMotors.moveRelative(4000, 200);
		driveP(900, 900, 70);
		// move to zone
		turnP(132);
		driveP(520, 520);
		// stack
		intakeMotors.moveRelative(-900, 200);
		liftMotor.move_absolute(-20, 100);
		trayMotor.moveAbsolute(6200, 100);
		while (trayMotor.getPosition() < 6000) {
			pros::delay(20);
		}
		trayMotor.moveAbsolute(0, 100);
		intakeMotors.moveVelocity(-50);
		driveP(250, 250);
		driveP(-600, -600, 80);
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

void opcontrol() {
	chassis->stop();
	chassis->getModel()->setBrakeMode(AbstractMotor::brakeMode::coast);
	trayMotor.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
	chassis->setMaxVelocity(200);
	float joystickAvg = 0; // an average of the left and right joystick values
	bool cubesPositioning = false; // true when we are moving the cubes down to the line sensor, for stacking
	bool trayDebug = false; // im lazy
	bool cubeDebug = false; // still lazy

	// main loop
	while (true) {
		// basic lift control
		if (liftUp.isPressed()) liftMotor.move_velocity(100);
		else if (liftDown.isPressed()) liftMotor.move_velocity(-100);
		else if (liftMotor.get_position() < LIFT_STACKING_HEIGHT and liftMotor.get_position() > LIFT_STACKING_HEIGHT - 300) liftMotor.move_voltage(-2000); // basically to force the lift down but not burn the motor, shut off the motor when we stabalize at 0
		else if (liftMotor.get_position() < LIFT_STACKING_HEIGHT and liftMotor.get_efficiency() > 50) liftMotor.move_voltage(-2000);
		else liftMotor.move(0);

		// flipout routine
		if (flipoutBtn.changedToPressed()) {
			intakeMotors.moveVelocity(200);
			liftMotor.move_absolute(400, 200);
			while(line.get_value_calibrated_HR() > 46000) pros::delay(20); // wait for the cube to get to position
			intakeMotors.moveRelative(1300, 200);
			while(intakeMotors.getPositionError() > 10) pros::delay(20);
			intakeMotors.moveRelative(-1000, 200);
			pros::delay(200);
			while(intakeMotors.getPositionError() > 5) pros::delay(20);
			pros::delay(200);
			liftMotor.move_absolute(100, 50);
		}

		// advanced intake control, with goal-oriented assists

		// auto cube positioning for stacking
		if (cubeReturn.isPressed()) cubesPositioning = true; // outake until we detect cube

		if (cubesPositioning) {
			if (cubeState == cubeStates::settingCovered) {
				if (abs(intakeMotors.getPositionError()) <= 20) { // we finished setting the cube
					cubeState = cubeStates::finished;
					cubesPositioning = 0;
					if (cubeDebug) std::cout << pros::millis() << ": cubeState finished" << std::endl;
				} 
			}
			else if (line.get_value_calibrated_HR() < 46000) { // if we are already covering, move up to uncover
				if (cubeState == cubeStates::setting) { // this means we have found cube position, so we must move it to its final position
					intakeMotors.moveRelative(-280, 100);
					cubeState = cubeStates::settingCovered;
					if (cubeDebug) std::cout << pros::millis() << ": cubeState settingCovered" << std::endl;
				}
				else intakeMotors.moveVelocity(100);
				if (cubeDebug) std::cout << pros::millis() << ": cubeState uncovering" << std::endl;
			}
			else { // if we arent covering the sensor and we arent setting the final position
				intakeMotors.moveVelocity(-100);
				cubeState = cubeStates::setting;
				if (cubeDebug) std::cout << pros::millis() << ": cubeState setting" << std::endl;
			}
		}

		// user controlled intake only enabled while returned
		if (trayState == trayStates::returned and not shift.isPressed()) { // if nothing else is controlling the intake and we arent moving the tray
			if (intakeIn.isPressed() and not intakeOut.isPressed() and liftMotor.get_position() > LIFT_STACKING_HEIGHT) intakeMotors.moveVelocity(100); // if we are dumping into tower, redue intake velocity as not to shoot the cube halfway accross the field
			else if (intakeIn.isPressed() and not intakeOut.isPressed()) intakeMotors.moveVelocity(200);
			else if (intakeOut.isPressed() and liftMotor.get_position() > LIFT_STACKING_HEIGHT) intakeMotors.moveVelocity(-100);
			else if (intakeOut.isPressed() or (intakeShift.isPressed() and liftMotor.get_position() > LIFT_STACKING_HEIGHT)) intakeMotors.moveVelocity(-200);
			else if (not cubesPositioning) intakeMotors.moveVoltage(0);
		}
		
		// advanced tray control, also with goal-oriented assists
		if (trayReturn.changedToPressed()) { // manual tray toggle requests, this is highest priority control
			if (trayState == trayStates::returned) { // if we are already returned, move the tray out
				intakeMotors.setBrakeMode(AbstractMotor::brakeMode::coast);
				chassis->getModel()->setBrakeMode(AbstractMotor::brakeMode::hold);
				liftMotor.move_absolute(0, 100);
				trayMotor.moveAbsolute(6400, 100);
				trayState = trayStates::extending;
			}
			else { // return to default tray position
				intakeMotors.setBrakeMode(AbstractMotor::brakeMode::hold);
				chassis->getModel()->setBrakeMode(AbstractMotor::brakeMode::coast);
				trayMotor.moveAbsolute(0, 100);
				trayState = trayStates::returning;
			}
		}
		else if (trayReturnAlt.changedToPressed()) { // a slower, further tray movement for high stacks
			if (trayState == trayStates::returned) { // if we are already returned, move the tray out
				intakeMotors.setBrakeMode(AbstractMotor::brakeMode::coast);
				chassis->getModel()->setBrakeMode(AbstractMotor::brakeMode::hold);
				liftMotor.move_absolute(0, 100);
				trayMotor.moveAbsolute(6600, 65);
				trayState = trayStates::extending;
			}
			else { // return to default tray position
				intakeMotors.setBrakeMode(AbstractMotor::brakeMode::hold);
				chassis->getModel()->setBrakeMode(AbstractMotor::brakeMode::coast);
				trayMotor.moveAbsolute(0, 100);
				trayState = trayStates::returning;
			}
		}

		// update trayState
		if (trayMotor.getPosition() <= 100 and abs(trayMotor.getActualVelocity()) <= 5 and trayState != trayStates::extending) trayState = trayStates::returned; // let functions know if weve returned
		else if (trayMotor.getPosition() >= 6350) trayState = trayStates::returning;

		// tray control using shift key
		if (trayState == trayStates::returned) {
			if (shift.isPressed()) {
				if (intakeIn.isPressed()) traySlew(true);
				else if (intakeOut.isPressed()) traySlew(false);
				else trayMotor.moveVoltage(0);
			}
			// adjust tray based on lift position
			else if (liftMotor.get_position() > LIFT_STACKING_HEIGHT) trayMotor.moveAbsolute(600, 100);
			else if (liftMotor.get_position() <= LIFT_STACKING_HEIGHT and trayMotor.getPosition() <= 600) trayMotor.moveAbsolute(0, 100);
			else trayMotor.moveVoltage(0);
		}

		// tray stacking mods
		switch (trayState) {
			case trayStates::returned:
				intakeMotors.setBrakeMode(AbstractMotor::brakeMode::hold);
				chassis->getModel()->setBrakeMode(AbstractMotor::brakeMode::coast);
				if (trayDebug) std::cout << pros::millis() << ": trayState returned" << std::endl;
				break;
			case trayStates::returning:
				if (intakeIn.isPressed() and not intakeOut.isPressed()) intakeMotors.moveVelocity(200);
				else if (intakeOut.isPressed() or (intakeShift.isPressed())) intakeMotors.moveVelocity(-200);
				else if (joystickAvg > 0) intakeMotors.moveVoltage(0);
				else intakeMotors.moveVelocity((joystickAvg*350));
				chassis->getModel()->setBrakeMode(AbstractMotor::brakeMode::coast);
				if (trayDebug) std::cout << pros::millis() << ": trayState returning" << std::endl;
				break;
			case trayStates::extending:
				liftMotor.move_absolute(-150, 100);
				chassis->getModel()->setBrakeMode(AbstractMotor::brakeMode::hold);

				// the other intake control will not be used while we are stacking, all control is transfered to this block
				if (not shift.isPressed() and not shift.changedToReleased()) {
					if (intakeIn.isPressed()) intakeMotors.moveVelocity(50);
					else if (intakeOut.isPressed() or intakeShift.isPressed()) intakeMotors.moveVelocity(-50); // two ways to move stack down slowly
					else intakeMotors.moveVoltage(0);
				}
				if (trayDebug) std::cout << pros::millis() << ": trayState extending" << std::endl;
				break;
			default:
				break;
		}

		// lift brake mod
		if (liftMotor.get_position() > LIFT_STACKING_HEIGHT) liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
		else liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

		chassis->getModel()->tank(masterController.getAnalog(ControllerAnalog::leftY),
								masterController.getAnalog(ControllerAnalog::rightY));


		// update vals
		joystickAvg = (masterController.getAnalog(ControllerAnalog::leftY) + (masterController.getAnalog(ControllerAnalog::rightY)) / 2);

		// debug
		// std::cout << pros::millis() << ": line " << line.get_value_calibrated_HR() << std::endl;
		// std::cout << pros::millis() << ": lef " << trackingLeft.get_value() << std::endl;
		// std::cout << pros::millis() << ": rig " << trackingRight.get_value() << std::endl;
		pros::delay(20);
	}
}