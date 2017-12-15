#include "main.h"
#include "constants.h"


// globals
bool PidIsEnabled = true;

/*-----------------------------------------------------------------------------*/
/*  A argument based encoder pid */
/*-----------------------------------------------------------------------------*/
int encoderPid(int encoderTarget, float encoderCalcValue, int pidKp, int pidKi, int pidKd)
{
    float pidError;
    float pidLastError;
    float pidIntegral;
    float pidDerivative;
    float pidDrive;
    pidLastError = 0;
    pidIntegral = 0;
    while (PidIsEnabled == true)
    {
        // calculate error
        pidError = encoderCalcValue - encoderTarget;

        // integral - if Ki is not 0
        if (pidKi != 0)
        {
            // If we are inside controlable window then integrate the error
            if (abs(pidError) < PID_INTEGRAL_LIMIT)
                pidIntegral = pidIntegral + pidError;
            else
                pidIntegral = 0;
        }
        else
            pidIntegral = 0;

        // calculate the derivative
        pidDerivative = pidError - pidLastError;
        pidLastError = pidError;

        // calculate drive
        pidDrive = (pidKp * pidError) + (pidKi * pidIntegral) + (pidKd * pidDerivative);
        // limit drive
        if (pidDrive > PID_DRIVE_MAX)
            pidDrive = PID_DRIVE_MAX;
        if (pidDrive < PID_DRIVE_MIN)
            pidDrive = PID_DRIVE_MIN;
        // send back
        return pidDrive;
        delay(50);
        // Run at 50Hz
    }
}