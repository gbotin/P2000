// https://github.com/JChristensen/JC_Button
#include <JC_Button.h>

#define DIRECTION_PIN 7
#define RUN_PIN 9
#define HOME_PIN 10

Button RunButton  (RUN_PIN);
Button HomeButton (HOME_PIN);

bool btn_runHoming()
{
    return RunButton.pressedFor(1000) && HomeButton.pressedFor(1000);
}

// Home button pressed for 1s and run button was not pressed last 0,5s
bool btn_setHome()
{
    return HomeButton.pressedFor(1000) && RunButton.releasedFor(500);
}

// Run button pressed for 1s and home button was not pressed last 0,5s
bool btn_run()
{
    return RunButton.pressedFor(1000) && HomeButton.releasedFor(500);
}

void btn_read()
{
    HomeButton.read();
    RunButton.read();
}

void btn_setup()
{
    // pinMode(RUN_PIN, INPUT_PULLUP);
    // pinMode(HOME_PIN, INPUT_PULLUP);
    pinMode(DIRECTION_PIN, INPUT);

    HomeButton.begin();
    RunButton.begin();
}

bool isDirectionUp()
{
    return !digitalRead(DIRECTION_PIN);
}

bool isDirectionDown()
{
    return digitalRead(DIRECTION_PIN);
}
