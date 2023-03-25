#include "defines.h"
#include <Arduino.h>
#include <EEPROM.h>

long eeprom_readLong(long address);
void eeprom_updateLong(int address, long value);

long eeprom_getCurrPosition();
void eeprom_setCurrPosition(long pos);

long eeprom_getOpenPosition();
void eeprom_setOpenPosition(long pos);

long eeprom_getClosePosition();
void eeprom_setClosePosition(long pos);

void eeprom_setDirectionPosition(long pos, Direction direction);

void eeprom_reset();
