#include "controls.h"

Button RunButton(RUN_PIN);
Button HomeButton(HOME_PIN);

// Home and Run buttons pressed for 1s
bool ctrl_mode()
{
    return RunButton.pressedFor(1000) && HomeButton.pressedFor(1000);
}

// Home button pressed for 1s and run button was not pressed last 0,5s
bool ctrl_home()
{
    return HomeButton.pressedFor(1000) && RunButton.releasedFor(500);
}

// Run button pressed for 1s and home button was not pressed last 0,5s
bool ctrl_move()
{
    return RunButton.pressedFor(1000) && HomeButton.releasedFor(500);
}

// Run button was pressed
bool ctrl_run()
{
    return RunButton.wasReleased();
}

void ctrl_read()
{
    HomeButton.read();
    RunButton.read();
}

void ctrl_setup()
{
    pinMode(RUN_PIN, INPUT_PULLUP);
    pinMode(HOME_PIN, INPUT_PULLUP);
    pinMode(DIRECTION_PIN, INPUT);

    HomeButton.begin();
    RunButton.begin();
}

bool ctrl_isDirectionUp()
{
    return !digitalRead(DIRECTION_PIN);
}

bool ctrl_isDirectionDown()
{
    return digitalRead(DIRECTION_PIN);
}

Direction ctrl_getDirection()
{
    return ctrl_isDirectionUp() ? Direction::Up : Direction::Down;
}
