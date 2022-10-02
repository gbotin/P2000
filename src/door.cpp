#include <Arduino.h>

// https://www.pjrc.com/teensy/td_libs_AccelStepper.html
#include <AccelStepper.h>

#include "defines.h"

#define MOTOR_MODE AccelStepper::HALF4WIRE

AccelStepper Motor(
    MOTOR_MODE,
    STEPPER_PIN_1,
    STEPPER_PIN_3,
    STEPPER_PIN_2,
    STEPPER_PIN_4);

// long currPosition = 0;
// long openPosition = 0;
// long closePosition = 0;

Position position;

void door_setup(Position pos)
{
    pinMode(STEPPER_PIN_1, OUTPUT);
    pinMode(STEPPER_PIN_2, OUTPUT);
    pinMode(STEPPER_PIN_3, OUTPUT);
    pinMode(STEPPER_PIN_4, OUTPUT);

    position = pos;

    Motor.setMaxSpeed(MOTOR_SPEED);
    Motor.setAcceleration(MOTOR_ACCEL);
    Motor.setCurrentPosition(position.curr);
}

long door_getPosition()
{
    return position.curr;
}

void door_set(Direction direction, void (*save)(long pos, Direction dir))
{
    if (direction == Direction::Down && position.close != Motor.currentPosition())
    {
        Serial.println("door_set close");
        position.close = Motor.currentPosition();
        save(position.close, direction);
    }

    if (direction == Direction::Up && position.open != Motor.currentPosition())
    {
        Serial.println("door_set open");
        position.open = Motor.currentPosition();
        save(position.open, direction);
    }
}

void door_move(Direction direction, bool (*cond)(), void (*after)())
{
    Serial.println("door_move");
    int i = direction == Direction::Down ? -1 : 1;

    long lastPosition = Motor.currentPosition();
    Motor.setCurrentPosition(0);

    while (cond())
    {
        Motor.move(Motor.currentPosition() + i);
        Motor.run();

        after();
    }

    position.curr = lastPosition + Motor.currentPosition();
    Motor.setCurrentPosition(position.curr);
}

void door_toggle(Direction direction)
{
    if (direction == Direction::Down)
    {
        Serial.println("door_toggle close");
        Motor.runToNewPosition(position.close);
    }

    if (direction == Direction::Up)
    {
        Serial.println("door_toggle open");
        Motor.runToNewPosition(position.open);
    }
}
