#include "main.h"

Encoder leftencoder;
Encoder rightencoder;
Encoder chainencoder;

//globals

bool drivePidIsEnabled;
int driveLeftTarget;
int driveRightTarget;

void driveLeftPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd);
void driveRightPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd);