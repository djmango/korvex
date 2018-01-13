#include "main.h"

Encoder leftencoder;
Encoder rightencoder;
Encoder chainencoder;
Encoder dr4bleftencoder;
Encoder dr4brightencoder;

// globals

// drive globals
bool drivePidIsEnabled;
int driveLeftTarget;
float driveLeftValue;
int driveRightTarget;
float driveRightValue;

// auto stacker globals
int coneIncrementGlobal;
bool isDriverloadGlobal;
bool autoStackerEnabled;

// control modifiers
bool isReverse = false;
bool isFineControl = false;
float fineControl = 1;

// opcontrol functions
void driveControl(int chassisControlLeft, int chassisControlRight);
void dr4bControl(int dr4bControl);
void fineControlToggle(int fineBtn, int fineBtn2, int reverseBtn,
                       int reverseBtn2);
void mobileGoalControl(int moboLiftBtnUp, int moboLiftBtnDown);
void coneHandlerControl(int clawBtnUp, int clawBtnDown, int chainControl);
void autoStackControl(int incrementUpBtn, int incrementDownBtn,
                      int incrementResetBtn, int driverloadBtn,
                      int fieldloadBtn, int incrementUpNoFuncBtn);

// drive pids
void driveLeftPid(int pidKp, int pidKi, int pidKd);
void driveRightPid(int pidKp, int pidKi, int pidKd);

// lift control
void liftTo(int liftTarget, int chainTarget, int waitTo);

// ext
void autoStacker(int coneIncrement, bool isDriverload);