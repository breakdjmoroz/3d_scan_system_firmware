#ifndef MOTORS_API
#define MOTORS_API

class Motor
{
public:
    Motor(size_t ena_pin, size_t dir_pin, size_t pul_pin):
        ena_pin(ena_pin), dir_pin(dir_pin), pul_pin(pul_pin){};

    void step();
    void set_dir(bool direction);

private:
    size_t ena_pin;
    size_t dir_pin;
    size_t pul_pin;
};

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

#endif
