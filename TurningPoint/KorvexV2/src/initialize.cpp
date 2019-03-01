#include "main.h"
#include "korvexlib.h"

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

// called when a button is released or long pressed

// base global defenitions
int autonSelection = 10;
bool autonPark = true;
int timeHold = 0; // just so we have this later
bool ballTriggerBottom = false;
bool ballTriggerTop = false;

static lv_res_t redBtnmAction(lv_obj_t *btnm, const char *txt)
{
	printf("red button: %s released\n", txt);

	if (txt == "Close")
	{
		autonSelection = -1; // or whatever red close is
	}
	if (txt == "Far")
	{
		autonSelection = -2;
	}
	if (txt == "Descore")
	{
		autonSelection = -3;
	}

	return LV_RES_OK; // return OK because the button matrix is not deleted
}

static lv_res_t blueBtnmAction(lv_obj_t *btnm, const char *txt)
{
	printf("blue button: %s released\n", txt);

	if (txt == "Close")
	{
		autonSelection = 1;
	}
	if (txt == "Far")
	{
		autonSelection = 2;
	}
	if (txt == "Descore")
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

static lv_res_t parkBtnAction(lv_obj_t *btn)
{
	uint8_t id = lv_obj_get_free_num(btn);

	printf("Btn %d is long pressed %d\n", btn);
	autonPark = false;

	return LV_RES_OK;
}

/*Create a button descriptor string array*/
static const char *btnmMap[] = {"Close", "Far", "Descore", ""};

void initialize()
{
	// lvgl theme

	lv_theme_t *th = lv_theme_alien_init(280, NULL); //Set a HUE value and keep font default
	lv_theme_set_current(th);

	// create a tab view object
	lv_obj_t *tabview;
	tabview = lv_tabview_create(lv_scr_act(), NULL);

	// add 3 tabs (the tabs are page (lv_page) and can be scrolled
	lv_obj_t *redTab = lv_tabview_add_tab(tabview, "Red");
	lv_obj_t *blueTab = lv_tabview_add_tab(tabview, "Blue");
	lv_obj_t *skillsTab = lv_tabview_add_tab(tabview, "Skills");

	// add content to the tabs

	// red tab

	// button matrix
	lv_obj_t *redBtnm = lv_btnm_create(redTab, NULL);
	lv_btnm_set_map(redBtnm, btnmMap);
	lv_btnm_set_action(redBtnm, redBtnmAction);
	lv_obj_set_size(redBtnm, 450, 50);
	lv_obj_set_pos(redBtnm, 0, 100);
	lv_obj_align(redBtnm, NULL, LV_ALIGN_CENTER, 0, 0);

	// auton switches
	lv_obj_t *redBtn1 = lv_btn_create(redTab, NULL);
	lv_obj_t *redBtn1Label = lv_label_create(redBtn1, NULL);
	lv_label_set_text(redBtn1Label, "Park");
	lv_btn_set_action(redBtn1, LV_BTN_ACTION_LONG_PR, parkBtnAction);
	lv_obj_set_size(redBtn1, 100, 40);
	lv_obj_set_pos(redBtn1, 0, 200);
	lv_obj_align(redBtn1, NULL, LV_ALIGN_CENTER, 0, 0);

	// blue tab
	lv_obj_t *blueBtnm = lv_btnm_create(blueTab, NULL);
	lv_btnm_set_map(blueBtnm, btnmMap);
	lv_btnm_set_action(blueBtnm, blueBtnmAction);
	lv_obj_set_size(blueBtnm, 450, 50);
	lv_obj_set_pos(blueBtnm, 0, 100);
	lv_obj_align(blueBtnm, NULL, LV_ALIGN_CENTER, 0, 0);

	// auton switches
	lv_obj_t *blueBtn1 = lv_btn_create(blueTab, NULL);
	lv_obj_t *blueBtn1Label = lv_label_create(blueBtn1, NULL);
	lv_label_set_text(blueBtn1Label, "Park");
	lv_btn_set_action(blueBtn1, LV_BTN_ACTION_LONG_PR, parkBtnAction);
	lv_obj_set_size(blueBtn1, 100, 40);
	lv_obj_set_pos(blueBtn1, 0, 200);
	lv_obj_align(blueBtn1, NULL, LV_ALIGN_CENTER, 0, 0);

	// skills tab
	lv_obj_t *skillsBtn = lv_btn_create(skillsTab, NULL);
	lv_obj_t *label = lv_label_create(skillsBtn, NULL);
	lv_label_set_text(label, "Skills");
	lv_btn_set_action(skillsBtn, LV_BTN_ACTION_CLICK, skillsBtnAction);
	// lv_btn_set_state(skillsBtn, LV_BTN_STATE_TGL_REL);
	lv_obj_set_size(skillsBtn, 450, 50);
	lv_obj_set_pos(skillsBtn, 0, 100);
	lv_obj_align(skillsBtn, NULL, LV_ALIGN_CENTER, 0, 0);

	// flywheel motor group setup
	flywheelController.setGearing(okapi::AbstractMotor::gearset::blue);
	flywheelController.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);

	okapi::ADIGyro gyro(1, 1); // port, multiplier
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled()
{
	chassis.stop();
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
