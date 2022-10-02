// LEDS
#define LED_PIN 2

// CONTROLS
#define DIRECTION_PIN 7
#define RUN_PIN 9
#define HOME_PIN 10

// STEPPER
#define STEPPER_PIN_1 3
#define STEPPER_PIN_2 4
#define STEPPER_PIN_3 5
#define STEPPER_PIN_4 6

// upwards max speed at half step + 300g = 840 grand MAX
// upwards max sans charge at half step 1700

// #define MOTOR_SPEED 600.0
// #define MOTOR_ACCEL 100.0
// #define MOTOR_MODE  AccelStepper::FULL4WIRE
#define MOTOR_SPEED 1000.0 // 1500.0
#define MOTOR_ACCEL 500.0

// EEPROM
#define POS_CURR_EEPROM_ADDR 0
#define POS_OPEN_EEPROM_ADDR 4
#define POS_CLOSE_EEPROM_ADDR 8

enum Direction
{
    Up,
    Down
};

struct Position
{
    long curr;
    long open;
    long close;

    Position (long curr = 0, long open = 0, long close = 0)
    {
        this->curr = curr;
        this->curr = open;
        this->curr = close;
    }
};
