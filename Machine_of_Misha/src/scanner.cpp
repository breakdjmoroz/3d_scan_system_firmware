#include <Arduino.h>

#include "scanner.h"

// Distance, that the axis has passed after full rotation of the motor, mm
#define MM_IN_FULL_ROTATION (10)

// Angel, that rotor has passed after full rotation of the motor, degrees
#define DEGREE_IN_FULL_ROTATION (360)

// Interrupt flags becoming true when stop button pushed
volatile bool _is_x_stop = false;
volatile bool _is_z_stop = false;

// Stop button ISR
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
    _STEPS_IN_MM(motor._STEPS_IN_FULL_ROTATION / MM_IN_FULL_ROTATION) {};

Scanner::Rotor::Rotor(Motor motor):
    _motor(motor),
    _STEPS_IN_DEGREE(motor._STEPS_IN_FULL_ROTATION / DEGREE_IN_FULL_ROTATION),
    _accumulated_rotation(0) {};

Scanner::Scanner(Motor x_motor, Motor z_motor, Motor scanner_motor, Motor table_motor,
  size_t x_stop_pin, size_t z_stop_pin) :
  _x_stop_pin(x_stop_pin), _z_stop_pin(z_stop_pin),
  _x_axis(Axis(x_motor)), _z_axis(Axis(z_motor)),
  _scanner_rotor(Rotor(scanner_motor)), _table_rotor(Rotor(table_motor)) {};

