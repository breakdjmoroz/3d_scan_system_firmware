#ifndef MOTORS_API
#define MOTORS_API

// Microseconds in second
#define USEC_IN_SEC             (1e6)

// Frequency of the motor's motion, Hz
#define MAX_FREQUENCY           (200000)

enum STEP_MODE
{
    STEP_1    = 200,
    STEP_2_A  = 400,
    STEP_2_B  = 400,
    STEP_4    = 800,
    STEP_8    = 1600,
    STEP_16   = 3200,
    STEP_32   = 6400,
    STEP_NEMA = 33152,      // Step of NEMA motor used in scanner rotor
    STEP_57HS100 = 25600,   // Step of 57HS100 motor used in table rotor
};

class Motor
{
public:
    const STEP_MODE _STEPS_IN_FULL_ROTATION;

    Motor(size_t ena_pin, size_t dir_pin, size_t pul_pin, STEP_MODE step_mode);

    void set_up();

    void enable();
    void disable();

    void step();

    void clockwise_dir();
    void counterclockwise_dir();
    void inverse_dir();
private:
    bool _direction;
    size_t _ena_pin;
    size_t _dir_pin;
    size_t _pul_pin;

    void set_dir();
};

#endif
