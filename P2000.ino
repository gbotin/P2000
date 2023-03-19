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

// https://github.com/adafruit/RTClib
#include <RTClib.h>

// #define SENSOR_PIN A0

#include "src/defines.h"
#include "src/controls.h"
#include "src/door.h"
#include "src/eeprom.h"
#include "src/led.h"
#include "src/time.h"

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
            time_setSystemTime(arg.toInt());
        }

        if (command.startsWith("date_set"))
        {
            int day, month, year;
            String arg = command.substring(command.indexOf(' ') + 1, command.length());
            sscanf(arg.c_str(), "%2d/%2d/%4d", &day, &month, &year);

            DateTime date(year, month, day);
            time_setSystemTime(date.unixtime());
        }

        if (command.startsWith("scenario_for"))
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
