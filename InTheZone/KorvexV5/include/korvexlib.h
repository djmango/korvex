#include "main.h"

// init encoders
Encoder leftencoder;
Encoder rightencoder;
Gyro gyro;

// debug enabler
bool debugGlobal; // set this to true whenever you want print statements in terminal

// rerun globals
bool isRecording; // ill get to this stuff eventually :wink:
bool isReplaying;

// opcontrol functions
void driveControl(int chassisControlLeft, int chassisControlRight);
void fineControlToggle(int fineBtn, int fineBtn2, int reverseBtn,
                       int reverseBtn2);
void mobileGoalControl(int moboLiftBtnUp, int moboLiftBtnDown);

// autonomous stuff
void driveTo(int leftTarget, int rightTarget, int waitTo); // drive pid control tuned for skills
void driveToStraight(int target, int waitTo); // drive straight pid control tuned for skills

// ext
void rerunRecord(); // recoder for rerun