#include <Arduino.h>

#include "motors_api.h"
#include "scanner.h"

#define MOTOR_X_PUL_PIN           (13)  // Responsible for the motor's motion
#define MOTOR_X_DIR_PIN           (12)  // Responsible for direction of the motor's motion
#define MOTOR_X_ENA_PIN           (11)  // Responsible for enabling the motor

#define MOTOR_Z_PUL_PIN           (10)
#define MOTOR_Z_DIR_PIN           (9)
#define MOTOR_Z_ENA_PIN           (8)

#define MOTOR_S_PUL_PIN           (7)
#define MOTOR_S_DIR_PIN           (6)
#define MOTOR_S_ENA_PIN           (5)

#define MOTOR_T_PUL_PIN           (A2)
#define MOTOR_T_DIR_PIN           (A1)
#define MOTOR_T_ENA_PIN           (A0)

#define MOTOR_0_STOP_BUTTON_PIN   (3)   // Generate interrupt to stop the motion
#define MOTOR_1_STOP_BUTTON_PIN   (2)

Motor x_motor       = {MOTOR_X_ENA_PIN, MOTOR_X_DIR_PIN, MOTOR_X_PUL_PIN, STEP_32};
Motor z_motor       = {MOTOR_Z_ENA_PIN, MOTOR_Z_DIR_PIN, MOTOR_Z_PUL_PIN, STEP_32};
Motor scanner_motor = {MOTOR_S_ENA_PIN, MOTOR_S_DIR_PIN, MOTOR_S_PUL_PIN, STEP_32};
Motor table_motor   = {MOTOR_T_ENA_PIN, MOTOR_T_DIR_PIN, MOTOR_T_PUL_PIN, STEP_32};

Scanner scanner = {x_motor, z_motor, scanner_motor, table_motor, MOTOR_0_STOP_BUTTON_PIN, MOTOR_1_STOP_BUTTON_PIN};

void setup()
{
  scanner.init();

  scanner.move(100, 100);
  delay(500);
  scanner.move_and_rotate(50, 50, 64);
  delay(500);
  scanner.move_and_rotate(50, -50, -64);
  delay(500);
  scanner.move_and_rotate(-50, -50, -64);
  delay(500);
  scanner.move_and_rotate(-50, 50, 64);
  delay(1000);

  scanner.move_and_rotate(50, 50, 70);
  delay(500);
  scanner.rotate_to_zero();
  delay(500);
  scanner.move_and_rotate(-50, -70, 55);
  delay(500);
  scanner.rotate_to_zero();
  delay(500);
  scanner.move_to_zero();
}

void loop()
{
    #if 0
    scanner.rotate(18);
    delay(1000);
    #endif
}