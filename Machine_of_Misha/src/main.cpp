#include <Arduino.h>

#include "motors_api.h"
#include "scanner.h"

#define MOTOR_0_PUL_PIN           (13)  // Responsible for the motor's motion
#define MOTOR_0_DIR_PIN           (12)  // Responsible for direction of the motor's motion
#define MOTOR_0_ENA_PIN           (11)  // Responsible for enabling the motor

#define MOTOR_1_PUL_PIN           (10)
#define MOTOR_1_DIR_PIN           (9)
#define MOTOR_1_ENA_PIN           (8)

#define MOTOR_0_STOP_BUTTON_PIN   (3)   // Generate interrupt to stop the motion
#define MOTOR_1_STOP_BUTTON_PIN   (2)

Motor x_motor = {MOTOR_0_ENA_PIN, MOTOR_0_DIR_PIN, MOTOR_0_PUL_PIN, STEP_32};
Motor z_motor = {MOTOR_1_ENA_PIN, MOTOR_1_DIR_PIN, MOTOR_1_PUL_PIN, STEP_32};

Scanner scanner = {x_motor, z_motor, MOTOR_0_STOP_BUTTON_PIN, MOTOR_1_STOP_BUTTON_PIN};

size_t iterations = 2;
bool is_running = true;

void setup()
{
  scanner.init();

  scanner.move(100, 100);

  while (iterations--)
  {
    scanner.move(10, 0);
    scanner.move(0, 10);
    scanner.move(-10, 0);
    scanner.move(0, -10);
  }

  scanner.move_to_zero();
}

void loop()
{
}