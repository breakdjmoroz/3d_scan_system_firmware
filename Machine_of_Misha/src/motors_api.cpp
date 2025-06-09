#include <Arduino.h>

#include "motors_api.h"

// Microseconds in second
#define USEC_IN_SEC       (1e6)

// An interval, while waiting of setting up new signal, us
#define WAIT_INTERVAL     (5)

// Frequency of the motor's motion, Hz
#define MAX_FREQUENCY     (200000)
// Period of one impulse of the PUL signal, us
#define PERIOD            (USEC_IN_SEC / MAX_FREQUENCY)

void Motor::enable()
{
    // Enable the motor
    digitalWrite(_ena_pin, ENABLE_MOTOR);
}

void Motor::disable()
{
    // Disable the motor
    digitalWrite(_ena_pin, DISABLE_MOTOR);
}

void Motor::step()
{
    // Create impulses on PUL pin to move the motor
    digitalWrite(_pul_pin, PULSE_HIGH);

    // Wait a half of period
    delayMicroseconds((uint32_t)(PERIOD / 2));

    // Same actions for the second half of period
    digitalWrite(_pul_pin, PULSE_LOW);

    delayMicroseconds((uint32_t)(PERIOD / 2));
}

void Motor::set_dir()
{
    // Wait as it said in specification
    delayMicroseconds(WAIT_INTERVAL);

    // Change direction of motion
    digitalWrite(_dir_pin, _direction);

    // The same as in the begining
    delayMicroseconds(WAIT_INTERVAL);
}

void Motor::forward_dir()
{
    // Setting up forward direction of the motor's step
    _direction = DIR_FORWARD;
    set_dir();
}

void Motor::reverse_dir()
{
    // Setting up reverse direction of the motor's step
    _direction = DIR_REVERSE;
    set_dir();
}

void Motor::inverse_dir()
{
    // Invert direction of the motor's step
    _direction = !_direction;
    set_dir();
}
