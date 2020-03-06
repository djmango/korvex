#include "main.h"
#include "okapi/api.hpp"

#pragma
// ports for motors

// chassis motors
const int LEFT_MTR1 = 19;
const int LEFT_MTR2 = 11;
const int RIGHT_MTR1 = 20;
const int RIGHT_MTR2 = 12;

// lift and tray motors
const int TRAY_MTR = 17;
const int LIFT_MTR = 2;

// flywheel and intake motors
const int INTAKE_MTR1 = 3;
const int INTAKE_MTR2 = 5;

// sensors
const int IMU_PORT = 9;
const char LINE_PORT = 'G';