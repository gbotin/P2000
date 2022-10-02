// Utils :
// Send current time via Serial : date +T%s\n > /dev/tty.usbserial-141410

// TODO :
// - update stepper lib to accelstepper
// - buy rtc module
// - homing step motor - buy homing switch

// Infos :
// Stepper : 28byj-48

// FIFO readings
// https://github.com/rlogiacco/CircularBuffer

// https://github.com/PaulStoffregen/Time
#include <Time.h>
#include <TimeLib.h>

// http://github.com/JChristensen/Timezone
#include <Timezone.h>

// #define SENSOR_PIN A0

// Config
// #define TIME_HEADER "T" // Header tag for serial time sync message

#include "src/defines.h"

#include "src/controls.h"
#include "src/door.h"
#include "src/eeprom.h"
#include "src/led.h"

// struct BusinessHours
// {
//   uint8_t opening;
//   uint8_t closing;
// };

// const TimeChangeRule CEST = { "CEST", Last, Sun, Mar, 2, 120 }; // Central European Summer Time
// const TimeChangeRule CET = { "CET", Last, Sun, Oct, 3, 60 };    // Central European Standard Time
// Timezone CE(CEST, CET);

// const BusinessHours dst_hours = { 7, 22 };
// const BusinessHours st_hours = { 7, 22 };

// bool isDST = false; // Daylight Saving Time (nb. Summer hours)

// short int state = DOOR_STATE_UNDEF;
// long homing = -1;

void setup()
{
    Serial.begin(9600);

    pinMode(LED_PIN, OUTPUT);

    //   setSyncProvider(requestSync); // Set function to call when sync required
    Position position = Position(
        eeprom_getCurrPosition(),
        eeprom_getOpenPosition(),
        eeprom_getClosePosition()
    );

    door_setup(position);
    ctrl_setup();
}

void loop()
{
    ctrl_read();

    //   Serial.println("---");
    //   Serial.println("currpos : " + String(Motor.currentPosition()) + " / " + String(currentPosition));
    //   Serial.println("openpos : " + String(openPosition));
    //   Serial.println("closepos : " + String(closePosition));
    //   Serial.println(isDirectionDown() ? "down" : "up");
    //   Serial.println("---");

    if (Serial.available())
    {
        // processSyncMessage();

        String command = Serial.readString();
        command.trim();

        if (command == "eeprom_reset")
        {
            Serial.println("Resetting EEPROM...");

            eeprom_reset();

            Serial.println("Done.");
            led_blink(LED_PIN, 2, 500);
        }
    }

    // printCurrentTime();

    // working
    // showLuminosity();

    if (ctrl_move())
    {
        door_move(ctrl_getDirection(), &ctrl_move, &ctrl_read);
        eeprom_setCurrPosition(door_getPosition());
    }

    if (ctrl_toggle())
    {
        door_toggle(ctrl_getDirection());
    }

    if (ctrl_set())
    {
        door_set(ctrl_getDirection(), &eeprom_setDirectionPosition);
    }

    delay(100);
}

// bool shouldOpen() {
//   TODO
// }

// bool shouldClose() {
//   TODO
// }

// void printCurrentTime()
// {
//   char buffer[50];
//   sprintf(buffer, "%.2d:%.2d:%.2d", hour(), minute(), second());
//   Serial.println(buffer);
// }

// void processSyncMessage()
// {
//   unsigned long pctime;
//   unsigned long localtime;

//   if (Serial.find(TIME_HEADER))
//   {
//     pctime = Serial.parseInt();
//     localtime = CE.toLocal(pctime);

//     isDST = CE.locIsDST(localtime);
//     setTime(localtime);
//   }
// }

// BusinessHours getBusinessHours ()
// {
//   return isDST ? dst_hours : st_hours;
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
