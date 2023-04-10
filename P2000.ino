#include <RTClib.h>

#include "src/defines.h"
#include "src/command.h"
#include "src/controls.h"
#include "src/door.h"
#include "src/eeprom.h"
#include "src/led.h"
#include "src/time.h"

bool mode_set = true;

void setup()
{
    Serial.begin(9600);

    pinMode(LED_PIN, OUTPUT);

    led_setState(LED_PIN, mode_set);

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
    command_read();

    command_try("eeprom_reset", &command_eepromReset);
    command_try("time_set", &command_timeSet);
    command_try("date_set", &command_dateSet);
    command_try("scenario_for", &command_scenarioFor);

    if (ctrl_set())
    {
        mode_set = !mode_set;
        led_setState(LED_PIN, mode_set);
        delay(1000);
    }

    if (mode_set)
    {
        if (ctrl_move())
        {
            door_move(ctrl_getDirection(), &ctrl_move, &ctrl_read, &eeprom_setCurrPosition);
        }

        if (ctrl_home())
        {
            door_set(ctrl_getDirection(), &eeprom_setDirectionPosition);
            led_blink(LED_PIN, 1);
        }
    }
    else
    {
        if (time_isSet())
        {
            return;
        }

        if (now() >= time_getActionTime(Action::Open, now()) && now() < time_getActionTime(Action::Close, now()))
        {
            door_toggle(Direction::Up, &eeprom_setCurrPosition);
        }

        if (now() >= time_getActionTime(Action::Close, now()) && now() > time_getActionTime(Action::Open, now()))
        {
            door_toggle(Direction::Down, &eeprom_setCurrPosition);
        }
    }

    delay(100);
}

time_t requestSync()
{
    return 0; // Time will be sent later in response to serial msg
}
