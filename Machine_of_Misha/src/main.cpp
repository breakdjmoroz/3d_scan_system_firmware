#include <Arduino.h>

#include "motors_api.h"

// Responsible for the motor's motion
#define MOTOR_0_PUL_PIN   (13)
// Responsible for direction of the motor's motion
#define MOTOR_0_DIR_PIN   (12)
// Responsible for enabling the motor
#define MOTOR_0_ENA_PIN   (11)

#define MOTOR_1_PUL_PIN   (10)
#define MOTOR_1_DIR_PIN   (9)
#define MOTOR_1_ENA_PIN   (8)

Motor motor_0 = {MOTOR_0_ENA_PIN, MOTOR_0_DIR_PIN, MOTOR_0_PUL_PIN};
Motor motor_1 = {MOTOR_1_ENA_PIN, MOTOR_1_DIR_PIN, MOTOR_1_PUL_PIN};

// Frequency of motors in % from max
#define FREQUENCY_0       (10)
#define FREQUENCY_1       (20)

// Timer's threshold in us
#define THRESHOLD_0       (100)
#define THRESHOLD_1       ((MAX_FREQUENCY / 100) * FREQUENCY_1)

// Timer's variables to make multitasking
uint32_t timer;
uint32_t timer0;
uint32_t timer1;

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
  motor_0.enable();
  motor_1.enable();

  // Set primary motors direction
  motor_0.forward_dir();
  motor_1.forward_dir();
}

uint32_t motor_0_step = 0;
uint32_t motor_1_step = 0;

void loop()
{
  #if 1
  // Move the motor
  if (micros() - timer0 >= THRESHOLD_0)
  {
    if (motor_0_step > 1000)
    {
      motor_0_step = 0;
      motor_0.inverse_dir();
    }

    timer0 = micros();
    ++motor_0_step;
    motor_0.step();
  }
  #endif

  #if 0
  if (micros() - timer1 >= THRESHOLD_1)
  {
    if (motor_1_step > 10000)
    {
      motor_1_step = 0;
      motor_1.inverse_dir();
    }

    timer1 = micros();
    motor_1.step();
    ++motor_1_step;
  }
  #endif

}