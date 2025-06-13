#include <Arduino.h>

#include "scanner.h"

// Distance, that the axis has passed after full rotation of the motor, mm
#define MM_IN_FULL_ROTATION (10)

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

Scanner::Motorized::Motorized(Motor motor): _motor(motor) {};

void Scanner::Motorized::init()
{
  // Enable and set up motor
  _motor.enable();
  _motor.set_up();
}

void Scanner::Motorized::move()
{
  _motor.step();
}

void Scanner::Motorized::choose_direction(int32_t coordinate)
{
  // Chose direction of rotation
  if (coordinate < 0)
  {
    _motor.counterclockwise_dir();
  }
  else if (coordinate > 0)
  {
    _motor.clockwise_dir();
  }
}

Scanner::Axis::Axis(Motor motor): Motorized(motor),
    _STEPS_IN_MM(motor._STEPS_IN_FULL_ROTATION / MM_IN_FULL_ROTATION) {};

uint32_t Scanner::Axis::distance_to_steps(int32_t distance)
{
    // Convert coordinates into steps
    return abs(distance) * _STEPS_IN_MM;
}

Scanner::Rotor::Rotor(Motor motor, STEP_SCALER step_scaler):
    Motorized(motor),
    _DEGREES_IN_STEP(motor._STEPS_IN_FULL_ROTATION / step_scaler),
    _STEP_SCALER(step_scaler),
    _accumulated_rotation(0) {};

uint32_t Scanner::Rotor::distance_to_steps(int32_t distance)
{
    // Convert coordinates into steps
    return abs(distance) * _DEGREES_IN_STEP;
}

void Scanner::Rotor::accumulate_rotations(int32_t distance)
{
  // Accumulate rotations to compute distance to zero
  _accumulated_rotation += distance;
}

void Scanner::Rotor::skip_full_rotations()
{
  // Find distance to zero inside boundaries of the cycle
  _accumulated_rotation = _accumulated_rotation % _STEP_SCALER;
}

int64_t Scanner::Rotor::distance_to_zero()
{
  // Find distance to zero position
  skip_full_rotations();
  // Choose minimal distance
  return (abs(_accumulated_rotation) > _STEP_SCALER / 2)?
    _STEP_SCALER - _accumulated_rotation : -_accumulated_rotation;
}

Scanner::Scanner(Motor x_motor, Motor z_motor, Motor scanner_motor, Motor table_motor,
  STEP_SCALER scanner_step_scaler, STEP_SCALER table_step_scaler,
  size_t x_stop_pin, size_t z_stop_pin) :
  _x_stop_pin(x_stop_pin), _z_stop_pin(z_stop_pin),
  _x_axis(Axis(x_motor)), _z_axis(Axis(z_motor)),
  _scanner_rotor(Rotor(scanner_motor, scanner_step_scaler)),
  _table_rotor(Rotor(table_motor, table_step_scaler)) {};

void Scanner::init()
{
  // Enable motors
  _x_axis.init();
  _z_axis.init();
  _scanner_rotor.init();
  _table_rotor.init();

  // Setting up button pins mode
  pinMode(_x_stop_pin, INPUT_PULLUP);
  pinMode(_z_stop_pin, INPUT_PULLUP);

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(_x_stop_pin), _x_stop_handler, FALLING);
  attachInterrupt(digitalPinToInterrupt(_z_stop_pin), _z_stop_handler, FALLING);
  
  rotate_to_zero();
  move_to_zero();
}

void Scanner::parallel_move(Motorized* motors[], int32_t distanses[], size_t motors_number)
{
  // Selecting direction for each motor
  for (size_t i = 0; i < motors_number; ++i)
  {
    motors[i]->choose_direction(distanses[i]);
  }

  // Converting each distance to steps
  uint32_t steps[motors_number] = {};
  for (size_t i = 0; i < motors_number; ++i)
  {
    steps[i] = motors[i]->distance_to_steps(distanses[i]);
  }

  // Time between steps. Influence on speed
  const size_t PERIOD = ((USEC_IN_SEC * 10) / ((MAX_FREQUENCY / 100) * 30));

  // Timers to speed control
  uint32_t timers[motors_number] = {};

  // Moving flag
  bool is_moving = true;

  while(is_moving)
  {
    // Clear flag
    is_moving = false;

    for (size_t i = 0; i < motors_number; ++i)
    {
      // Move motors
      if ((micros() - timers[i] >= PERIOD) && steps[i])
      {
        motors[i]->move();

        --steps[i];

        timers[i] = micros();
      }

      // If there are any steps, continue to move
      is_moving = is_moving || steps[i] > 0;
    }
  }
}

void Scanner::move(int32_t x, int32_t z)
{
  // Number of motors
  size_t n = 2;

  // Motors
  Motorized* motors[n] = {&_x_axis, &_z_axis};

  // Distances to move
  int32_t distances[n] = {x, z};

  parallel_move(motors, distances, n);
}

void Scanner::rotate(int32_t degree)
{
  size_t n = 1;

  Motorized* motors[n] = {&_scanner_rotor};

  int32_t distances[n] = {degree};

  // Accumulate rotations from zero position
  _scanner_rotor.accumulate_rotations(degree);
  parallel_move(motors, distances, n);
}

void Scanner::move_and_rotate(int32_t x, int32_t z, int32_t degree)
{ 
  size_t n = 3;

  Motorized* motors[n] = {&_x_axis, &_z_axis, &_scanner_rotor};

  int32_t distances[n] = {x, z, degree};

  _scanner_rotor.accumulate_rotations(degree);
  parallel_move(motors, distances, n);
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

void Scanner::table_rotate(int32_t degree)
{ 
    size_t n = 1;

    Motorized* motors[n] = {&_table_rotor};

    int32_t distances[n] = {degree};

    _table_rotor.accumulate_rotations(degree);
    parallel_move(motors, distances, n);
}

void Scanner::move_and_rotate_table(int32_t x, int32_t z, int32_t degree)
{ 
    size_t n = 3;

    Motorized* motors[n] = {&_x_axis, &_z_axis, &_table_rotor};

    int32_t distances[n] = {x, z, degree};

    _table_rotor.accumulate_rotations(degree);
    parallel_move(motors, distances, n);
}

void Scanner::move_and_rotate_scanner_and_table(int32_t x, int32_t z, int32_t s_degree, int32_t t_degree)
{ 
    size_t n = 4;

    Motorized* motors[n] = {&_x_axis, &_z_axis, &_scanner_rotor, &_table_rotor};

    int32_t distances[n] = {x, z, s_degree, t_degree};

    _scanner_rotor.accumulate_rotations(s_degree);
    _table_rotor.accumulate_rotations(t_degree);
    parallel_move(motors, distances, n);
}

void Scanner::rotate_to_zero()
{
  // Return to initial position
  rotate(_scanner_rotor.distance_to_zero());
}

void Scanner::table_rotate_to_zero()
{
  // Return to initial position
  table_rotate(_table_rotor.distance_to_zero());
}