#ifndef SCANNER
#define SCANNER

#include <Arduino.h>

#include "motors_api.h"

class Scanner
{
public:
    Scanner(Motor x_motor, Motor z_motor,
        Motor scanner_motor, Motor table_motor,
        size_t x_stop_pin, size_t z_stop_pin);

    void init();

    void move(int32_t x, int32_t z);
    void move_and_rotate(int32_t x, int32_t z, int32_t degree);
    void rotate(int32_t degree);
    void move_to_zero();
    void rotate_to_zero();

    void table_rotate(int32_t degree);
    void move_and_rotate_table(int32_t x, int32_t z, int32_t degree);
    void move_and_rotate_scanner_and_table(int32_t x, int32_t z, int32_t s_degree, int32_t degree);
    void table_rotate_to_zero();
private:
    class Axis
    {
    public:
        Axis(Motor motor);

        const size_t _STEPS_IN_MM;
        Motor _motor;
    };

    class Rotor
    {
    public:
        Rotor(Motor motor);
        const size_t _STEPS_IN_DEGREE;
        int64_t _accumulated_rotation;
        Motor _motor;
    };

    Axis _x_axis;
    Axis _z_axis;
    Rotor _scanner_rotor;
    Rotor _table_rotor;

    size_t _x_stop_pin;
    size_t _z_stop_pin;

    void chose_dir(int32_t &coordinate, Motor motor);
};

#endif
