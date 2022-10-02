#include "defines.h"
#include <Arduino.h>
#include <EEPROM.h>

long eeprom_readLong(long address)
{
    Serial.println("read_eeprom");
    long four = EEPROM.read(address);
    long three = EEPROM.read(address + 1);
    long two = EEPROM.read(address + 2);
    long one = EEPROM.read(address + 3);

    return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void eeprom_updateLong(int address, long value)
{
    Serial.println("write_eeprom");
    byte four = (value & 0xFF);
    byte three = ((value >> 8) & 0xFF);
    byte two = ((value >> 16) & 0xFF);
    byte one = ((value >> 24) & 0xFF);

    EEPROM.update(address, four);
    EEPROM.update(address + 1, three);
    EEPROM.update(address + 2, two);
    EEPROM.update(address + 3, one);
}

long eeprom_getCurrPosition()
{
    return eeprom_readLong(POS_CURR_EEPROM_ADDR);
}

void eeprom_setCurrPosition(long pos)
{
    eeprom_updateLong(POS_CURR_EEPROM_ADDR, pos);
}

long eeprom_getOpenPosition()
{
    return eeprom_readLong(POS_OPEN_EEPROM_ADDR);
}

void eeprom_setOpenPosition(long pos)
{
    eeprom_updateLong(POS_OPEN_EEPROM_ADDR, pos);
}

long eeprom_getClosePosition()
{
    return eeprom_readLong(POS_CLOSE_EEPROM_ADDR);
}

void eeprom_setClosePosition(long pos)
{
    eeprom_updateLong(POS_CLOSE_EEPROM_ADDR, pos);
}

void eeprom_setDirectionPosition(long pos, Direction direction)
{
    int addr = direction == Direction::Up ? POS_OPEN_EEPROM_ADDR : POS_CLOSE_EEPROM_ADDR;
    eeprom_updateLong(addr, pos);
}

void eeprom_reset()
{
    for (int i = 0; i < EEPROM.length(); i++)
    {
        EEPROM.write(i, 0);
    }
}
