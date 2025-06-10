#include <Arduino.h>

#include "scanner.h"

void Scanner::move(int x, int z)
{
    // Convert coordinates into steps
    size_t x_steps = x * x_axis._motor._STEP_MODE / x_axis._MM_IN_FULL_ROTATION;
    size_t z_steps = z * z_axis._motor._STEP_MODE / z_axis._MM_IN_FULL_ROTATION;

    // Move scanner
    x_axis._motor.rotate(x_steps);
    z_axis._motor.rotate(z_steps);
}