#include "time.h"

const TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; // Central European Summer Time
const TimeChangeRule CET = {"CET", Last, Sun, Oct, 3, 60};    // Central European Standard Time
Timezone CE(CEST, CET);

Dusk2Dawn location(LAT, LNG, TMZ);

bool time_isSet()
{
    return now() < TIME_SET_TESTTIME;
}

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

time_t time_getCycleTime(SunCycle cycle, time_t time)
{
    int cycle_time;
    if (cycle == SunCycle::Sunrise)
    {
        cycle_time = location.sunrise(year(time), month(time), day(time), CE.locIsDST(time));
    }

    if (cycle == SunCycle::Sunset)
    {
        cycle_time = location.sunset(year(time), month(time), day(time), CE.locIsDST(time));
    }

    int h_cycle_time = cycle_time / 60;
    int m_cycle_time = cycle_time - (60 * h_cycle_time);

    DateTime cycle_dt(year(time), month(time), day(time), h_cycle_time, m_cycle_time, 0);
    return cycle_dt.unixtime();
}

time_t time_getActionTime(Action action, time_t time)
{
    DateTime action_dt;
    DateTime action_min_dt(year(time), month(time), day(time));

    if (action == Action::Open)
    {
        action_dt = DateTime(time_getCycleTime(SunCycle::Sunrise, time));
        action_dt = action_dt + TimeSpan(S_DELTA_OPEN);
        action_min_dt = action_min_dt + TimeSpan(S_MIN_OPEN);
    }

    if (action == Action::Close)
    {
        action_dt = DateTime(time_getCycleTime(SunCycle::Sunset, time));
        action_dt = action_dt + TimeSpan(S_DELTA_CLOSE);
        action_min_dt = action_min_dt + TimeSpan(S_MIN_CLOSE);
    };

    DateTime real_action_dt = action_dt > action_min_dt ? action_dt : action_min_dt;
    return real_action_dt.unixtime();
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
    Serial.println("| Sunrise | " + time_getTimeFormat(time_getCycleTime(SunCycle::Sunrise, time)) + " |");
    Serial.println("| Sunset  | " + time_getTimeFormat(time_getCycleTime(SunCycle::Sunset, time)) + " |");
    Serial.println("| Open    | " + time_getTimeFormat(time_getActionTime(Action::Open, time)) + " |");
    Serial.println("| Close   | " + time_getTimeFormat(time_getActionTime(Action::Close, time)) + " |");
    Serial.println(".--------------------.");
    Serial.println("");
    Serial.println("---");
}
