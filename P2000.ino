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
// #include <Time.h>
// #include <TimeLib.h>

// http://github.com/JChristensen/Timezone
#include <Timezone.h>

// https://github.com/dmkishi/Dusk2Dawn
#include <Dusk2Dawn.h>

// https://github.com/adafruit/RTClib
#include <RTClib.h>

// #define SENSOR_PIN A0

#include "src/defines.h"
#include "src/controls.h"
#include "src/door.h"
#include "src/eeprom.h"
#include "src/led.h"

#define LAT 50.62448840748527
#define LNG 2.6875116810701773
#define TMZ 1

const TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; // Central European Summer Time
const TimeChangeRule CET = {"CET", Last, Sun, Oct, 3, 60};    // Central European Standard Time
Timezone CE(CEST, CET);

Dusk2Dawn home(LAT, LNG, TMZ);

#define S_DELTA_OPEN  3600 // 1H
#define S_DELTA_CLOSE 3600 // 1H

#define S_MIN_OPEN 28800 // 1970-01-01 9:00:00
#define S_MIN_CLOSE 68400 // 1970-01-01 19:00:00

void setup()
{
    Serial.begin(9600);

    pinMode(LED_PIN, OUTPUT);

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

    if (Serial.available())
    {
        String command = Serial.readString();
        command.trim();

        if (command.startsWith("eeprom_reset"))
        {
            Serial.println("Resetting EEPROM...");

            eeprom_reset();

            Serial.println("Done.");
            led_blink(LED_PIN, 2, 500);
        }

        if (command.startsWith("time_set"))
        {
            String arg = command.substring(command.indexOf(' ') + 1, command.length());

            unsigned long pctime = arg.toInt();
            unsigned long localtime = CE.toLocal(pctime);

            setTime(localtime);

            Serial.println("Time set to : " + getFormattedCurrentDateTime());
            Serial.println("---");
        }

        if (command.startsWith("date_set"))
        {
            String arg = command.substring(command.indexOf(' ') + 1, command.length());

            int day;
            int month;
            int year;
            sscanf(arg.c_str(), "%2d/%2d/%4d", &day, &month, &year);

            DateTime date(year, month, day, 0, 0, 0);

            unsigned long localtime = CE.toLocal(date.unixtime());
            setTime(localtime);

            Serial.println("Date set to : " + getFormattedCurrentDateTime());
            Serial.println("---");
        }

        if (command.startsWith("time_program"))
        {
            Serial.println("Current Time :");
            Serial.println(getFormattedCurrentDateTime());

            int sunrise = home.sunrise(year(), month(), day(), CE.locIsDST(now()));
            int sunset = home.sunset(year(), month(), day(), CE.locIsDST(now()));

            char time[6];
            Dusk2Dawn::min2str(time, sunrise);
            Serial.println("Sunrise at " + String(time));

            Dusk2Dawn::min2str(time, sunset);
            Serial.println("Sunset at " + String(time));

            Serial.println("Open at " + getFormattedTime(getOpenTime().unixtime()));
            Serial.println("Close at " + getFormattedTime(getCloseTime().unixtime()));

            Serial.println("---");
        }
    }

    // working
    // showLuminosity();


    if (ctrl_move())
    {
        door_move(ctrl_getDirection(), &ctrl_move, &ctrl_read, &eeprom_setCurrPosition);
    }

    if (ctrl_toggle())
    {
        door_toggle(ctrl_getDirection(), &eeprom_setCurrPosition);
    }

    if (ctrl_set())
    {
        door_set(ctrl_getDirection(), &eeprom_setDirectionPosition);
    }

    delay(100);
}

DateTime getCloseTime()
{
    int close_time = home.sunset(year(), month(), day(), CE.locIsDST(now()));
    int h_close_time = close_time / 60;
    int m_close_time = close_time - (60 * h_close_time);

    DateTime close_dt(year(), month(), day(), h_close_time, m_close_time, 0);
    close_dt = close_dt + TimeSpan(S_DELTA_CLOSE);

    DateTime close_min_dt(year(), month(), day(), 0, 0, 0);
    close_min_dt = close_min_dt + TimeSpan(S_MIN_CLOSE);

    return close_dt > close_min_dt ? close_dt : close_min_dt;
}

DateTime getOpenTime()
{
    int open_time = home.sunrise(year(), month(), day(), CE.locIsDST(now()));
    int h_open_time = open_time / 60;
    int m_open_time = open_time - (60 * h_open_time);

    DateTime open_dt(year(), month(), day(), h_open_time, m_open_time, 0);
    open_dt = open_dt + TimeSpan(S_DELTA_OPEN);

    DateTime open_min_dt(year(), month(), day(), 0, 0, 0);
    open_min_dt = open_min_dt + TimeSpan(S_MIN_OPEN);

    return open_dt > open_min_dt ? open_dt : open_min_dt;
}

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

String getFormattedDateTime(time_t t)
{
    char buffer[50];
    sprintf(buffer, "%.2d/%.2d/%.4d %.2d:%.2d:%.2d", day(t), month(t), year(t), hour(t), minute(t), second(t));
    return String(buffer);
}

String getFormattedCurrentTime()
{
    char buffer[50];
    sprintf(buffer, "%.2d:%.2d:%.2d", hour(), minute(), second());
    return String(buffer);
}

String getFormattedCurrentDateTime()
{
    char buffer[50];
    sprintf(buffer, "%.2d/%.2d/%.4d %.2d:%.2d:%.2d", day(), month(), year(), hour(), minute(), second());
    return String(buffer);
}
