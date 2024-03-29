#pragma once

#include "defines.h"

// https://www.pjrc.com/teensy/td_libs_AccelStepper.html
#include <AccelStepper.h>

void door_powerDown();
void door_setup(long curr = 0, long open = 0, long close = 0);
long door_getPosition();
void door_set(Direction direction, void (*save)(long pos, Direction dir));
void door_move(Direction direction, bool (*cond)(), void (*after)(), void (*save)(long pos));
void door_toggle(Direction direction, void (*save)(long pos));
bool door_isClose();
bool door_isOpen();
bool door_shouldClose(Direction direction);
bool door_shouldOpen(Direction direction);
