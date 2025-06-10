#ifndef MOTORS_API
#define MOTORS_API

enum STEP_MODE
{
    STEP_1    = 200,
    STEP_2_A  = 400,
    STEP_2_B  = 400,
    STEP_4    = 800,
    STEP_8    = 1600,
    STEP_16   = 3200,
    STEP_32   = 6400,
};

class Motor
{
public:
    Motor(size_t ena_pin, size_t dir_pin, size_t pul_pin, STEP_MODE step_mode);

    void set_up();

    void enable();
    void disable();

    void step();
    void rotate(size_t steps);

    void clockwise_dir();
    void counterclockwise_dir();
    void inverse_dir();

private:
    void set_dir();

    bool _direction;
    const STEP_MODE _step_mode;
    size_t _ena_pin;
    size_t _dir_pin;
    size_t _pul_pin;
};

#endif
