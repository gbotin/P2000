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
#include <Dusk2Dawn.h>

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

            Serial.println("Time set to " + getFormattedCurrentTime());
            digitalClockDisplay();
        }

        if (command.startsWith("time_print"))
        {
            Serial.println(getFormattedCurrentTime());

            int sunrise = home.sunrise(year(), month(), day(), CE.locIsDST(now()));
            int sunset = home.sunset(year(), month(), day(), CE.locIsDST(now()));

            char time[6];
            Dusk2Dawn::min2str(time, sunrise);
            Serial.println("sunrise " + String(time));
            Dusk2Dawn::min2str(time, sunset);
            Serial.println("sunset " + String(time));
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