void Scanner::move(int32_t x, int32_t z)
{
    // Time between steps. Influence on speed
    const size_t PERIOD = ((USEC_IN_SEC * 10) / ((MAX_FREQUENCY / 100) * 60));

    uint32_t x_timer;
    uint32_t z_timer;

    // Chose direction
    chose_dir(x, _x_axis._motor);
    chose_dir(z, _z_axis._motor);

    // Convert coordinates into steps
    uint32_t x_steps = x * _x_axis._STEPS_IN_MM;
    uint32_t z_steps = z * _z_axis._STEPS_IN_MM;

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

void Scanner::rotate(int32_t degree)
{
    // Accumulate rotations to return to zero position
    _scanner_rotor._accumulated_rotation += degree;
    // The same as move function
    const size_t PERIOD = ((USEC_IN_SEC * 10) / ((MAX_FREQUENCY / 100) * 20));

    uint32_t timer;

    chose_dir(degree, _scanner_rotor._motor);

    /* TODO: remove magic numbers!!! */
    uint32_t steps = degree * 33152;

    while (steps--)
    {
      if ((micros() - timer >= PERIOD) && steps)
      {
        _scanner_rotor._motor.step();
        timer = micros();
      }
    }
}

void Scanner::move_and_rotate(int32_t x, int32_t z, int32_t degree)
{
    _scanner_rotor._accumulated_rotation += degree;

    // The same as move function
    const size_t PERIOD = ((USEC_IN_SEC * 10) / ((MAX_FREQUENCY / 100) * 60));

    uint32_t x_timer;
    uint32_t z_timer;
    uint32_t degree_timer;

    chose_dir(x, _x_axis._motor);
    chose_dir(z, _z_axis._motor);
    chose_dir(degree, _scanner_rotor._motor);

    uint32_t x_steps = x * _x_axis._STEPS_IN_MM;
    uint32_t z_steps = z * _z_axis._STEPS_IN_MM;
    /* TODO: remove all magic numbers!!!*/
    uint32_t degree_steps = degree * 33152;

    while (x_steps || z_steps || degree_steps)
    {
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
      if ((micros() - degree_timer >= PERIOD) && degree_steps)
      {
        _scanner_rotor._motor.step();
        --degree_steps;
        degree_timer = micros();
      }
    }
}

void Scanner::init()
{
  // Enable motors
  _x_axis._motor.enable();
  _z_axis._motor.enable();
  _scanner_rotor._motor.enable();
  _table_rotor._motor.enable();

  // Setting motors up
  _x_axis._motor.set_up();
  _z_axis._motor.set_up();
  _scanner_rotor._motor.set_up();
  _table_rotor._motor.set_up();

  // Setting up button pins mode
  pinMode(_x_stop_pin, INPUT_PULLUP);
  pinMode(_z_stop_pin, INPUT_PULLUP);

  // Attach interrupts
  // Pin - 2 = number of interrupt channel
  attachInterrupt(digitalPinToInterrupt(_x_stop_pin), _x_stop_handler, FALLING);
  attachInterrupt(digitalPinToInterrupt(_z_stop_pin), _z_stop_handler, FALLING);
  
  rotate_to_zero();
  move_to_zero();
}

void Scanner::chose_dir(int32_t &coordinate, Motor motor)
{
    // Chose direction of rotation
    if (coordinate < 0)
    {
      coordinate = -coordinate;
      motor.counterclockwise_dir();
    }
    else if (coordinate > 0)
    {
      motor.clockwise_dir();
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

void Scanner::rotate_to_zero()
{
  // Return to initial position
  rotate(-(_scanner_rotor._accumulated_rotation % 18));
}

void Scanner::table_rotate(int32_t degree)
{
    // Accumulate rotations to return to zero position
    _table_rotor._accumulated_rotation += degree;

    // The same as move function
    const size_t PERIOD = ((USEC_IN_SEC * 10) / ((MAX_FREQUENCY / 100) * 10));

    uint32_t timer;

    chose_dir(degree, _table_rotor._motor);

    uint32_t steps = degree * _table_rotor._motor._STEPS_IN_FULL_ROTATION;

    while (steps--)
    {
      if ((micros() - timer >= PERIOD) && steps)
      {
        _table_rotor._motor.step();
        timer = micros();
      }
    }
}

void Scanner::table_rotate_to_zero()
{
  // Return to initial position
  rotate(-_table_rotor._accumulated_rotation % 18);
}

void Scanner::move_and_rotate_table(int32_t x, int32_t z, int32_t degree)
{
    _table_rotor._accumulated_rotation += degree;

    // The same as move function
    const size_t PERIOD = ((USEC_IN_SEC * 10) / ((MAX_FREQUENCY / 100) * 60));
    const size_t TABLE_PERIOD = ((USEC_IN_SEC * 10) / ((MAX_FREQUENCY / 100) * 10));

    uint32_t x_timer;
    uint32_t z_timer;
    uint32_t degree_timer;

    chose_dir(x, _x_axis._motor);
    chose_dir(z, _z_axis._motor);
    chose_dir(degree, _table_rotor._motor);

    uint32_t x_steps = x * _x_axis._STEPS_IN_MM;
    uint32_t z_steps = z * _z_axis._STEPS_IN_MM;
    /* TODO: remove all magic numbers!!!*/
    uint32_t degree_steps = degree * _table_rotor._motor._STEPS_IN_FULL_ROTATION;

    while (x_steps || z_steps || degree_steps)
    {
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
      if ((micros() - degree_timer >= TABLE_PERIOD) && degree_steps)
      {
        _table_rotor._motor.step();
        --degree_steps;
        degree_timer = micros();
      }
    }
}

void Scanner::move_and_rotate_scanner_and_table(int32_t x, int32_t z, int32_t s_degree, int32_t t_degree)
{
    _scanner_rotor._accumulated_rotation += s_degree;
    _table_rotor._accumulated_rotation += t_degree;

    // The same as move function
    const size_t PERIOD = ((USEC_IN_SEC * 10) / ((MAX_FREQUENCY / 100) * 60));
    const size_t TABLE_PERIOD = ((USEC_IN_SEC * 10) / ((MAX_FREQUENCY / 100) * 10));

    uint32_t x_timer;
    uint32_t z_timer;
    uint32_t s_degree_timer;
    uint32_t t_degree_timer;

    chose_dir(x, _x_axis._motor);
    chose_dir(z, _z_axis._motor);
    chose_dir(s_degree, _table_rotor._motor);
    chose_dir(t_degree, _table_rotor._motor);

    uint32_t x_steps = x * _x_axis._STEPS_IN_MM;
    uint32_t z_steps = z * _z_axis._STEPS_IN_MM;
    /* TODO: remove all magic numbers!!!*/
    uint32_t s_degree_steps = s_degree * 33152;
    uint32_t t_degree_steps = t_degree * _table_rotor._motor._STEPS_IN_FULL_ROTATION;

    while (x_steps || z_steps || t_degree_steps || s_degree_steps)
    {
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
      if ((micros() - s_degree_timer >= PERIOD) && s_degree_steps)
      {
        _scanner_rotor._motor.step();
        --s_degree_steps;
        s_degree_timer = micros();
      }
      if ((micros() - t_degree_timer >= TABLE_PERIOD) && t_degree_steps)
      {
        _table_rotor._motor.step();
        --t_degree_steps;
        t_degree_timer = micros();
      }
    }
}