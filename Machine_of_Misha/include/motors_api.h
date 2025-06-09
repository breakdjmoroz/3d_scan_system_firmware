#ifndef MOTORS_API
#define MOTORS_API

// Enable the motor constant
#define ENABLE_MOTOR      (1)
// Disable the motor constant
#define DISABLE_MOTOR     (0)

// Define high level for pulse
#define PULSE_HIGH        (1)
// Define low level for pulse
#define PULSE_LOW         (0)

// Define forward motor's direction
#define DIR_FORWARD       (true)
// Define reverse motor's direction
#define DIR_REVERSE       (false)

class Motor
{
public:
    Motor(size_t ena_pin, size_t dir_pin, size_t pul_pin):
        _ena_pin(ena_pin), _dir_pin(dir_pin),
        _pul_pin(pul_pin), _direction(DIR_FORWARD) {};

    void enable();
    void disable();
    void step();

    void forward_dir();
    void reverse_dir();
    void inverse_dir();

private:
    void set_dir();

    bool _direction;
    size_t _ena_pin;
    size_t _dir_pin;
    size_t _pul_pin;
};

#endif
