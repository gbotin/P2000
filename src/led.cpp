#include "led.h"

void led_blink(int pin, int times = 1, int freq = 50)
{
    int lastState = digitalRead(pin);

    if (lastState == HIGH)
    {
        digitalWrite(pin, LOW);
        delay(freq);
    }

    for (size_t i = 0; i < times; i++)
    {
        digitalWrite(pin, HIGH);
        delay(freq);
        digitalWrite(pin, LOW);
        delay(freq);
    }

    digitalWrite(pin, lastState);
}

void led_setState(int pin, bool state)
{
    digitalWrite(pin, state ? HIGH : LOW);
}
