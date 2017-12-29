#include "main.h"

Encoder leftencoder;
Encoder rightencoder;
Encoder chainencoder;

//globals
bool drivePidIsEnabled;
int driveLeftTarget;
int driveRightTarget;

//drive pids
void driveLeftPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd);
void driveRightPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd);

//dr4b pids
void dr4bLeftPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd);
void dr4bRightPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd);

//ext
void autoStacker(int coneIncrement, bool isPreload);