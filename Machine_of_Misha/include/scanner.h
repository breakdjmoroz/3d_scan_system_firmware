#ifndef SCANNER
#define SCANNER

#include <Arduino.h>

#include "motors_api.h"

enum STEP_SCALER
{
    SCALER_128 = 128,
};

class Scanner
{
public:
    Scanner(Motor x_motor, Motor z_motor,
        Motor scanner_motor, Motor table_motor,
        STEP_SCALER scanner_step_scaler,
        STEP_SCALER table_step_scaler,
        size_t x_stop_pin, size_t z_stop_pin);

    // Neccessary initial function
    void init();

    // Move axis
    void move(int32_t x, int32_t z);

    // Rotate scanner
    void rotate(int32_t degree);

    // Rotate table
    void table_rotate(int32_t degree);

    // Combined funcitons
    void move_and_rotate(int32_t x, int32_t z, int32_t degree);
    void move_and_rotate_table(int32_t x, int32_t z, int32_t degree);
    void move_and_rotate_scanner_and_table(int32_t x, int32_t z, int32_t s_degree, int32_t degree);

    // Return to initial position
    void move_to_zero();
    void rotate_to_zero();
    void table_rotate_to_zero();
private:
    class Motorized
    {
    public:
        Motorized(Motor motor);

        // Neccessary initial function
        void init();

        // Move motor on one step
        void move();

        // Choose direction of the motor's rotation
        void choose_direction(int32_t distance);

        // Convert distance (mm or degrees or smth else) to steps
        virtual uint32_t distance_to_steps(int32_t distance);

    protected:
        Motor _motor;
    };

    class Axis : public Motorized
    {
    public:
        Axis(Motor motor);

        uint32_t distance_to_steps(int32_t distance) override;

    private:
        const size_t _STEPS_IN_MM;
    };

    class Rotor : public Motorized
    {
    public:
        Rotor(Motor motor, STEP_SCALER step_scaler);

        uint32_t distance_to_steps(int32_t distance) override;

        // Sum new distance with accumulated value
        // Need to compute distance to zero postition
        void accumulate_rotations(int32_t distance);

        // Return distance to zero position
        int64_t distance_to_zero();

    private:
        const size_t _DEGREES_IN_STEP;
        const STEP_SCALER _STEP_SCALER;
        int64_t _accumulated_rotation;

        // Cut off unneccessary rotations
        void skip_full_rotations();
    };

    Axis _x_axis;
    Axis _z_axis;
    Rotor _scanner_rotor;
    Rotor _table_rotor;

    size_t _x_stop_pin;
    size_t _z_stop_pin;

    // Perform different move actions (rotation, moving) simaltaneously
    void parallel_move(Motorized* motors[], int32_t distanses[], size_t motors_number);
};

#endif
