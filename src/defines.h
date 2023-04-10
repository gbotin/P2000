#pragma once

#include <Arduino.h>

// LEDS
#define LED_PIN 2

// CONTROLS
#define DIRECTION_PIN 7
#define RUN_PIN 9
#define HOME_PIN 10

// LOCATION
#define LAT 50.62448840748527
#define LNG 2.6875116810701773
#define TMZ 1

// DOOR TIMING
#define TIME_SET_TESTTIME 946681200; // 2000-01-01 00:00:00 GMT+0

#define S_DELTA_OPEN 0     // 0H
#define S_DELTA_CLOSE 3600 // 1H

#define S_MIN_OPEN 32400  // 1970-01-01 09:00:00 GMT+0
#define S_MIN_CLOSE 68400 // 1970-01-01 19:00:00 GMT+0

// STEPPER
#define STEPPER_PIN_1 3
#define STEPPER_PIN_2 4
#define STEPPER_PIN_3 5
#define STEPPER_PIN_4 6

// upwards max speed at half step + 300g = 840 grand MAX
// upwards max sans charge at half step 1700

#define MOTOR_SPEED 1000.0 // 1500.0
#define MOTOR_ACCEL 500.0

// EEPROM
#define POS_CURR_EEPROM_ADDR 0
#define POS_OPEN_EEPROM_ADDR 4
#define POS_CLOSE_EEPROM_ADDR 8

enum Direction
{
    Up,
    Down,
};

enum SunCycle
{
    Sunrise,
    Sunset,
};

enum Action
{
    Open,
    Close,
};

enum Mode
{
    Auto,
    Set,
};
