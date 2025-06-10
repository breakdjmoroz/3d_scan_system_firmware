#ifndef SCANNER
#define SCANNER

#include <Arduino.h>

#include "motors_api.h"

class Scanner
{
public:
    Scanner();

    void move(int x, int z);
private:
    class Axis
    {
    public:
        Axis(Motor motor, size_t mm_in_full_rotation):
        _motor(motor),
        _MM_IN_FULL_ROTATION(mm_in_full_rotation) {};

        const size_t _MM_IN_FULL_ROTATION;
        Motor _motor;
    };

    Axis x_axis;
    Axis z_axis;
};

#endif
