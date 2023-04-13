#pragma once

#include "defines.h"

void led_blink(int pin, int times = 1, int freq = 50);
void led_setState(int pin, bool state);
