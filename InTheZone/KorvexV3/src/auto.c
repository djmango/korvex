/** @file auto.c
 * @brief File for autonomous code
 */

#include "main.h"
#include "constants.h"
#include "korvexlib.h"

/*
 * Runs the user autonomous code. This function will be started in its own task
 * with the default
 * priority and stack size whenever the robot is enabled via the Field
 * Management System or the
 * VEX Competition Switch in the autonomous mode. If the robot is disabled or
 * communications is
 * lost, the autonomous task will be stopped by the kernel. Re-enabling the
 * robot will restart
 * the task, not re-start it from where it left off.
 *
 * Code running in the autonomous task cannot access information from the VEX
 * Joystick. However,
 * the autonomous function can be invoked from another task if a VEX Competition
 * Switch is not
 * available, and it can access joystick information if called in this way.
 *
 * The autonomous task may exit, unlike operatorControl() which should never
 * exit. If it does
 * so, the robot will adelay a switch to another mode or disable/enable cycle.
 */

/*-----------------------------------------------------------------------------*/
/*  Funky messages for the lcd */
/*-----------------------------------------------------------------------------*/
void lcdTextA()
{
  lcdSetBacklight(uart1, true);
  lcdSetText(uart1, 1, "it works");
  lcdSetText(uart1, 2, "i  swear");
}

void pidFeedA() {
  driveLeftValue = encoderGet(leftencoder);
  taskCreate(driveLeftPid, TASK_DEFAULT_STACK_SIZE, (5, 0, 1),
             TASK_PRIORITY_DEFAULT);
  driveRightValue = encoderGet(rightencoder);
  taskCreate(driveRightPid, TASK_DEFAULT_STACK_SIZE, (5, 0, 1),
             TASK_PRIORITY_DEFAULT);
  dr4bLeftValue = encoderGet(dr4bleftencoder);
  taskCreate(dr4bLeftPid, TASK_DEFAULT_STACK_SIZE, (5, 0, 1),
             TASK_PRIORITY_DEFAULT);
  dr4bRightValue = encoderGet(dr4brightencoder);
  taskCreate(dr4bRightPid, TASK_DEFAULT_STACK_SIZE, (5, 0, 1),
             TASK_PRIORITY_DEFAULT);
  chainValue = encoderGet(chainencoder);
  taskCreate(chainPid, TASK_DEFAULT_STACK_SIZE, (5, 0, 1),
             TASK_PRIORITY_DEFAULT);
}

/* motors:
  driveLeft
  driveRight
  mobileGoal
  dr4bLeft
  dr4bRight
  chainBar
  claw
*/

/* port map DESIRED
  port 1 = **scrubbed**
  port 2 = drive left side (y-cable to power expander)
  port 3 = drive right side (y-cable to power expander)
  port 4 = mobile goal lift (y-cable)
  port 5 = free
  port 6 = dr4b left (y-cable)
  port 7 = dr4b right (y-cable)
  port 8 = chain bar (y-cable)
  port 9 = claw (direct)
  port 10 = **scrubbed**
*/

void autonomous() {
  pidFeedA();
  lcdTextA();
  int auton = 0;
  switch (auton) {
  case 0 :
    break;
  default :
    break;
  }

}
