#include <Arduino.h>

#include "motors_api.h"

// Microseconds in second
#define USEC_IN_SEC       (1e6)

// Frequency of the motor's motion, Hz
#define MAX_FREQUENCY     (200000)
// Period of one impulse of the PUL signal, us
#define PERIOD            (USEC_IN_SEC / MAX_FREQUENCY)

// An interval, while waiting of setting up new signal, us
#define WAIT_INTERVAL     (5)

void Motor::step()
{
    // Create impulses on PUL pin to move the motor
    digitalWrite(pul_pin, PULSE_HIGH);

    // Wait a half of period
    delayMicroseconds((uint32_t)(PERIOD / 2));

    // Same actions for the second half of period
    digitalWrite(pul_pin, PULSE_LOW);

    delayMicroseconds((uint32_t)(PERIOD / 2));
}

void Motor::set_dir(bool direction)
{
  // Wait as it said in specification
  delayMicroseconds(WAIT_INTERVAL);

  // Change direction of motion
  digitalWrite(dir_pin, direction);

  // The same as in the begining
  delayMicroseconds(WAIT_INTERVAL);
}
