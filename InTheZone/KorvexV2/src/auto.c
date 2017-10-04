/** @file auto.c
 * @brief File for autonomous code
 *
 * This file should contain the user autonomous() function and any functions
 * related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

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
 * so, the robot will await a switch to another mode or disable/enable cycle.
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

// defs
// motor defs
#define driveLeft 2
#define driveRight 3
#define mobileGoal 4
#define dr4bLeft 6
#define dr4bRight 7
#define chainBar 8
#define claw 9

// pid defs
#define PID_DRIVE_MAX 127
#define PID_DRIVE_MIN (-127)
#define PID_INTEGRAL_LIMIT 50

// lcd defs
#define vexCompetitionState ((isEnabled() ? 0 : 2) + (isAutonomous() ? 1 : 0))

#define kButtonNone 0
#define kButtonLeft LCD_BTN_LEFT
#define kButtonCenter LCD_BTN_CENTER
#define kButtonRight LCD_BTN_RIGHT

// vars

// drive pid floats
float pidKpDrive = 2.0;
float pidKiDrive = 0.04;
float pidKdDrive = 0.05;
bool encoderPIDenabledDrive = false;

// left drive values
int encoderValueLeft;
int encoderTargetLeft;
int motorLeft;

// right drive values
int encoderValueRight;
int encoderTargetRight;
int motorRight;

// global hold the auton selection
static int MyAutonomous = 0;
int AutonLevel = 0; // level 0 = side, level 1 = strat

int getLcdButtons() { // thanks jpearman
  int competitionState = vexCompetitionState;
  int buttons;

  // This function will block until either
  // 1. A button is pressed on the LCD
  //    If a button is pressed when the function starts then that button
  //    must be released before a new button is detected.
  // 2. Robot competition state changes

  // Wait for all buttons to be released
  while (lcdReadButtons(uart1) != kButtonNone) {
    // check competition state, bail if it changes
    if (vexCompetitionState != competitionState)
      return (kButtonNone);
    taskDelay(10);
  }

  // block until an LCD button is pressed
  do {
    // we use a copy of the lcd buttons to avoid their state changing
    // between the test and returning the status
    buttons = lcdReadButtons(uart1);

    // check competition state, bail if it changes
    if (vexCompetitionState != competitionState)
      return (kButtonNone);

    taskDelay(10);
  } while (buttons == kButtonNone);

  return (buttons);
}

/*-----------------------------------------------------------------------------*/
/*  Display autonomous selection */
/*-----------------------------------------------------------------------------*/
void LcdSetAutonomous(int value) {
  // Simple selection display
  // TODO: add side selector, then interior strat selector, use 0-5 for
  // MyAutonomous
  if (AutonLevel == 0) {
    int button;
    lcdSetText(uart1, 1, "Select Side");
    lcdSetText(uart1, 2, "Blue - Disable - Red");
    // side selection
    while (AutonLevel == 0) {
      // this function blocks until button is pressed
      button = getLcdButtons();
      // Display and select the autonomous routine
      if (button == kButtonLeft)
        LcdSetAutonomous(0);
      AutonLevel = 1;

      if (button == kButtonRight)
        LcdSetAutonomous(2);
      AutonLevel = 1;

      // Don't hog the cpu !
      taskDelay(10);
    }
  }
  if (AutonLevel == 1) {
    int button;
    if (value == 0) {
      // if its blue
      while (AutonLevel == 1) {
        // this function blocks until button is pressed
        button = getLcdButtons();
        // Display and select the autonomous routine
        if (button == kButtonLeft) {
          lcdSetText(uart1, 1, "Blue Left");
          lcdSetText(uart1, 2, "Running...");
          LcdSetAutonomous(0);
          AutonLevel = 2;
        }

        if (button == kButtonRight) {
          lcdSetText(uart1, 1, "Blue Right");
          lcdSetText(uart1, 2, "Running...");
          LcdSetAutonomous(1);
          AutonLevel = 2;
        }
        if (button == kButtonRight) {
          lcdSetText(uart1, 1, "Blue Something");
          lcdSetText(uart1, 2, "Running...");
          LcdSetAutonomous(2);
          AutonLevel = 2;
        }
        // Don't hog the cpu !
        taskDelay(10);
      }
    }
    if (value == 2) {
      // if its red
      while (AutonLevel == 1) {
        // this function blocks until button is pressed
        button = getLcdButtons();
        // Display and select the autonomous routine
        if (button == kButtonLeft) {
          lcdSetText(uart1, 1, "Red Left");
          lcdSetText(uart1, 2, "Running...");
          LcdSetAutonomous(3);
          AutonLevel = 2;
        }
        if (button == kButtonRight) {
          lcdSetText(uart1, 1, "Red Right");
          lcdSetText(uart1, 2, "Running...");
          LcdSetAutonomous(4);
          AutonLevel = 2;
        }
        if (button == kButtonRight) {
          lcdSetText(uart1, 1, "Red Something");
          lcdSetText(uart1, 2, "Running...");
          LcdSetAutonomous(5);
          AutonLevel = 2;
        }
        // Don't hog the cpu !
        taskDelay(10);
      }
    }
    if (value == 1) {
      // if its disabled
      lcdSetText(uart1, 1, "DISABLED!");
      lcdSetText(uart1, 2, "what a surprise");
      AutonLevel == 3;
    }
    // value 0 = strat1, value 1 = strat2, etc. 0-2 is blue, 3-5 is red
  }
  // Save autonomous mode for later
  if (AutonLevel == 2) { // if auton is final
    MyAutonomous = value;
  }
}

