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

// dr4b globals
int dr4bLeftTarget;
float dr4bLeftValue;
int dr4bRightTarget;
float dr4bRightValue;

// chain globals
int chainTarget;
float chainValue;

// auto stacker globals
bool autoStackerEnabled;

// drive pids
void driveLeftPid(int pidKp, int pidKi, int pidKd);
void driveRightPid(int pidKp, int pidKi, int pidKd);

// dr4b pids
void dr4bLeftPid(float pidKp, float pidKi, float pidKd);
void dr4bRightPid(float pidKp, float pidKi, float pidKd);

// chain pid
void chainPid(float pidKp, float pidKi, float pidKd);

// ext
void autoStacker(int coneIncrement, bool isDriverload);