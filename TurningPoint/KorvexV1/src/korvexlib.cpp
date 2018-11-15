#include "main.h"
#include "korvexlib.h"

// all korvexlib functions are located in korvex namespace for simplicity
namespace korvex
{
// additional korvex motor functions, the original pros motor class must be passed in order for telemetry + order functions to operate correctly
void motorTBH(pros::Motor &motor, int target, int buffer = 5, int gain = 1)
{
    // set motor, declerations, loop tbh
    int error = 0;
    int prev_error = 0;
    int velocity = 0;
    int output = 0;
    int tbh = 0;
    while (true)
    {
        velocity = motor.get_actual_velocity();
        error = target - velocity;                                               // calculate the error;
        output += gain * error;                                                  // integrate the output;
        if (!((velocity < (target + buffer)) && (velocity > (target - buffer)))) // if not within buffer range
        {
            output = 0.5 * (output + tbh); // then Take Back Half
            tbh = output;                  // update Take Back Half variable
            prev_error = error;            // and save the previous error
        }
        motor.move(output);
    }
}

void motorPID()
{
    // pull data from datastruct
    pros::Motor motor(FLY_MTR, pros::E_MOTOR_GEARSET_06, true);
    int targetVel = flywheelTarget;
    float kP = 0;
    float kI = 1;
    float kD = .05;

    // delcare
    int error = 0;
    int prev_error = 0;
    int velocity = 0;
    int output = 0;
    int p = 0;
    int i = 0;
    int d = 0;
    while (true) {
        velocity = motor.get_actual_velocity();
        
        error = targetVel - velocity;
        // pid calc
        p = (error * kP);
        if (abs(error) < (targetVel / 2)) // if the error is greater than half of target
            i = ((i + error) * kI);
        else
            i = 0;
        d = ((error - prev_error) * kD);
        // store last error
        prev_error = error;

        // calculate output
        output = (p + i + d);

        // apply output
        printf("fly output%d\n", output);
        motor.move(output);
        pros::delay(50);
    }
}
} // namespace korvex
