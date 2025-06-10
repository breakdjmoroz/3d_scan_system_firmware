#include <Arduino.h>

#include "scanner.h"

// Distance, that the axis has passed after full rotation of the motor, mm
#define MM_IN_FULL_ROTATION (10)

Scanner::Axis::Axis(Motor motor):
    _motor(motor),
    _MM_IN_FULL_ROTATION(MM_IN_FULL_ROTATION) {};

Scanner::Scanner(Motor x_motor, Motor z_motor) :
  _x_axis(Axis(x_motor)), _z_axis(Axis(z_motor)){};

void Scanner::move(int32_t x, int32_t z)
{
    const size_t PERIOD = ((USEC_IN_SEC * 10) / ((MAX_FREQUENCY / 100) * 60));

    uint32_t x_timer;
    uint32_t z_timer;

    // Chose direction
    chose_dir(x, _x_axis);
    chose_dir(z, _z_axis);

    // Convert coordinates into steps
    uint32_t x_steps = x * (_x_axis._motor._STEP_MODE / _x_axis._MM_IN_FULL_ROTATION);
    uint32_t z_steps = z * (_z_axis._motor._STEP_MODE / _z_axis._MM_IN_FULL_ROTATION);

    while (x_steps || z_steps)
    {
      // Move scanner
      if ((micros() - x_timer >= PERIOD) && x_steps)
      {
        _x_axis._motor.step();
        --x_steps;
        x_timer = micros();
      }
      if ((micros() - z_timer >= PERIOD) && z_steps)
      {
        _z_axis._motor.step();
        --z_steps;
        z_timer = micros();
      }
    }
}

void Scanner::init()
{
  // Enable motors
  _x_axis._motor.enable();
  _z_axis._motor.enable();

  // Setting motors up
  _x_axis._motor.set_up();
  _z_axis._motor.set_up();
}

void Scanner::chose_dir(int32_t &coordinate, Axis axis)
{
    // Chose direction of rotation
    if (coordinate < 0)
    {
      coordinate = -coordinate;
      axis._motor.counterclockwise_dir();
    }
    else if (coordinate > 0)
    {
      axis._motor.clockwise_dir();
    }
}