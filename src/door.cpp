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

long currPosition = 0;
long openPosition = 0;
long closePosition = 0;

bool door_shouldClose(Direction direction)
{
    return direction == Direction::Down && currPosition != closePosition;
}

bool door_shouldOpen(Direction direction)
{
    return direction == Direction::Up && currPosition != openPosition;
}

long door_getPosition()
{
    return currPosition;
}

void door_setup(long curr = 0, long open = 0, long close = 0)
{
    pinMode(STEPPER_PIN_1, OUTPUT);
    pinMode(STEPPER_PIN_2, OUTPUT);
    pinMode(STEPPER_PIN_3, OUTPUT);
    pinMode(STEPPER_PIN_4, OUTPUT);

    currPosition = curr;
    openPosition = open;
    closePosition = close;

    Motor.setMaxSpeed(MOTOR_SPEED);
    Motor.setAcceleration(MOTOR_ACCEL);
    Motor.setCurrentPosition(currPosition);
}

void door_set(Direction direction, void (*save)(long pos, Direction dir))
{
    if (door_shouldClose(direction))
    {
        Serial.println("door_set close");
        closePosition = Motor.currentPosition();
        save(closePosition, direction);
    }

    if (door_shouldOpen(direction))
    {
        Serial.println("door_set open");
        openPosition = Motor.currentPosition();
        save(openPosition, direction);
    }
}

void door_move(Direction direction, bool (*cond)(), void (*after)(), void (*save)(long pos))
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

    currPosition = lastPosition + Motor.currentPosition();
    Motor.setCurrentPosition(currPosition);
    save(currPosition);
}

void door_toggle(Direction direction, void (*save)(long pos))
{
    if (!door_shouldOpen(direction) && !door_shouldClose(direction))
    {
        return;
    }

    if (direction == Direction::Down)
    {
        Serial.println("door_toggle close");
        Motor.runToNewPosition(closePosition);
    }

    if (direction == Direction::Up)
    {
        Serial.println("door_toggle open");
        Motor.runToNewPosition(openPosition);
    }

    currPosition = Motor.currentPosition();
    save(currPosition);
}
