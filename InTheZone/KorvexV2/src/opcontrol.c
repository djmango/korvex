#include "main.h"
// all variables
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

// vars

// drive pid floats
float pidKpDriveO = 1;
float pidKiDriveO = 0;
float pidKdDriveO = 0;
bool driveIsPidEnabledO = false;

// dr4b pid floats
float pidKpDr4bO = 1;
float pidKiDr4bO = 0;
float pidKdDr4bO = 0;
bool dr4bIsPidEnabled = false;

// left drive values
int driveEncoderValueLeft;
int driveEncoderTargetLeft;

// right drive values
int drivedriveEncoderValueRight;
int driveEncoderTargetRight;

// right dr4b values
int dr4bRightPotValue;
int dr4bRightPotTarget;

// left dr4b values
int dr4bLeftPotValue;
int dr4bLeftPotTarget;

bool isFineControl = false;
int fineControl = 1;

bool dr4bIsDebugEnabled = false; //if enabled print logs to console
// functions

/*-----------------------------------------------------------------------------*/
/*  Reset all encoders */
/*-----------------------------------------------------------------------------*/
void encoderResetAllO() {
  // reset all encoders
  encoderReset(leftencoder);
  encoderReset(rightencoder);
}
/*-----------------------------------------------------------------------------*/
/*  Calculate error and drive for left drive */
/*-----------------------------------------------------------------------------*/
void driveLeftPIDO() {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  float pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (driveIsPidEnabledO == true) {
    float encoderCalcValue = (driveEncoderValueLeft * 31.9024 / 360);
    // convert to a universal unit, in this case centimeters because science
    // 31.9024 is avg circumfrence of omniwheel, 360 due to how the optical
    // shaft encoder counts
    // sensor scale becuase why not
    // encoder target becuase i kinda need that
    // glenn IS the code
    // derek IS the coder

    // mathy stuff

    // calculate error
    pidError = encoderCalcValue - driveEncoderTargetLeft;

    // integral - if Ki is not 0
    if (pidKiDriveO != 0) {
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
    pidDrive = (pidKpDriveO * pidError) + (pidKiDriveO * pidIntegral) +
               (pidKdDriveO * pidDerivative);

    // limit drive
    if (pidDrive > PID_DRIVE_MAX)
      pidDrive = PID_DRIVE_MAX;
    if (pidDrive < PID_DRIVE_MIN)
      pidDrive = PID_DRIVE_MIN;

    // send to motor
    motorSet(dr4bLeft, pidDrive);
    delay(25);
    // Run at 50Hz
  }
}
/*-----------------------------------------------------------------------------*/
/*  Calculate error and drive for right drive */
/*-----------------------------------------------------------------------------*/
void driveRightPIDO() {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  float pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (driveIsPidEnabledO == true) {
    float encoderCalcValue = (drivedriveEncoderValueRight * 31.9024 / 360);
    // convert to a universal unit, in this case centimeters because science
    // 31.9024 is avg circumfrence of omniwheel, 360 due to how the optical
    // shaft encoder counts
    // sensor scale becuase why not
    // encoder target becuase i kinda need that
    // glenn IS the code
    // derek IS the coder

    // mathy stuff

    // calculate error
    pidError = encoderCalcValue - driveEncoderTargetRight;

    // integral - if Ki is not 0
    if (pidKiDriveO != 0) {
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
    pidDrive = (pidKpDriveO * pidError) + (pidKiDriveO * pidIntegral) +
               (pidKdDriveO * pidDerivative);
    // limit drive
    if (pidDrive > PID_DRIVE_MAX)
      pidDrive = PID_DRIVE_MAX;
    if (pidDrive < PID_DRIVE_MIN)
      pidDrive = PID_DRIVE_MIN;
    // send to motor
    motorSet(driveRight, pidDrive);
    delay(25);
    // Run at 50Hz
  }
}
/*-----------------------------------------------------------------------------*/
/*  Calculate error and drive for right dr4b */
/*-----------------------------------------------------------------------------*/
void dr4bRightPIDO() {
  int pidError;
  int pidLastError;
  int pidIntegral;
  int pidDerivative;
  int pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (dr4bIsPidEnabled == true) {
    int potCalcValue = dr4bRightPotValue;
    // convert to a universal unit, in this case centimeters because science
    // 31.9024 is avg circumfrence of omniwheel, 360 due to how the optical
    // shaft encoder counts
    // sensor scale becuase why not
    // encoder target becuase i kinda need that
    // glenn IS the code
    // derek IS the coder

    // mathy stuff
    //debug
    if (dr4bIsDebugEnabled) {
    printf("pid error %d\n", pidError);
    printf("pot calc value %d\n", analogRead(4));
    printf("pot target %d\n", dr4bRightPotTarget);
    }
    // calculate error
    pidError = potCalcValue - dr4bRightPotTarget;

    // integral - if Ki is not 0
    if (pidKiDr4bO != 0) {
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
    pidDrive = (pidKpDr4bO * pidError) + (pidKiDr4bO * pidIntegral) +
               (pidKdDr4bO * pidDerivative);

    // limit drive
    if (pidDrive > PID_DRIVE_MAX)
      pidDrive = PID_DRIVE_MAX;
    if (pidDrive < PID_DRIVE_MIN)
      pidDrive = PID_DRIVE_MIN;

    pidDrive = pidDrive * -1;
    // send to motor
    if (dr4bIsDebugEnabled) printf("right pot drive  %d\n", pidDrive);
    // motorSet(motorDr4bRight, pidDrive);
    motorSet(dr4bRight, pidDrive);
    delay(25);
    // Run at 50Hz
  }
}
/*-----------------------------------------------------------------------------*/
/*  Calculate error and drive for left dr4b */
/*-----------------------------------------------------------------------------*/
void dr4bLeftPIDO() {
  int pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  int pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (dr4bIsPidEnabled == true) {
    int potCalcValue = (dr4bLeftPotValue);
    // convert to a universal unit, in this case centimeters because science
    // 31.9024 is avg circumfrence of omniwheel, 360 due to how the optical
    // shaft encoder counts
    // sensor scale becuase why not
    // encoder target becuase i kinda need that
    // glenn IS the code
    // derek IS the coder

    // mathy stuff

    // calculate error
    pidError = potCalcValue - dr4bLeftPotTarget;
    // integral - if Ki is not 0
    if (pidKiDr4bO != 0) {
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
    pidDrive = (pidKpDr4bO * pidError) + (pidKiDr4bO * pidIntegral) +
               (pidKdDr4bO * pidDerivative);

    // limit drive
    if (pidDrive > PID_DRIVE_MAX)
      pidDrive = PID_DRIVE_MAX;
    if (pidDrive < PID_DRIVE_MIN)
      pidDrive = PID_DRIVE_MIN;

    pidDrive = pidDrive * 1;
    // send to motor
    // printf("left read %d\n", pidDrive);
    // motorSet(motorDr4bLeft, pidDrive);
    motorSet(dr4bLeft, pidDrive);
    delay(25);
    // Run at 50Hz
  }
}

void updateDrive(int chassisControlLeft, int chassisControlRight,
                 int liftControl) {
  //chassis control
  motorSet(driveRight, (chassisControlRight * fineControl));
  motorSet(driveLeft, (chassisControlLeft * fineControl));
  //lift control
  motorSet(dr4bLeft, (liftControl));
  motorSet(dr4bRight, (liftControl));
}
void fineControlToggle(int fineBtn) {
  // fine control toggle
  if (fineBtn == 1 && isFineControl == false) { // toggle it on
    isFineControl = true;
    fineControl = .5;
  }
  if (fineBtn == 1 && isFineControl == true) { // toggle it off
    isFineControl = false;
    fineControl = 1;
  }
}
void mobileGoalControl(int moboLiftBtnUp, int moboLiftBtnDown,
                       int moboTiltBtnUp, int moboTiltBtnDown) {
  // mobo lift control
  if (moboLiftBtnUp == 1) {
    motorSet(4, 127);
  }
  if (moboLiftBtnDown == 1) {
    motorSet(4, -127);
  }
  if (moboLiftBtnUp == 0 && moboLiftBtnDown == 0) {
    motorSet(4, 0);
  }
  // mobo tilt control
  if (moboTiltBtnUp == 1) {
    motorSet(5, 127);
  }
  if (moboTiltBtnDown == 1) {
    motorSet(5, -127);
  }
  if (moboTiltBtnUp == 0 && moboTiltBtnDown == 0) {
    motorSet(5, 0);
  }
}
void coneHandlerControl(int clawBtnUp, int clawBtnDown, int chainbarBtnUp,
                        int chainbarBtnDown) {
  motorSet(chainBar, (joystickGetAnalog(2, 3)));
  /* chain bar control
     if (chainbarBtnUp == 1) {
     // move up
     motorSet(8, 127);
     }
     if (chainbarBtnDown == 1) {
     // move down
     motorSet(8, -127);
     }
     if (chainbarBtnUp == 0 &&
      chainbarBtnDown == 0) {
     // dont move
     motorSet(8, 0);
     }*/
  // claw control
  if (clawBtnUp == 1) {
    // move up
    motorSet(9, 40);
  }
  if (clawBtnDown == 1) {
    // move down
    motorSet(9, -40);
  }
  if (clawBtnUp == 0 && clawBtnDown == 0) {
    // dont move
    motorSet(9, 0);
  }
}

void lcdText() {
  lcdSetBacklight(uart1, true);
  lcdSetText(uart1, 1, "i want to");
  lcdSetText(uart1, 2, "sudo rm -rf myself");
}

/*port map DESIRED
   port 1 = **scrubbed**
   port 2 = drive left side (y-cable to power expander)
   port 3 = drive right side (y-cable to power expander)
   port 4 = mobile goal lift (y-cable)
   port 5 = free
   port 6 = dr4b left (y-cable)
   port 7 = dr4b right (y-cable)
   port 8 = chain bar (direct)
   port 9 = claw (direct)
   port 10 = **scrubbed**
 */

/*control map DESIRED
   drive left side = mainLeftJoy(1,3)
   drive right side = mainRightJoy(1,2)
   mobile goal lift = seccondaryRightBumper(2,6)
   dr4b = seccondaryRightJoy(2,3)
   chain bar = seccondaryLeftJoy(2,2)
   claw = seccondaryLeftBumper(2,5)
 */

/*motor port names:
   driveLeft
   driveRight
   mobileGoal
   dr4bLeft
   dr4bRight
   chainBar
   claw
 */

/*control arguement names
   chassisControl chassisControlLeft, chassisControlRight, liftControl
   fineControlToggle fineBtn
   mobileGoalControl moboLiftBtn, moboTiltBtn
   coneHandlerControl clawBtn, chainbarBtn
 */

/* TODO VERY IMPORTANT
alright im writing this now because im going to forget later
so ive pinpointed the pid issue to a task run issue, because the motor is
not getting the correct power value. possible solutions are to increase time
before
reset or initiate with rest of the loop, normaly function.
 */
void operatorControl() {
  // driveIsPidEnabledO = true;
  // int driveEncoderValueLeft = encoderGet(leftencoder);
  // int driveEncoderValueRight = encoderGet(rightencoder);
  lcdText();
  // dr4b stuff, same format
  // dr4bIsPidEnabled = true; // enable dr4b pid
  // dr4bLeftPotValue = analogRead(3);
  // dr4bRightPotValue = analogRead(4); // potentiometer reads, respective to side
  // TaskHandle dr4bTaskHandleLeft = taskRunLoop(dr4bLeftPIDO, 50);
  // TaskHandle dr4bTaskHandleRight = taskRunLoop(dr4bRightPIDO, 50);
  // TaskHandle driveTaskHandleLeft = taskRunLoop(driveLeftPIDO, 50);
  // TaskHandle driveTaskHandleRight = taskRunLoop(driveRightPIDO, 50);

  // dr4bLeftPotTarget = 4000;
  // dr4bRightPotTarget = 4000;
  // dr4bIsDebugEnabled = true;
  while (isEnabled()) {
    /*
      if (joystickGetAnalog(2, 2) > 25 ||
          joystickGetAnalog(2, 2) <
              25) { // if joystick is out of deadzone, change target
        dr4bRightPotTarget = dr4bRightPotTarget + (joystickGetAnalog(2, 2) / 20);
      }
      if (joystickGetAnalog(2, 2) > 25 || joystickGetAnalog(2, 2) < 25) {
        dr4bLeftPotTarget = dr4bLeftPotTarget + (joystickGetAnalog(2, 2) / 20);
      }
      if (dr4bRightPotTarget > (analogRead(4) + PID_DRIVE_MAX))
        dr4bRightPotTarget = (analogRead(4) + PID_DRIVE_MAX);
      if (dr4bLeftPotTarget < (analogRead(3) - PID_DRIVE_MIN))
        dr4bLeftPotTarget = (analogRead(3) - PID_DRIVE_MIN);
      //drive stuff

      if (joystickGetAnalog(1, 3) > 25 || joystickGetAnalog(1, 3) < 25)
      { // if joystick is out of deadzone, change target
        driveEncoderTargetLeft = driveEncoderTargetLeft + (joystickGetAnalog(1, 3) / 20);
      }
      else
      {
        encoderResetAllO();
        driveEncoderTargetLeft = 0;
      }

      if (joystickGetAnalog(1, 2) > 25 || joystickGetAnalog(1, 2) < 25)
      {
        driveEncoderTargetRight = driveEncoderTargetRight + (joystickGetAnalog(1, 2) / 20);
      }
      else
      {
        encoderResetAllO();
            driveEncoderTargetRight = 0;
      }
    */
    updateDrive(joystickGetAnalog(1, 3), joystickGetAnalog(1, 2),
                joystickGetAnalog(2, 2));
    fineControlToggle(joystickGetDigital(1, 7, JOY_DOWN));
    mobileGoalControl(
        joystickGetDigital(1, 6, JOY_UP), joystickGetDigital(1, 6, JOY_DOWN),
        joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN));
    coneHandlerControl(
        joystickGetDigital(2, 5, JOY_UP), joystickGetDigital(2, 5, JOY_DOWN),
        joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_UP));
    delay(20);
  }
  // taskDelete(dr4bTaskHandleLeft);
  // taskDelete(dr4bTaskHandleRight);
  // taskDelete(driveTaskHandleLeft);
  // taskDelete(driveTaskHandleRight);
}
