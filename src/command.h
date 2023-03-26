#pragma once

#include "defines.h"
#include "eeprom.h"
#include "led.h"
#include "time.h"

boolean command_read();
void command_try(String name, void (*callback)(String command));

void command_eepromReset(String command);
void command_timeSet(String command);
void command_dateSet(String command);
void command_scenarioFor(String command);
