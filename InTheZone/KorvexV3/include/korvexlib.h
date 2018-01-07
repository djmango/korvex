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
bool autoStackerEnabled;

// drive pids
void driveLeftPid(int pidKp, int pidKi, int pidKd);
void driveRightPid(int pidKp, int pidKi, int pidKd);

// lift control
void liftTo(int liftTarget, int chainTarget);

// ext
void autoStacker(int coneIncrement, bool isDriverload);