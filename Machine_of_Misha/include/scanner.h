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
    void rotate(int32_t degree);
    void move_and_rotate(int32_t x, int32_t z, int32_t degree);
    void move_to_zero();

    void table_rotate(int32_t degree);
    void move_and_rotate_table(int32_t x, int32_t z, int32_t degree);
    void move_and_rotate_scanner_and_table(int32_t x, int32_t z, int32_t s_degree, int32_t degree);

    void rotate_to_zero();
    void table_rotate_to_zero();
private:
    class Motorized
    {
    public:
        Motorized(Motor motor);

        void init();
        void move();
        void choose_direction(int32_t distance);

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
        int64_t _current_position;
    };

    class Rotor : public Motorized
    {
    public:
        Rotor(Motor motor);

        uint32_t distance_to_steps(int32_t distance) override;

        void accumulate_rotations(int32_t distance);
        int64_t distance_to_zero();

    private:
        const size_t _STEPS_IN_DEGREE;
        int64_t _accumulated_rotation;

        void skip_full_rotations();
    };

    Axis _x_axis;
    Axis _z_axis;
    Rotor _scanner_rotor;
    Rotor _table_rotor;

    size_t _x_stop_pin;
    size_t _z_stop_pin;

    void Scanner::parallel_move(Motorized* motors[], int32_t distanses[], size_t motors_number);
};

#endif
