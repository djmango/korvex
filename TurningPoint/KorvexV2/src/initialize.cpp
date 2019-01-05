#include "main.h"
#include "korvexlib.h"

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

// called when a button is released or long pressed

int autonSelection = -3;

static lv_res_t redBtnmAction(lv_obj_t *btnm, const char *txt)
{
	printf("red button: %s released\n", txt);

	if (txt == "Close")
	{
		autonSelection = 3; // or whatever red close is
	}
	if (txt == "Far")
	{
		autonSelection = 4;
	}
	if (txt == "Stack")
	{
		autonSelection = 5;
	}

	return LV_RES_OK; // return OK because the button matrix is not deleted
}

static lv_res_t blueBtnmAction(lv_obj_t *btnm, const char *txt)
{
	printf("blue button: %s released\n", txt);

	if (txt == "Close")
	{
		autonSelection = 0;
	}
	if (txt == "Far")
	{
		autonSelection = 1;
	}
	if (txt == "Stack")
	{
		autonSelection = 2;
	}

		return LV_RES_OK; // return OK because the button matrix is not deleted
}

static lv_res_t skillsBtnAction(lv_obj_t *btn)
{
	uint8_t id = lv_obj_get_free_num(btn);

	printf("Button %d is released\n", id);

	return LV_RES_OK; // return OK because the button matrix is not deleted
}

/*Create a button descriptor string array*/
static const char *btnmMap[] = {"Close", "Far", "Stack", ""};

void initialize()
{
	// pros::lcd::initialize();
	// TODO: move the themes to src and revert the includes to default, compile locally
	// lvgl theme
	// lv_color_t c;
	// c.red = 0xCC;
	// c.green = 0xCC;
	// c.blue = 0x90;
	// c = LV_COLOR_HEX(ff1500);
	// lv_color16_t c16;
	// c16.full = lv_color_to16(c);

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

	// Create a default button matrix
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
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
	// chassis.moveDistanceAsync(0);
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
