#include <Arduino.h>

#include "scanner.h"

// Distance, that the axis has passed after full rotation of the motor, mm
#define MM_IN_FULL_ROTATION (10)

volatile bool _is_x_stop = false;
volatile bool _is_z_stop = false;

void _x_stop_handler()
{
  noInterrupts();
  _is_x_stop = true;
  interrupts();
}

void _z_stop_handler()
{
  noInterrupts();
  _is_z_stop = true;
  interrupts();
}

Scanner::Axis::Axis(Motor motor):
    _motor(motor),
    _MM_IN_FULL_ROTATION(MM_IN_FULL_ROTATION) {};

Scanner::Scanner(Motor x_motor, Motor z_motor, size_t x_stop_pin, size_t z_stop_pin) :
  _x_stop_pin(x_stop_pin), _z_stop_pin(z_stop_pin),
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

  // Setting up button pins mode
  pinMode(_x_stop_pin, INPUT_PULLUP);
  pinMode(_z_stop_pin, INPUT_PULLUP);

  // Attach interrupts
  // Pin - 2 = number of interrupt channel
  attachInterrupt(_x_stop_pin - 2, _x_stop_handler, FALLING);
  attachInterrupt(_z_stop_pin - 2, _z_stop_handler, FALLING);
  
  move_to_zero();
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

void Scanner::move_to_zero()
{
  // Both coordinates in their inital positions
  bool in_zero = false;

  // X in it's initial position
  bool is_x_in_zero = false;
  // Z in it's initial position
  bool is_z_in_zero = false;

  while (!in_zero)
  {
    // If button pushed down and it is first entering
    if (_is_x_stop && !is_x_in_zero)
    {
      // Coordinate in it's initial position
      is_x_in_zero = true;

      // Stop hold the button
      move(10, 0);
    }

    if (_is_z_stop && !is_z_in_zero)
    {
      is_z_in_zero = true;

      move(0, 10);
    }

    // If both coordinates in initial position
    if (is_x_in_zero && is_z_in_zero)
    {
      // Set the corresponding flag
      in_zero = true;

      // Clear flags for button pushing
      _is_x_stop = false;
      _is_z_stop = false;
    }
    else if (!is_x_in_zero && !is_z_in_zero)
    {
      // Go to zero changing both coordinates
      move(-1, -1);
    }
    else if (is_x_in_zero)
    {
      // Go to zero changing z, because x already in zero
      move(0, -1);
    }
    else
    {
      // Go to zero changing x, because z already in zero
      move(-1, 0);
    }
  }

}