/*-----------------------------------------------------------------------------*/
/*  Select one of three autonomous choices */
/*-----------------------------------------------------------------------------*/

void LcdAutonomousSelection() {
  int button;

  // Clear LCD and turn on backlight
  lcdClear(uart1);
  lcdSetBacklight(uart1, true);

  // diaplay default choice
  LcdSetAutonomous(0);
  // PROS seems to need a delay
  taskDelay(2000);

  while (!isEnabled()) {
    // this function blocks until button is pressed
    button = getLcdButtons();

    // Display and select the autonomous routine
    if (button == kButtonLeft)
      LcdSetAutonomous(0);

    if (button == kButtonCenter)
      LcdSetAutonomous(1);

    if (button == kButtonRight)
      LcdSetAutonomous(2);

    // Don't hog the cpu !
    taskDelay(10);
  }

  lcdSetText(uart1, 2, "Running....     ");
}

void encoderResetAll() {
  // reset all encoders
  encoderReset(leftencoder);
  encoderReset(rightencoder);
}
void driveLeftPID() {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  float pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (encoderPIDenabledDrive == true) {
    float encoderCalcValue = (encoderValueLeft * 31.9024 / 360);
    // convert to a universal unit, in this case centimeters because science
    // 31.9024 is avg circumfrence of omniwheel, 360 due to how the optical
    // shaft encoder counts
    // sensor scale becuase why not
    // encoder target becuase i kinda need that
    // glenn IS the code
    // derek IS the coder

    // mathy stuff

    // calculate error
    pidError = encoderCalcValue - encoderTargetLeft;

    // integral - if Ki is not 0
    if (pidKiDrive != 0) {
      // If we are inside controlable window then integrate the error
      if (abs(pidError) < PID_INTEGRAL_LIMIT)
        pidIntegral = pidIntegral + pidError;
      else
        pidIntegral = 0;
    } else
      pidIntegral = 0;

    // calculate the derivative
    pidDerivative = pidError - pidLastError;
    pidLastError = pidError;

    // calculate drive
    pidDrive = (pidKpDrive * pidError) + (pidKiDrive * pidIntegral) +
               (pidKdDrive * pidDerivative);

    // limit drive
    if (pidDrive > PID_DRIVE_MAX)
      pidDrive = PID_DRIVE_MAX;
    if (pidDrive < PID_DRIVE_MIN)
      pidDrive = PID_DRIVE_MIN;

    // send to motor
    motorSet(motorLeft, pidDrive);

    // Run at 50Hz
  }
}
void driveRightPID() {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  float pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (encoderPIDenabledDrive == true) {
    float encoderCalcValue = (encoderValueRight * 31.9024 / 360);
    // convert to a universal unit, in this case centimeters because science
    // 31.9024 is avg circumfrence of omniwheel, 360 due to how the optical
    // shaft encoder counts
    // sensor scale becuase why not
    // encoder target becuase i kinda need that
    // glenn IS the code
    // derek IS the coder

    // mathy stuff

    // calculate error
    pidError = encoderCalcValue - encoderTargetRight;

    // integral - if Ki is not 0
    if (pidKiDrive != 0) {
      // If we are inside controlable window then integrate the error
      if (abs(pidError) < PID_INTEGRAL_LIMIT)
        pidIntegral = pidIntegral + pidError;
      else
        pidIntegral = 0;
    } else
      pidIntegral = 0;

    // calculate the derivative
    pidDerivative = pidError - pidLastError;
    pidLastError = pidError;

    // calculate drive
    pidDrive = (pidKpDrive * pidError) + (pidKiDrive * pidIntegral) +
               (pidKdDrive * pidDerivative);

    // limit drive
    if (pidDrive > PID_DRIVE_MAX)
      pidDrive = PID_DRIVE_MAX;
    if (pidDrive < PID_DRIVE_MIN)
      pidDrive = PID_DRIVE_MIN;

    // send to motor
    motorSet(motorRight, pidDrive);

    // Run at 50Hz
  }
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
void autonomous() {
  encoderTargetLeft = 0;
  encoderTargetRight = 0;
  TaskHandle driveTaskHandleLeft = taskRunLoop(driveLeftPID, 25);
  TaskHandle driveTaskHandleRight = taskRunLoop(driveRightPID, 25);
  LcdAutonomousSelection();
  switch (MyAutonomous) {
  case 0: // blue left
    // init all the important stuff, it shouldnt change throughout the code
    encoderPIDenabledDrive = true;
    // when you do not want the drive powered encoderPIDenabledDrive must be
    // false
    encoderValueLeft = encoderGet(leftencoder);
    // this gives the read value of the optical shaft encoder
    motorLeft = driveLeft;
    // the motor the pid is calculating for
    encoderValueRight = encoderGet(rightencoder);
    motorRight = driveRight;
    break;
  case 1: // blue right
    break;
  case 2: // blue something
    break;
  case 3: // red left
    break;
  case 4: // red right
    break;
  case 5: // red something
    break;
  }
  // testing the target, it stays at target until reset
  encoderTargetLeft = 12; // target, in cm
  encoderTargetRight = 12;
  // this will stay at 12 cm, until a reset
  encoderResetAll();
  encoderTargetLeft = -6;
  encoderTargetRight = 6;
  // should turn counter-clockwise? not sure, gota test
  taskDelete(driveTaskHandleLeft);
  taskDelete(driveTaskHandleRight);
}
