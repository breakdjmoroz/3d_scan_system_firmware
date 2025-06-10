#include <Arduino.h>

#include "motors_api.h"

#define MOTOR_0_PUL_PIN   (13)  // Responsible for the motor's motion
#define MOTOR_0_DIR_PIN   (12)  // Responsible for direction of the motor's motion
#define MOTOR_0_ENA_PIN   (11)  // Responsible for enabling the motor

#define MOTOR_1_PUL_PIN   (10)
#define MOTOR_1_DIR_PIN   (9)
#define MOTOR_1_ENA_PIN   (8)

// Timer's threshold in us
#define THRESHOLD_0       (1)
#define THRESHOLD_1       (1)

// Timer's variables to make multitasking
uint32_t timer;
uint32_t timer0;
uint32_t timer1;

Motor motor_0 = {MOTOR_0_ENA_PIN, MOTOR_0_DIR_PIN, MOTOR_0_PUL_PIN, STEP_32};
Motor motor_1 = {MOTOR_1_ENA_PIN, MOTOR_1_DIR_PIN, MOTOR_1_PUL_PIN, STEP_32};

void setup()
{
  // Setting up motors
  motor_0.set_up();
  motor_1.set_up();

  motor_1.counterclockwise_dir();
}

uint32_t motor_0_step = 0;
uint32_t motor_1_step = 0;

bool is_running = true;

void loop()
{
  #if 1

  motor_1.rotate(6400);

  #endif

  #if 1
  if ((micros() - timer1 >= THRESHOLD_1) && is_running)
  {
    //motor_1.rotate(64000);
    motor_0.disable();
    is_running = false;
  }
  #endif

}