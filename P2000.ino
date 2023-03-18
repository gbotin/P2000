// Utils :
// Send current time via Serial : date +T%s\n > /dev/tty.usbserial-141410

// TODO :
// - update stepper lib to accelstepper
// - buy rtc module
// - homing step motor - buy homing switch

// Infos :
// Stepper : 28byj-48

// http://suncalc.net/#/50.6244,2.6875,9/2023.12.01/15:34

// FIFO readings
// https://github.com/rlogiacco/CircularBuffer

// https://github.com/PaulStoffregen/Time
#include <Time.h>
#include <TimeLib.h>

// http://github.com/JChristensen/Timezone
#include <Timezone.h>

// https://github.com/dmkishi/Dusk2Dawn
// #include <Dusk2Dawn.h>

// https://github.com/adafruit/RTClib
// #include <RTClib.h>

// #define SENSOR_PIN A0

#include "src/defines.h"
#include "src/controls.h"
#include "src/door.h"
#include "src/eeprom.h"
#include "src/led.h"

// #define LAT 50.62448840748527
// #define LNG 2.6875116810701773
// #define TMZ 1

const TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; // Central European Summer Time
const TimeChangeRule CET = {"CET", Last, Sun, Oct, 3, 60};    // Central European Standard Time
Timezone CE(CEST, CET);

// Dusk2Dawn home(LAT, LNG, TMZ);

#define S_DELTA_OPEN  3600 // 1H
#define S_DELTA_CLOSE 3600 // 1H

#define S_MIN_OPEN 28800 // 1970-01-01 9:00:00
#define S_MIN_CLOSE 68400 // 1970-01-01 19:00:00

#include <AccelStepper.h>
#define MOTOR_MODE AccelStepper::HALF4WIRE
AccelStepper Motor(
    MOTOR_MODE,
    STEPPER_PIN_1,
    STEPPER_PIN_3,
    STEPPER_PIN_2,
    STEPPER_PIN_4);

void setup()
{
    Serial.begin(9600);

    pinMode(LED_PIN, OUTPUT);

    pinMode(STEPPER_PIN_1, OUTPUT);
    pinMode(STEPPER_PIN_2, OUTPUT);
    pinMode(STEPPER_PIN_3, OUTPUT);
    pinMode(STEPPER_PIN_4, OUTPUT);

    //   setSyncProvider(requestSync); // Set function to call when sync required

    ctrl_setup();
    door_setup(
        eeprom_getCurrPosition(),
        eeprom_getOpenPosition(),
        eeprom_getClosePosition());
}

void loop()
{
    ctrl_read();

    // if (Serial.available())
    // {
    //     String command = Serial.readString();
    //     command.trim();

    //     if (command.startsWith("eeprom_reset"))
    //     {
    //         Serial.println("Resetting EEPROM...");

    //         eeprom_reset();

    //         Serial.println("Done.");
    //         led_blink(LED_PIN, 2, 500);
    //     }

    //     if (command.startsWith("time_set"))
    //     {
    //         String arg = command.substring(command.indexOf(' ') + 1, command.length());

    //         unsigned long pctime = arg.toInt();
    //         unsigned long localtime = CE.toLocal(pctime);

    //         setTime(localtime);

    //         // Serial.println("Time set to :" + getFormattedCurrentTime());
    //         Serial.println("Time set to : ");
    //         digitalClockDisplay();
    //         Serial.println("---");
    //     }

    //     if (command.startsWith("time_print"))
    //     {
    //         Serial.println(getFormattedCurrentTime());

    //         int sunrise = home.sunrise(year(), month(), day(), CE.locIsDST(now()));
    //         int sunset = home.sunset(year(), month(), day(), CE.locIsDST(now()));

    //         char time[6];
    //         Dusk2Dawn::min2str(time, sunrise);
    //         Serial.println("sunrise " + String(time));
    //         Dusk2Dawn::min2str(time, sunset);
    //         Serial.println("sunset " + String(time));
    //     }

    //     if (command.startsWith("time_hours"))
    //     {
    //         getCloseTime();
    //     }
    // }

    // working
    // showLuminosity();


    if (ctrl_move())
    {
        Serial.println("move from P2000");
        // door_move(ctrl_getDirection(), &ctrl_move, &ctrl_read, &eeprom_setCurrPosition);

        int i = ctrl_getDirection() == Direction::Down ? -1 : 1;


        Motor.setMaxSpeed(2000);
        Motor.setAcceleration(100);
        // long lastPosition = Motor.currentPosition();
        Motor.setCurrentPosition(0);
        Motor.setSpeed(1000);

        while (ctrl_move())
        {
            Serial.println("inside while");
            // Motor.move(Motor.currentPosition() + i * 1000);
            Motor.move(i * 10);
            Motor.run();

            // ctrl_read();
        }

        Serial.println("leave while");
    }

    // if (ctrl_toggle())
    // {
    //     door_toggle(ctrl_getDirection(), &eeprom_setCurrPosition);
    // }

    // if (ctrl_set())
    // {
    //     door_set(ctrl_getDirection(), &eeprom_setDirectionPosition);
    // }

    // delay(100);
}

