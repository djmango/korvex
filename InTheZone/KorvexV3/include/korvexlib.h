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
int driveRightTarget;

// dr4b globals
int dr4bLeftTarget;
int dr4bRightTarget;

// chain globals
int chainTarget;

// auto stacker globals
bool autoStackerEnabled;

// drive pids
void driveLeftPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd);
void driveRightPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd);

// dr4b pids
void dr4bLeftPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd);
void dr4bRightPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd);

// chain pid
void chainPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd);

// ext
void autoStacker(int coneIncrement, bool isDriverload);