#include "defines.h"

// https://github.com/adafruit/RTClib
#include <RTClib.h>

// https://github.com/PaulStoffregen/Time
#include <Time.h>
#include <TimeLib.h>

// http://github.com/JChristensen/Timezone
#include <Timezone.h>

// https://github.com/dmkishi/Dusk2Dawn
#include <Dusk2Dawn.h>

const TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; // Central European Summer Time
const TimeChangeRule CET = {"CET", Last, Sun, Oct, 3, 60};    // Central European Standard Time
Timezone CE(CEST, CET);

Dusk2Dawn location(LAT, LNG, TMZ);

String time_getTimeFormat(time_t t)
{
    char buffer[50];
    sprintf(buffer, "%.2d:%.2d:%.2d", hour(t), minute(t), second(t));
    return String(buffer);
}

String time_getDateFormat(time_t t)
{
    char buffer[50];
    sprintf(buffer, "%.2d/%.2d/%.4d", day(t), month(t), year(t));
    return String(buffer);
}

String time_getDateTimeFormat(time_t t)
{
    char buffer[50];
    sprintf(buffer, "%.2d/%.2d/%.4d %.2d:%.2d:%.2d", day(t), month(t), year(t), hour(t), minute(t), second(t));
    return String(buffer);
}

time_t time_getSunset(time_t time)
{
    int sunset_time = location.sunset(year(time), month(time), day(time), CE.locIsDST(time));
    int h_sunset_time = sunset_time / 60;
    int m_sunset_time = sunset_time - (60 * h_sunset_time);

    DateTime sunset_dt(year(time), month(time), day(time), h_sunset_time, m_sunset_time, 0);
    return sunset_dt.unixtime();
}

time_t time_getSunrise(time_t time)
{
    int sunrise_time = location.sunrise(year(time), month(time), day(time), CE.locIsDST(time));
    int h_sunrise_time = sunrise_time / 60;
    int m_sunrise_time = sunrise_time - (60 * h_sunrise_time);

    DateTime sunrise_dt(year(time), month(time), day(time), h_sunrise_time, m_sunrise_time, 0);
    return sunrise_dt.unixtime();
}

time_t time_getCloseTime(time_t time)
{
    DateTime sunset_dt = DateTime(time_getSunset(time));
    DateTime close_dt = sunset_dt + TimeSpan(S_DELTA_CLOSE);

    DateTime close_min_dt(year(time), month(time), day(time));
    close_min_dt = close_min_dt + TimeSpan(S_MIN_CLOSE);

    DateTime read_close_dt = close_dt > close_min_dt ? close_dt : close_min_dt;
    return read_close_dt.unixtime();
}

time_t time_getOpenTime(time_t time)
{
    DateTime sunrise_dt = DateTime(time_getSunrise(time));
    DateTime open_dt = sunrise_dt + TimeSpan(S_DELTA_OPEN);

    DateTime open_min_dt(year(time), month(time), day(time));
    open_min_dt = open_min_dt + TimeSpan(S_MIN_OPEN);

    DateTime real_open_dt = open_dt > open_min_dt ? open_dt : open_min_dt;
    return real_open_dt.unixtime();
}

void time_setSystemTime(time_t time, bool local = true)
{
    setTime(local ? CE.toLocal(time) : time);

    Serial.println("Time set to : " + time_getDateTimeFormat(now()));
    Serial.println("---");
}

void time_printScenario(time_t time)
{
    Serial.println(".--------------------.");
    Serial.println("|     " + time_getDateFormat(time) + "     |");
    Serial.println(".--------------------.");
    Serial.println("| Sunrise | " + time_getTimeFormat(time_getSunrise(time)) + " |");
    Serial.println("| Sunset  | " + time_getTimeFormat(time_getSunset(time)) + " |");
    Serial.println("| Open    | " + time_getTimeFormat(time_getOpenTime(time)) + " |");
    Serial.println("| Close   | " + time_getTimeFormat(time_getCloseTime(time)) + " |");
    Serial.println(".--------------------.");
    Serial.println("");
    Serial.println("---");
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
