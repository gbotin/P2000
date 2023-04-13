#pragma once

#include "defines.h"

// https://github.com/adafruit/RTClib
#include <RTClib.h>

// https://github.com/PaulStoffregen/Time
#include <TimeLib.h>

// http://github.com/JChristensen/Timezone
#include <Timezone.h>

// https://github.com/dmkishi/Dusk2Dawn
#include <Dusk2Dawn.h>

String time_getTimeFormat(time_t t);
String time_getDateFormat(time_t t);
String time_getDateTimeFormat(time_t t);
time_t time_getCycleTime(SunCycle cycle, time_t time);
time_t time_getActionTime(Action action, time_t time);
time_t time_getRTCTime();
void time_setSystemTime(time_t time, bool local = true);
void time_printScenario(time_t time);
void time_setup();
