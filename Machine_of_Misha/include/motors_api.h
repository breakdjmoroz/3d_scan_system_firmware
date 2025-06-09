#ifndef MOTORS_API
#define MOTORS_API

// Microseconds in second
#define USEC_IN_SEC       (1e6)

// Frequency of the motor's motion, Hz
#define MAX_FREQUENCY     (200000)

class Motor
{
public:
    Motor(size_t ena_pin, size_t dir_pin, size_t pul_pin);

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
