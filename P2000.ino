#include <RTClib.h>

#include "src/defines.h"
#include "src/command.h"
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
    command_read();

    command_try("eeprom_reset", &command_eepromReset);
    command_try("time_set", &command_timeSet);
    command_try("date_set", &command_dateSet);
    command_try("scenario_for", &command_scenarioFor);

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

time_t requestSync()
{
    return 0; // Time will be sent later in response to serial msg
}
