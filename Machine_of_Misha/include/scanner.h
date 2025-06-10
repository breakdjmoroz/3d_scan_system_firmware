#ifndef SCANNER
#define SCANNER

#include <Arduino.h>

#include "motors_api.h"

class Scanner
{
public:
    Scanner(Motor x_motor, Motor z_motor);

    void init();
    void move(int32_t x, int32_t z);
private:
    class Axis
    {
    public:
        Axis(Motor motor);

        const size_t _MM_IN_FULL_ROTATION;
        Motor _motor;
    };

    Axis _x_axis;
    Axis _z_axis;

    void chose_dir(int32_t &coordinate, Axis axis);
};

#endif
