#include <Arduino.h>

// Microseconds in second
#define USEC_IN_SEC       (1e6)

// Enable the motor constant
#define ENABLE_MOTOR      (1)
// Disable the motor constant
#define DISABLE_MOTOR     (0)

// Define high level for pulse
#define PULSE_HIGH        (1)
// Define low level for pulse
#define PULSE_LOW         (0)

// Responsible for the motor's motion
#define MOTOR_0_PUL_PIN   (13)
// Responsible for direction of the motor's motion
#define MOTOR_0_DIR_PIN   (12)
// Responsible for enabling the motor
#define MOTOR_0_ENA_PIN   (11)

#define MOTOR_1_PUL_PIN   (10)
#define MOTOR_1_DIR_PIN   (9)
#define MOTOR_1_ENA_PIN   (8)

// Frequency of the motor's motion, Hz
#define MOTOR_0_FREQUENCY (1000)
// Period of one impulse of the PUL signal, us
#define MOTOR_0_PERIOD    (USEC_IN_SEC / MOTOR_0_FREQUENCY)

#define MOTOR_1_FREQUENCY (10000)
#define MOTOR_1_PERIOD    (USEC_IN_SEC / MOTOR_1_FREQUENCY)

// An interval, while waiting of setting up new signal, us
#define WAIT_INTERVAL     (5)

#define STEP_IN_ONE_DIR   (10000)

bool motor_0_dir = false;

void move_motor(size_t motor_pul_pin, size_t period);

void setup()
{
  // Setting up pins to output mode
  pinMode(MOTOR_0_PUL_PIN, OUTPUT);
  pinMode(MOTOR_0_DIR_PIN, OUTPUT);
  pinMode(MOTOR_0_ENA_PIN, OUTPUT);

  pinMode(MOTOR_1_PUL_PIN, OUTPUT);
  pinMode(MOTOR_1_DIR_PIN, OUTPUT);
  pinMode(MOTOR_1_ENA_PIN, OUTPUT);

  // Enable motors
  digitalWrite(MOTOR_0_ENA_PIN, ENABLE_MOTOR);
  digitalWrite(MOTOR_1_ENA_PIN, ENABLE_MOTOR);
}

void loop()
{
  // Wait before setting up new signal (according to docs)
  delayMicroseconds(WAIT_INTERVAL);

  // Set direction of the motor's motion
  digitalWrite(MOTOR_0_DIR_PIN, motor_0_dir);
  digitalWrite(MOTOR_1_DIR_PIN, motor_0_dir);

  delayMicroseconds(WAIT_INTERVAL);

  // Move the motor
  for (size_t i = 0; i < STEP_IN_ONE_DIR; ++i)
  {
    move_motor(MOTOR_0_PUL_PIN, MOTOR_0_PERIOD);
  }

  motor_0_dir = !motor_0_dir;
}

void move_motor(size_t motor_pul_pin, size_t period)
{
    // Create impulses on PUL pin to move the motor
    digitalWrite(motor_pul_pin, PULSE_HIGH);

    // Wait a half of period
    delayMicroseconds((uint32_t)(period / 2));

    digitalWrite(motor_pul_pin, PULSE_LOW);

    delayMicroseconds((uint32_t)(period / 2));
}