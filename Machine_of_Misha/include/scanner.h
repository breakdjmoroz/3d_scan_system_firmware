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

    #if DEPRECATED
    void table_rotate_to_zero();

    void rotate_to_zero();
    #endif
private:
    class Motorized
    {
    public:
        virtual void init();

        virtual void choose_direction(int32_t distance);
        virtual uint32_t distance_to_steps(int32_t distance);

        virtual void move();
    private:
    };

    class Axis : public Motorized
    {
    public:
        Axis(Motor motor);

        void init() override;
        void move() override;

        uint32_t distance_to_steps(int32_t distance) override;
        void choose_direction(int32_t distance) override;

    private:
        const size_t _STEPS_IN_MM;
        Motor _motor;
    };

    class Rotor : public Motorized
    {
    public:
        Rotor(Motor motor);

        void init() override;
        void move() override;

        uint32_t distance_to_steps(int32_t distance) override;
        void choose_direction(int32_t distance) override;

    private:
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

    void Scanner::parallel_move(Motorized* motors[], int32_t distanses[], size_t motors_number);
};

#endif
