#include "main.h"

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	// pros::lcd::initialize();
	// TODO: move the themes to src and revert the includes to default, compile locally
	// lvgl theme
	lv_color_t c;
	// c.red = 0xCC;
	// c.green = 0xCC;
	// c.blue = 0x90;
	c = LV_COLOR_HEX(0x00ff80);
	lv_color16_t c16;
	c16.full = lv_color_to16(c);

	lv_theme_t *th = lv_theme_alien_init(c16.full, NULL); //Set a HUE value and keep font default
	lv_theme_set_current(th);

	/*Create a Tab view object*/
	lv_obj_t *tabview;
	tabview = lv_tabview_create(lv_scr_act(), NULL);

	/*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
	lv_obj_t *redTab = lv_tabview_add_tab(tabview, "Red");
	lv_obj_t *blueTab = lv_tabview_add_tab(tabview, "Blue");
	lv_obj_t *skillsTab = lv_tabview_add_tab(tabview, "Skills");

	/*Add content to the tabs*/
	lv_obj_t *label = lv_label_create(redTab, NULL);
	lv_label_set_text(label, "This the first tab\n\n"
							 "If the content\n"
							 "become too long\n"
							 "the tab become\n"
							 "scrollable\n\n");

	label = lv_label_create(blueTab, NULL);
	lv_label_set_text(label, "Second tab");

	label = lv_label_create(skillsTab, NULL);
	lv_label_set_text(label, "Third tab");
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
