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
static const char *btnmMap[] = {"Unprotec", "Protec", "Rick", ""};

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
	chassis->getModel()->tank(1,1);
	pros::delay(3000);
	chassis->getModel()->tank(-1,-1);
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
		else if (liftMotor.get_position() < LIFT_STACKING_HEIGHT and liftMotor.get_efficiency() > 50) liftMotor.move_voltage(-2000);
		else liftMotor.move(0);

		// advanced intake control, with goal-oriented assists
		if (intakeIn.isPressed() and not intakeOut.isPressed() and liftMotor.get_position() > LIFT_STACKING_HEIGHT) intakeMotors.moveVelocity(100); // if we are dumping into tower, redue intake velocity as not to shoot the cube halfway accross the field
		else if (intakeIn.isPressed() and not intakeOut.isPressed()) intakeMotors.moveVelocity(200);
		else if (intakeOut.isPressed() and liftMotor.get_position() > LIFT_STACKING_HEIGHT) intakeMotors.moveVelocity(-100);
		else if (intakeOut.isPressed() or intakeShift.isPressed()) intakeMotors.moveVelocity(-200);
		else if (not shift.isPressed()) intakeMotors.moveVoltage(0);

		// tray control using shift key
		if (shift.isPressed()) { //shift key
			if (intakeIn.isPressed()) { // tray forward
				// traySlew(true);
				// if tray and intake are interacting, move the intake
				intakeMotors.moveVelocity(-13);
		
			} else if (intakeOut.isPressed()) { // tray backward
				// traySlew(false);
				intakeMotors.moveVelocity(13);
			}
			else if (not (intakeIn.isPressed() or intakeOut.isPressed())) intakeMotors.moveVoltage(0);
		}
		// adjust tray based on lift position
		else if (liftMotor.get_position() > LIFT_STACKING_HEIGHT) trayMotor.move_absolute(700, 100);
		else if (liftMotor.get_position() <= LIFT_STACKING_HEIGHT and trayMotor.get_position() < 1400) trayMotor.move_absolute(0, 100);
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
		// std::cout << pros::millis() << ": lift " << liftMotor.get_efficiency() << std::endl;
		// std::cout << pros::millis() << ": left " << chassis->getModel()->getSensorVals()[0] << std::endl;
		pros::delay(20);
	}
}