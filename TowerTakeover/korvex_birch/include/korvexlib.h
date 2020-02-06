#include "main.h"
#include "okapi/api.hpp"

#pragma
// ports for motors

// chassis motors
const int LEFT_MTR1 = 9;
const int LEFT_MTR2 = 10;
const int RIGHT_MTR1 = 2;
const int RIGHT_MTR2 = 1;

// lift and tray motors
const int TRAY_MTR = 7;
const int LIFT_MTR = 13;

// flywheel and intake motors
const int INTAKE_MTR1 = 12;
const int INTAKE_MTR2 = 20;

// sensors
const int IMU_PORT = 6;
const char LINE_PORT = 'H';