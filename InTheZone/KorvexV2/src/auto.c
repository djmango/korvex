/** @file auto.c
 * @brief File for autonomous code
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
 * so, the robot will adelay a switch to another mode or disable/enable cycle.
 */

// defs
// motor defs
#define driveLeft 2
#define driveRight 3
#define mobileGoal 4
#define dr4bLeft 6
#define dr4bRight 7
#define chainBar 8
#define claw 8

// pid defs
int PID_DRIVE_MAX = 127;
int PID_DRIVE_MIN = -127;
#define PID_INTEGRAL_LIMIT 50

// lcd defs
#define vexCompetitionState ((isEnabled() ? 0 : 2) + (isAutonomous() ? 1 : 0))

#define kButtonNone 0
#define kButtonLeft LCD_BTN_LEFT
#define kButtonCenter LCD_BTN_CENTER
#define kButtonRight LCD_BTN_RIGHT

// vars

// drive pid floats
float pidKpDrive = 6.5;
float pidKiDrive = 0;
float pidKdDrive = 1;
bool driveIsPidEnabledA = false;

// chian pid floats
float pidKpChian = 1;
float pidKiChain = 0;
float pidKdChain = 0;
bool chainIsPidEnabledA = false;

// dr4b pid floats
float pidKpDr4b = 1;
float pidKiDr4b = 0;
float pidKdDr4b = 0;
bool dr4bIsPidEnabledA = false;

// left drive values
int driveEncoderValueLeft;
int driveEncoderTargetLeft;
int motorDriveLeft;

// right drive values
int driveEncoderValueRight;
int driveEncoderTargetRight;
int motorDriveRight;

//chain values
int chainEncoderValue;
int chainEncoderTarget;

// right dr4b values
int potValueRightDr4b;
int potTargetRightDr4b;
int motorDr4bRight;

// left dr4b values
int potValueLeftDr4b;
int potTargetLeftDr4b;
int motorDr4bLeft;

// global hold the auton selection
static int MyAutonomous = 0;
int AutonLevel = 0; // level 0 = side, level 1 = strat, level 2 = done and done,
                    // level 3 = disabled

int getLcdButtons() { // thanks jpearman
  int competitionState = vexCompetitionState;
  int buttons;

  // This function will block until either
  // 1. A button is pressed on the LCD
  //    If a button is pressed when the function starts then that button
  //    must be released before a new button is detected.
  // 2. Robot competition state changes

  // delay for all buttons to be released
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
  if (AutonLevel == 0) {
    int button;
    lcdSetText(uart1, 1, "Select Side");
    lcdSetText(uart1, 2, "Blue - D - Red");
    // side selection
    while (AutonLevel == 0) {
      // this function blocks until button is pressed
      button = getLcdButtons();
      // Display and select the autonomous routine
      if (button == kButtonLeft) {
        AutonLevel = 1;
        LcdSetAutonomous(0);
        lcdSetText(uart1, 2, "left");
      }
      if (button == kButtonRight) {
        AutonLevel = 1;
        lcdSetText(uart1, 2, "right");
        LcdSetAutonomous(2);

      } else {
        AutonLevel = 0;
      }
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

  while (AutonLevel != 2) {
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
}

/*-----------------------------------------------------------------------------*/
/*  Reset all encoders */
/*-----------------------------------------------------------------------------*/
void encoderResetAll() {
  // reset all encoders
  encoderReset(leftencoder);
  encoderReset(rightencoder);
}

/*-----------------------------------------------------------------------------*/
/*  Calculate error and drive for left drive */
/*-----------------------------------------------------------------------------*/
void driveLeftPID() {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  float pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (driveIsPidEnabledA == true && isAutonomous() == true)
  {
    float encoderCalcValue = (encoderGet(leftencoder) * 31.9024 / 360);
    // convert to a universal unit, in this case centimeters because science
    // 31.9024 is avg circumfrence of omniwheel, 360 due to how the optical shaft encoder counts
    // encoder target becuase i kinda need that
    // derek IS the coder

    // mathy stuff

    // calculate error
    pidError = encoderCalcValue - driveEncoderTargetLeft;

    // integral - if Ki is not 0
    if (pidKiDrive != 0)
    {
      // If we are inside controlable window then integrate the error
      if (abs(pidError) < PID_INTEGRAL_LIMIT)
        pidIntegral = pidIntegral + pidError;
      else
        pidIntegral = 0;
    }
    else
      pidIntegral = 0;

    // calculate the derivative
    pidDerivative = pidError - pidLastError;
    pidLastError = pidError;

    // calculate drive
    pidDrive = (pidKpDrive * pidError) + (pidKiDrive * pidIntegral) + (pidKdDrive * pidDerivative);
    pidDrive = pidDrive * -1;
    // printf("left drive  %d\n", pidDrive);
    // limit drive
    if (pidDrive > PID_DRIVE_MAX)
      pidDrive = PID_DRIVE_MAX;
    if (pidDrive < PID_DRIVE_MIN)
      pidDrive = PID_DRIVE_MIN;
    // send to motor
    motorSet(driveLeft, pidDrive);
    delay(50);
    // Run at 50Hz
  }
}

/*-----------------------------------------------------------------------------*/
/*  Calculate error and drive for right drive */
/*-----------------------------------------------------------------------------*/
void driveRightPID() {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  float pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (driveIsPidEnabledA == true && isAutonomous() == true)
  {
    float encoderCalcValue = (encoderGet(rightencoder) * 31.9024 / 360);
    // convert to a universal unit, in this case centimeters because science
    // 31.9024 is avg circumfrence of omniwheel, 360 due to how the optical shaft encoder counts
    // encoder target becuase i kinda need that
    // derek IS the coder

    // mathy stuff

    // calculate error
    pidError = encoderCalcValue - driveEncoderTargetRight;

    // integral - if Ki is not 0
    if (pidKiDrive != 0)
    {
      // If we are inside controlable window then integrate the error
      if (abs(pidError) < PID_INTEGRAL_LIMIT)
        pidIntegral = pidIntegral + pidError;
      else
        pidIntegral = 0;
    }
    else
      pidIntegral = 0;

    // calculate the derivative
    pidDerivative = pidError - pidLastError;
    pidLastError = pidError;

    // calculate drive
    pidDrive = (pidKpDrive * pidError) + (pidKiDrive * pidIntegral) + (pidKdDrive * pidDerivative);
    pidDrive = pidDrive * -1;
    // printf("right drive %d\n", pidDrive);
    // limit drive
    if (pidDrive > PID_DRIVE_MAX)
      pidDrive = PID_DRIVE_MAX;
    if (pidDrive < PID_DRIVE_MIN)
      pidDrive = PID_DRIVE_MIN;
    // send to motor
    motorSet(driveRight, pidDrive);
    delay(50);
    // Run at 50Hz
  }
}

/*-----------------------------------------------------------------------------*/
/*  Calculate error and drive for chainbar */
/*-----------------------------------------------------------------------------*/
void chainPID() {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  float pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (chainIsPidEnabledA == true && isAutonomous() == true)
  {
    float encoderCalcValue = (encoderGet(chainencoder));
    // shaft encoder counts, direct, no need for conversion
    // encoder target becuase i kinda need that
    // derek IS the coder

    // mathy stuff

    // calculate error
    pidError = encoderCalcValue - chainEncoderTarget;

    // integral - if Ki is not 0
    if (pidKiDrive != 0)
    {
      // If we are inside controlable window then integrate the error
      if (abs(pidError) < PID_INTEGRAL_LIMIT)
        pidIntegral = pidIntegral + pidError;
      else
        pidIntegral = 0;
    }
    else
      pidIntegral = 0;

    // calculate the derivative
    pidDerivative = pidError - pidLastError;
    pidLastError = pidError;

    // calculate drive
    pidDrive = (pidKpDrive * pidError) + (pidKiDrive * pidIntegral) + (pidKdDrive * pidDerivative);
    pidDrive = pidDrive * -1;
    // limit drive
    if (pidDrive > PID_DRIVE_MAX)
      pidDrive = PID_DRIVE_MAX;
    if (pidDrive < PID_DRIVE_MIN)
      pidDrive = PID_DRIVE_MIN;
    // send to motor
    motorSet(chainBar, pidDrive);
    delay(50);
    // Run at 50Hz
  }
}
/*-----------------------------------------------------------------------------*/
/*  Calculate error and drive for right dr4b */
/*-----------------------------------------------------------------------------*/
void dr4bRightPID() {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  float pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (dr4bIsPidEnabledA == true) {
    float potCalcValue = (potValueRightDr4b);
    // convert to a universal unit, in this case centimeters because science
    // 31.9024 is avg circumfrence of omniwheel, 360 due to how the optical
    // shaft encoder counts
    // sensor scale becuase why not
    // encoder target becuase i kinda need that
    // glenn IS the code
    // derek IS the coder

    // mathy stuff

    // calculate error
    pidError = potCalcValue - potTargetRightDr4b;

    // integral - if Ki is not 0
    if (pidKiDr4b != 0) {
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
    pidDrive = (pidKpDr4b * pidError) + (pidKiDr4b * pidIntegral) +
               (pidKdDrive * pidDerivative);

    // limit drive
    if (pidDrive > PID_DRIVE_MAX)
      pidDrive = PID_DRIVE_MAX;
    if (pidDrive < PID_DRIVE_MIN)
      pidDrive = PID_DRIVE_MIN;

    // send to motor
    motorSet(motorDr4bRight, pidDrive);

    // Run at 50Hz
  }
}

/*-----------------------------------------------------------------------------*/
/*  Calculate error and drive for left dr4b */
/*-----------------------------------------------------------------------------*/
void dr4bLeftPID() {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  float pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (dr4bIsPidEnabledA == true) {
    float potCalcValue = (potValueLeftDr4b);
    // convert to a universal unit, in this case centimeters because science
    // 31.9024 is avg circumfrence of omniwheel, 360 due to how the optical
    // shaft encoder counts
    // sensor scale becuase why not
    // encoder target becuase i kinda need that
    // glenn IS the code
    // derek IS the coder

    // mathy stuff

    // calculate error
    pidError = potCalcValue - potTargetLeftDr4b;

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
    motorSet(motorDr4bLeft, pidDrive);

    // Run at 50Hz
  }
}

/*-----------------------------------------------------------------------------*/
/*  Funky messages for the lcd */
/*-----------------------------------------------------------------------------*/
void lcdTextA()
{
  lcdSetBacklight(uart1, true);
  lcdSetText(uart1, 1, "it works");
  lcdSetText(uart1, 2, "i  swear");
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
  lcdTextA();
  encoderResetAll();
  int auton = 0;
  driveIsPidEnabledA = true;
  TaskHandle driveTaskHandleLeft = taskRunLoop(driveLeftPID, 25);
  TaskHandle driveTaskHandleRight = taskRunLoop(driveRightPID, 25);
  // auton0 is red left, auton1 is run awayyyyy
  switch (auton) {
  case 0 :
    //begin foldout
    encoderResetAll();
    motorSet(claw, 50);
    PID_DRIVE_MAX = 80;
    PID_DRIVE_MIN = 80;
    motorSet(dr4bLeft, -127);
    motorSet(dr4bRight, -127);
    delay(200);
    //drive through cones to mobile goal
    driveEncoderTargetLeft = 90;
    driveEncoderTargetRight = 90;
    motorSet(mobileGoal, -127);
    delay(800);
    //drop mobile goal intake
    motorSet(mobileGoal, 0);
    motorSet(dr4bLeft, 0);
    motorSet(dr4bRight, 0);
    delay(500);
    //drive into mobile goal
    encoderResetAll();
    PID_DRIVE_MAX = 127;
    PID_DRIVE_MIN = 127;
    driveEncoderTargetLeft = 50;
    driveEncoderTargetRight = 50;
    delay(2000);
    //pick up mobile goal
    motorSet(mobileGoal, 127);
    delay(600);
    //turn 180
    encoderResetAll();
    motorSet(mobileGoal, 0);
    driveEncoderTargetLeft = 50;
    driveEncoderTargetRight = -50;
    delay(2000);
    //move back to the bar
    encoderResetAll();
    driveEncoderTargetLeft = 140;
    driveEncoderTargetRight = 140;
    delay(2000);
    //turn 45 degrees
    encoderResetAll();
    driveEncoderTargetLeft = -10;
    driveEncoderTargetRight = 10;
    delay(1000);
    //forward to line up with middle of zone
    encoderResetAll();
    driveEncoderTargetLeft = 30;
    driveEncoderTargetRight = 30;
    delay(1000);
    //turn 175 to angle for drop
    encoderResetAll();
    driveEncoderTargetLeft = -30;
    driveEncoderTargetRight = 30;
    delay(1000);
    //drive into zone
    encoderResetAll();
    driveEncoderTargetLeft = 30;
    driveEncoderTargetRight = 30;
    delay(1000);
    //drop mobile goal
    motorSet(mobileGoal, -127);
    delay(600);
    motorSet(mobileGoal, 0);
    delay(600);
    //drive out
    encoderResetAll();
    driveEncoderTargetLeft = -70;
    driveEncoderTargetRight = -70;
    delay(700);
    driveIsPidEnabledA = false;
    taskDelete(driveTaskHandleLeft);
    taskDelete(driveTaskHandleRight);
    break;
  case 1 :
    //drive through cones to mobile goal
    driveEncoderTargetLeft = 900;
    driveEncoderTargetRight = 900;
    delay(4000);
    driveIsPidEnabledA = false;
    taskDelete(driveTaskHandleLeft);
    taskDelete(driveTaskHandleRight);
    motorStopAll();
  default:
    break;
  }
}
