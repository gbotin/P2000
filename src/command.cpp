#include "command.h"

String command;

boolean command_read()
{
    if (!Serial.available())
        return false;

    command = Serial.readString();
    command.trim();

    return true;
}

void command_try(String name, void (*callback)(String command))
{
    if (!command.startsWith(name))
        return;

    Serial.println("Running command " + name);
    callback(command);

    command = "";
}

void command_eepromReset(String command)
{
    eeprom_reset();
    led_blink(LED_PIN, 2, 500);
}

void command_timeSet(String command)
{
    String arg = command.substring(command.indexOf(' ') + 1, command.length());
    time_setSystemTime(arg.toInt());
}

void command_dateSet(String command)
{
    int day, month, year;
    String arg = command.substring(command.indexOf(' ') + 1, command.length());
    sscanf(arg.c_str(), "%2d/%2d/%4d", &day, &month, &year);

    DateTime date(year, month, day);
    time_setSystemTime(date.unixtime());
}

void command_scenarioFor(String command)
{
    int dayArg = day(), monthArg = month(), yearArg = year();

    String arg = command.substring(command.indexOf(' ') + 1, command.length());
    if (!arg.equals("today"))
    {
        sscanf(arg.c_str(), "%2d/%2d/%4d", &dayArg, &monthArg, &yearArg);
    }

    DateTime date(yearArg, monthArg, dayArg);
    time_printScenario(date.unixtime());
}
