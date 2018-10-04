#include "main.h"
#include "korvexlib.h"

// all korvexlib functions are located in korvex namespace for simplicity
namespace korvex
{
// additional korvex motor functions, the original pros motor class must be passed in order for telemetry + order functions to operate correctly
void motorMoveAbs(pros::Motor &motor, int target=0, int buffer=5, int maxVelocity=600)
{
    // target is literelly just the target, like how can i explain that more
    // buffer is the range that the motor can be off by
    // max velocity allowed: velocity from -+-100, +-200, or +-600 depending on the motor's gearset
    motor.move_absolute(target, maxVelocity); // Moves t units forward
    while (!((motor.get_position() < (target + buffer)) && (motor.get_position() > (target - buffer))))
    {
        // Continue running thi loop as long as the motor is not within +-buffer units of its goal
        printf("current vel%d\n", motor.get_actual_velocity());
        printf("current err%d\n", (target - motor.get_position()));
        pros::delay(10);
    }
    printf("target of %d reached!\n\n", target);
}
} // namespace korvex
