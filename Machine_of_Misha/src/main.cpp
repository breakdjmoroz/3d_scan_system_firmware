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
Motor scanner_motor = {MOTOR_S_ENA_PIN, MOTOR_S_DIR_PIN, MOTOR_S_PUL_PIN, STEP_NEMA};
Motor table_motor   = {MOTOR_T_ENA_PIN, MOTOR_T_DIR_PIN, MOTOR_T_PUL_PIN, STEP_57HS100};

Scanner scanner = {x_motor, z_motor, scanner_motor, table_motor,
  SCALER_128, SCALER_128, MOTOR_0_STOP_BUTTON_PIN, MOTOR_1_STOP_BUTTON_PIN};

void scan_program()
{
  // Prologue
  scanner.init();

  // Go to the table
  scanner.move(300, 700);

  // Full rotation of the table
  scanner.table_rotate(128);

  // Go up and continue to scan
  scanner.move_and_rotate(0, -86, -7);

  // Full rotation of the table
  scanner.table_rotate(128);

  // Go up and continue to scan
  scanner.move_and_rotate(0, -86, -3);

  // Full rotation of the table
  scanner.table_rotate(128);

  // Go to initial position
  scanner.move_and_rotate(0, 86 * 2, 7 + 3);

  // Full rotation of the table
  scanner.table_rotate(128);

  // Epilogue
  scanner.rotate_to_zero();
  scanner.table_rotate_to_zero();
  scanner.move_to_zero();
}

void setup()
{
  scan_program();
}

void loop()
{
}