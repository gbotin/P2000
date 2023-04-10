#include "mode.h"

Mode currentMode = Mode::Set;

void mode_set(Mode mode)
{
    currentMode = mode;
}

void mode_toggle()
{
    currentMode = currentMode == Mode::Set ? Mode::Auto : Mode::Set;
}

bool mode_is(Mode mode)
{
    return currentMode == mode;
}
