#include "main.h"

// // debug enabler
// bool debugGlobal; // set this to true whenever you want print statements in terminal

// // rerun globals
// bool isRecording; // ill get to this stuff eventually :wink:
// bool isReplaying;

namespace korvex
{

// motor functions
void motorMoveAbs(pros::Motor &motor, int target, int buffer, int maxVelocity);
void motorTBH(pros::Motor &motor, int target, int buffer, int gain);
} // namespace korvex
