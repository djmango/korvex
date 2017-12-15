#include "main.h"

Encoder leftencoder;
Encoder rightencoder;
Encoder chainencoder;

void encoderPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd);