#include <Arduino.h>

#define USEC_IN_SEC       (1e6)                    // Set's us in sec

#define ENABLE_MOTOR      (1)                          // Enable the motor constant
#define DISABLE_MOTOR     (0)                          // Disable the motor constant

#define PULSE_HIGH        (1)                          // Define high level for pulse
#define PULSE_LOW         (0)                          // Define low level for pulse

#define MOTOR_0_PUL_PIN   (13)                         // Responsible for the motor's motion
#define MOTOR_0_DIR_PIN   (12)                         // Responsible for direction of the motor's motion
#define MOTOR_0_ENA_PIN   (11)                         // Responsible for enabling the motor

#define FREQUENCY         (10000)                       // Frequency of the motor's motion, Hz
#define PERIOD            (USEC_IN_SEC / FREQUENCY)    // Period of one impulse of the PUL signal, us

#define WAIT_INTERVAL     (5)                          // An interval, while waiting of setting up new signal, us

#define STEP_IN_ONE_DIR   (10000)

bool motor_0_dir = false;

void setup()
{
  // Setting up pins to output mode
  pinMode(MOTOR_0_PUL_PIN, OUTPUT);
  pinMode(MOTOR_0_DIR_PIN, OUTPUT);
  pinMode(MOTOR_0_ENA_PIN, OUTPUT);

  // Enable motor
  digitalWrite(MOTOR_0_ENA_PIN, ENABLE_MOTOR);
}

void loop()
{
  // Wait before setting up new signal (according to docs)
  delayMicroseconds(WAIT_INTERVAL);

  // Set direction of the motor's motion
  digitalWrite(MOTOR_0_DIR_PIN, motor_0_dir);

  delayMicroseconds(WAIT_INTERVAL);

  for (size_t i = 0; i < STEP_IN_ONE_DIR; ++i)
  {
    // Create impulses on PUL pin to move the motor
    digitalWrite(MOTOR_0_PUL_PIN, PULSE_HIGH);

    // Wait a half of period
    delayMicroseconds((uint32_t)(PERIOD / 2));

    digitalWrite(MOTOR_0_PUL_PIN, PULSE_LOW);

    delayMicroseconds((uint32_t)(PERIOD / 2));
  }

  motor_0_dir = !motor_0_dir;
}
