#include <LowPower.h>

#include "src/defines.h"
#include "src/command.h"
#include "src/controls.h"
#include "src/door.h"
#include "src/eeprom.h"
#include "src/led.h"
#include "src/mode.h"
#include "src/time.h"

unsigned long lastwake;

void setup()
{
    Serial.begin(9600);

    pinMode(LED_PIN, OUTPUT);

    led_setState(LED_PIN, mode_is(Mode::Set));

    attachInterrupt(digitalPinToInterrupt(HOME_PIN), wake, RISING);
    attachInterrupt(digitalPinToInterrupt(RUN_PIN), wake, RISING);

    time_setup();
    ctrl_setup();
    door_setup(
        eeprom_getCurrPosition(),
        eeprom_getOpenPosition(),
        eeprom_getClosePosition());

    lastwake = millis();
}

void wake()
{
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();

    // debounce 200ms
    if (interrupt_time - last_interrupt_time > 200)
    {
        Serial.println("wake");
        lastwake = millis();
    }

    last_interrupt_time = interrupt_time;
}

void loop()
{
    ctrl_read();
    command_read();

    command_try("eeprom_reset", &command_eepromReset);
    command_try("time_set", &command_timeSet);
    command_try("date_set", &command_dateSet);
    command_try("scenario_for", &command_scenarioFor);

    if (ctrl_mode())
    {
        mode_toggle();
        led_setState(LED_PIN, mode_is(Mode::Set));
        delay(2000);
    }

    if (mode_is(Mode::Set))
    {
        if (ctrl_run())
        {
            door_toggle(ctrl_getDirection(), &eeprom_setCurrPosition);
            delay(2000);
        }

        if (ctrl_move())
        {
            door_move(ctrl_getDirection(), &ctrl_move, &ctrl_read, &eeprom_setCurrPosition);
            delay(2000);
        }

        if (ctrl_home())
        {
            door_set(ctrl_getDirection(), &eeprom_setDirectionPosition);
            led_blink(LED_PIN, 1);
            delay(2000);
        }
    }

    if (mode_is(Mode::Auto))
    {
        if (now() >= time_getActionTime(Action::Open, now()) && now() < time_getActionTime(Action::Close, now()))
        {
            door_toggle(Direction::Up, &eeprom_setCurrPosition);
        }

        if (now() >= time_getActionTime(Action::Close, now()) && now() > time_getActionTime(Action::Open, now()))
        {
            door_toggle(Direction::Down, &eeprom_setCurrPosition);
        }

        if (millis() >= lastwake + 10000) {
            delay(1000);
            LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
        }
    }

    delay(100);
}