// time_t getCloseTime()
// {
//     int close_time = home.sunset(year(), month(), day(), CE.locIsDST(now()));
//     int h_close_time = close_time / 60;
//     int m_close_time = close_time - (60 * h_close_time);

//     // int s_close_time = close_time * 60;

//     Serial.println("---");
//     Serial.println("Current Time : ");
//     digitalClockDisplay();

//     DateTime close_dt(year(), month(), day(), h_close_time, m_close_time, 0);
//     // close_dt = close_dt + TimeSpan(3600);

//     DateTime close_min_dt(year(), month(), day(), 0, 0, 0);
//     // close_min_dt = close_min_dt + TimeSpan(68400);

//     // DateTime real_close_dt = close_dt > close_min_dt ? close_dt : close_min_dt;

//     Serial.println(close_dt.toString("YYYY MM DD hh:mm:ss"));
//     Serial.println(close_min_dt.toString("YYYY MM DD hh:mm:ss"));
//     Serial.println("---");

//     // int test = year() + month() + day() + close_time;
//     //  + " " + test

//     // time_t delta_close_time = close_time + DELTA_CLOSE;
//     // time_t min_close_time = year() + month() + day() + MIN_TIME_CLOSE;


//     // digitalClockDisplayTest(year() + month() + day() + close_time);
//     // digitalClockDisplay(delta_close_time);
//     // digitalClockDisplay(min_close_time);
// }

// time_t getOpenTime()
// {

// }

// bool time_shouldClose()
// {
//     int sunset = home.sunset(year(), month(), day(), CE.locIsDST(now()));

//     !door_isClose();

//     return false;
// }

// bool time_shouldOpen()
// {
//     int sunrise = home.sunrise(year(), month(), day(), CE.locIsDST(now()));

//     !door_isOpen();

//     return false;
// }


// working - not needed
// void showLuminosity()
// {
//     int value = analogRead(SENSOR_PIN);
//   Serial.println("Current luminosity is " + value);

//   if (value > 500)
//   {
//     digitalWrite(LED_PIN, HIGH);
//   }
//   else
//   {
//     digitalWrite(LED_PIN, LOW);
//   }
// }

time_t requestSync()
{
    return 0; // Time will be sent later in response to serial msg
}

String getFormattedTime(time_t t)
{
    char buffer[50];
    sprintf(buffer, "%.2d:%.2d:%.2d", hour(t), minute(t), second(t));
    return String(buffer);
}

String getFormattedCurrentTime()
{
    char buffer[50];
    sprintf(buffer, "%.2d:%.2d:%.2d", hour(), minute(), second());
    return String(buffer);
}

void printDigits(int digits)
{
    // utility function for digital clock display: prints preceding colon and leading 0
    Serial.print(":");
    if (digits < 10)
        Serial.print('0');
    Serial.print(digits);
}

void digitalClockDisplayTest(time_t t)
{
    // digital clock display of the time
    Serial.print(hour(t));
    printDigits(minute(t));
    printDigits(second(t));
    Serial.print(" ");
    Serial.print(day(t));
    Serial.print(" ");
    Serial.print(month(t));
    Serial.print(" ");
    Serial.print(year(t));
    Serial.println();
}


void digitalClockDisplay()
{
    // digital clock display of the time
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.print(" ");
    Serial.print(day());
    Serial.print(" ");
    Serial.print(month());
    Serial.print(" ");
    Serial.print(year());
    Serial.println();
}
