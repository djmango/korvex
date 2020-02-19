#include "main.h"
#include "okapi/api.hpp"

#pragma
// ports for motors
//sensors


//chassis motor ports
const int CHASSIS_LEFT_FRONT  = 19;
const int CHASSIS_LEFT_REAR   = 20;
const int CHASIIS_RIGHT_FRONT = 12;
const int CHASSIS_RIGHT_REAR  = 11;

//inkate motor ports
const int INTAKE_MOTOR_LEFT   = 15;
const int INTAKE_MOTOR_RIGHT  = 13;

//lever motor 
const int LEVER_MOTOR = 16;

//arm motor
const int ARM_MOTOR = 18;

//arm varibles
const int ARM_PRESETS[5]  = {0, -1732, -1868, -2090, -2410};
const int ARM_PRESETS_LEN = 5;

const int IMU_PORT = 1;