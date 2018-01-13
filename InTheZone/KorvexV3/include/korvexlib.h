#include "main.h"

Encoder leftencoder;
Encoder rightencoder;
Encoder chainencoder;
Encoder dr4bleftencoder;
Encoder dr4brightencoder;

// globals

// auto stacker globals
int coneIncrementGlobal;
bool isDriverloadGlobal;
bool autoStackerEnabled;

// control modifiers
bool isReverse;
bool isFineControl;
float fineControl;

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

// drive control
void driveTo(int leftTarget, int rightTarget, int waitTo);

// lift control
void liftTo(int liftTarget, int chainTarget, int waitTo);

// ext
void autoStacker(int coneIncrement, bool isDriverload);