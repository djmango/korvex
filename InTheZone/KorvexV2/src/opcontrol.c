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
float pidKpDriveO = 2.0;
float pidKiDriveO = 0.1;
float pidKdDriveO = 0;
bool PIDenabledDriveO = false;

// dr4b pid floats
float pidKpDr4bO = 2.0;
float pidKiDr4bO = 0.01;
float pidKdDr4bO = 0;
bool PIDenabledDr4bO = false;

// left drive values
int encoderValueLeft;
int encoderTargetLeft;
int motorDriveLeft;

// right drive values
int encoderValueRight;
int encoderTargetRight;
int motorDriveRight;

// right dr4b values
int potValueRightDr4b;
int potTargetRightDr4b;
int motorDr4bRight;

// left dr4b values
int potValueLeftDr4b;
int potTargetLeftDr4b;
int motorDr4bLeft;

int isFineControl = false;
int fineControl = 1;

int dr4bPotTargetHolder; //dr4b pot target holder
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
  while (PIDenabledDriveO == true) {
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
    motorSet(motorDriveLeft, pidDrive);
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
  while (PIDenabledDriveO == true) {
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
    motorSet(motorDriveRight, pidDrive);
    delay(25);
    // Run at 50Hz
  }
}
/*-----------------------------------------------------------------------------*/
/*  Calculate error and drive for right dr4b */
/*-----------------------------------------------------------------------------*/
void dr4bRightPIDO() {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  float pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (PIDenabledDr4bO == true) {
    float potCalcValue = (potValueRightDr4b * 31.9024 / 360);
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

    // send to motor
    motorSet(motorDr4bRight, pidDrive);
    delay(25);
    // Run at 50Hz
  }
}
/*-----------------------------------------------------------------------------*/
/*  Calculate error and drive for left dr4b */
/*-----------------------------------------------------------------------------*/
void dr4bLeftPIDO() {
  float pidError;
  float pidLastError;
  float pidIntegral;
  float pidDerivative;
  float pidDrive;
  pidLastError = 0;
  pidIntegral = 0;
  while (PIDenabledDr4bO == true) {
    float potCalcValue = (potValueLeftDr4b * 31.9024 / 360);
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

    // send to motor
    motorSet(motorDr4bLeft, pidDrive);
    delay(25);
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

void updateDrive(int chassisControlLeft, int chassisControlRight,
                 int liftControl) {
  // chassis control
  motorSet(2, (chassisControlRight * fineControl));
  motorSet(3, (chassisControlLeft * fineControl));
  // lift control
  // motorSet(6, (liftControl));
  // motorSet(7, (liftControl));
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
  motorSet(8, (joystickGetAnalog(2, 3)));
  motorSet(8, (joystickGetAnalog(2, 3)));
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
port 8 = chain bar (y-cable)
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

void operatorControl() {
  lcdText();
  // dr4b stuff, same format
  PIDenabledDr4bO = true;
  potValueLeftDr4b = analogRead(3);
  potValueRightDr4b = analogRead(4);
  motorDr4bLeft = dr4bLeft;
  motorDr4bRight = dr4bRight;
  TaskHandle dr4bTaskHandleLeft = taskRunLoop(dr4bLeftPIDO, 25);
  TaskHandle dr4bTaskHandleRight = taskRunLoop(dr4bRightPIDO, 25);
  while (isEnabled()) {
    // dr4bLeftPIDO();
    // dr4bRightPIDO();
    printf("left read %d\n", analogRead(3));
    printf("right read %d\n", analogRead(4));
    // chassisControl chassisControlLeft, chassisControlRight, liftControl
    // fineControlToggle fineBtn
    // mobileGoalControl moboLiftBtn, moboTiltBtn
    // coneHandlerControl clawBtn, chainbarBtn
    updateDrive(joystickGetAnalog(1, 3), joystickGetAnalog(1, 2),
                joystickGetAnalog(2, 2));
    potTargetLeftDr4b = dr4bPotTargetHolder;
    potTargetRightDr4b = dr4bPotTargetHolder;
    if (joystickGetAnalog(2, 2) > 25 || joystickGetAnalog(2, 2) < 25) {
      dr4bPotTargetHolder = dr4bPotTargetHolder + (joystickGetAnalog(2, 2) * 1);
    }
    fineControlToggle(joystickGetDigital(1, 7, JOY_DOWN));
    mobileGoalControl(
        joystickGetDigital(2, 6, JOY_UP), joystickGetDigital(2, 6, JOY_DOWN),
        joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN));
    coneHandlerControl(
        joystickGetDigital(2, 5, JOY_UP), joystickGetDigital(2, 5, JOY_DOWN),
        joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_UP));
    delay(20);
  }
  taskDelete(dr4bTaskHandleLeft);
  taskDelete(dr4bTaskHandleRight);
}
