#include <Arduino.h>

#define USEC_IN_SEC    (1000000)

#define PUL_PIN        (13)                        // Responsible for the motor's motion
#define DIR_PIN        (12)                        // Responsible for direction of the motor's motion
#define ENA_PIN        (11)                        // Responsible for enabling the motor

#define FREQUENCY      (1000)                      // Frequency of the motor's motion, Hz
#define PERIOD         (USEC_IN_SEC / FREQUENCY)   // Period of one impulse of the PUL signal, us

#define WAIT_INTERVAL  (5)                         // An interval, while waiting of setting up new signal, us

void setup()
{
  // Setting up pins to output mode
  pinMode(PUL_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);
}

void loop()
{
  // Enable motor
  digitalWrite(ENA_PIN, 0);

  // Wait before setting up new signal (according to docs)
  delayMicroseconds(WAIT_INTERVAL);

  // Set direction of the motor's motion
  digitalWrite(DIR_PIN, 0);

  delayMicroseconds(WAIT_INTERVAL);

  // Create impulses on PUL pin to move the motor
  digitalWrite(PUL_PIN, 1);

  // Wait a half of period
  delayMicroseconds((uint32_t)(PERIOD / 2));

  digitalWrite(PUL_PIN, 0);

  delayMicroseconds((uint32_t)(PERIOD / 2));

  // Disable motor
  digitalWrite(ENA_PIN, 1);

  // Wait one sec
  delayMicroseconds(1000000);
}
