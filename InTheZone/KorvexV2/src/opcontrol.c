#include "main.h"
// all variables
// defs
// motor defs
#define driveLeft 2
#define driveRight 3
#define mobileGoal 4
#define chainBar 5
#define dr4bRight 7
#define dr4bLeft 8
#define claw 9

// pid defs
#define PID_DRIVE_MAX 127
#define PID_DRIVE_MIN (-127)
#define PID_INTEGRAL_LIMIT 50

// vars

// drive pid floats
float pidKpDriveO = .1;
float pidKiDriveO = 0;
float pidKdDriveO = 0;
bool driveIsPidEnabledO = false;

// chain pid floats
float pidKpChianO = 1;
float pidKiChainO = 0;
float pidKdChainO = 0;
bool chainIsPidEnabled = false;

// dr4b pid floats
float pidKpDr4bO = 1;
float pidKiDr4bO = 0;
float pidKdDr4bO = 0;
bool dr4bIsPidEnabled = false;

// left drive values
int driveEncoderValueLeft;
int driveEncoderTargetLeft;

// right drive values
int driveEncoderValueRight;
int driveEncoderTargetRight;

//chain values
int chainEncoderValue;
int chainEncoderTarget;

// right dr4b values
int dr4bRightPotValue;
int dr4bRightPotTarget;

// left dr4b values
int dr4bLeftPotValue;
int dr4bLeftPotTarget;

//control modifiers
bool isReverse = false;
bool isFineControl = false;
float fineControl = 1;

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
  while (driveIsPidEnabledO == true && isAutonomous() == false)  {
    float encoderCalcValue = (encoderGet(leftencoder) * 31.9024 / 360);
    // convert to a universal unit, in this case centimeters because science
    // 31.9024 is avg circumfrence of omniwheel, 360 due to how the optical
    // shaft encoder counts
    // encoder target becuase i kinda need that
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
    pidDrive = pidDrive * -1;
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
void driveRightPIDO() {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  float pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (driveIsPidEnabledO == true && isAutonomous() == false) {
    float encoderCalcValue = (encoderGet(rightencoder) * 31.9024 / 360);
    // convert to a universal unit, in this case centimeters because science
    // 31.9024 is avg circumfrence of omniwheel, 360 due to how the optical
    // shaft encoder counts
    // encoder target becuase i kinda need that
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
    pidDrive = pidDrive * -1;
    // printf("right drive  %d\n", pidDrive);
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
void chainPIDO() {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  float pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (chainIsPidEnabled == true && isAutonomous() == false)
  {
    float encoderCalcValue = (encoderGet(chainencoder));
    // shaft encoder counts, direct, no need for conversion
    // encoder target becuase i kinda need that
    // derek IS the coder

    // mathy stuff

    // calculate error
    pidError = encoderCalcValue - chainEncoderTarget;

    // integral - if Ki is not 0
    if (pidKiDriveO != 0)
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
    pidDrive = (pidKpDriveO * pidError) + (pidKiDriveO * pidIntegral) + (pidKdDriveO * pidDerivative);
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

/*-----------------------------------------------------------------------------*/
/*  Drive and lift control */
/*-----------------------------------------------------------------------------*/
void updateDrive(int chassisControlLeft, int chassisControlRight, int liftControl) {
  //chassis control
  if (isReverse == true) { //if in reverse, invert and switch sides for normal turning
    motorSet(driveLeft, (chassisControlRight * -1));
    motorSet(driveRight, (chassisControlLeft * -1));
  }
  else if (isReverse == false) { //if in normal operator, do not invert
    motorSet(driveRight, (chassisControlRight));
    motorSet(driveLeft, (chassisControlLeft));
  }
  //lift control
  motorSet(dr4bLeft, (liftControl * -1));
  motorSet(dr4bRight, (liftControl));
}

/*-----------------------------------------------------------------------------*/
/*  Toggle fine control for drive */
/*-----------------------------------------------------------------------------*/
void fineControlToggle(int fineBtn, int fineBtn2, int reverseBtn, int reverseBtn2) {
  // fine control toggle
  if (fineBtn == 1) { // toggle it on
    isFineControl = true;
    fineControl = .5;
  }
  if (fineBtn2 == 1) { // toggle it off
    isFineControl = false;
    fineControl = 1;
  }
  //reverse toggle
    if (reverseBtn == 1) { // toggle it on
    isReverse = true;
    fineControl = -1;
  }
  if (reverseBtn2 == 1) { // toggle it off
    isReverse = false;
    fineControl = 1;
  }
}

/*-----------------------------------------------------------------------------*/
/*  Mobile goal control */
/*-----------------------------------------------------------------------------*/
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

/*-----------------------------------------------------------------------------*/
/*  Cone handler control */
/*-----------------------------------------------------------------------------*/
void coneHandlerControl(int clawBtnUp, int clawBtnDown, int chainbarBtnUp,
                        int chainbarBtnDown) {
  motorSet(chainBar, (joystickGetAnalog(2, 3)));
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

/*-----------------------------------------------------------------------------*/
/*  Funky messages for the lcd */
/*-----------------------------------------------------------------------------*/
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
   fineControlToggle fineBtn, reverseBtn
   mobileGoalControl moboLiftBtn, moboTiltBtn
   coneHandlerControl clawBtn, chainbarBtn
 */
void operatorControl() {
  lcdText();
  while (isEnabled()) {
    //argument based control scheme
    updateDrive(joystickGetAnalog(1, 3), joystickGetAnalog(1, 2), joystickGetAnalog(2, 2));
    fineControlToggle(joystickGetDigital(1, 7, JOY_DOWN), joystickGetDigital(1, 7, JOY_UP), joystickGetDigital(1, 8, JOY_DOWN), joystickGetDigital(1, 8, JOY_UP));
    mobileGoalControl( joystickGetDigital(1, 6, JOY_UP), joystickGetDigital(1, 6, JOY_DOWN), joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN));
    coneHandlerControl(joystickGetDigital(2, 5, JOY_UP), joystickGetDigital(2, 5, JOY_DOWN), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_UP));
    delay(20);
  }
}