#pragma once

#include "defines.h"

// https://github.com/JChristensen/JC_Button
#include <JC_Button.h>

bool ctrl_set();
bool ctrl_home();
bool ctrl_move();
void ctrl_read();
void ctrl_setup();
bool ctrl_isDirectionUp();
bool ctrl_isDirectionDown();
Direction ctrl_getDirection();
