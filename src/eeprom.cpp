#include <Arduino.h>
#include <EEPROM.h>

#define POS_CURR_EEPROM_ADDR 0
#define POS_OPEN_EEPROM_ADDR 4
#define POS_CLOSE_EEPROM_ADDR 8

long EEPROMReadlong(long address) {
    Serial.println("read_eeprom");
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void EEPROMUpdatelong(int address, long value) {
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

long getCurrentPosition_eeprom()
{
    return EEPROMReadlong(POS_CURR_EEPROM_ADDR);
}

void setCurrentPosition_eeprom(long pos)
{
    EEPROMUpdatelong(POS_CURR_EEPROM_ADDR, pos);
}

long getOpenPosition_eeprom()
{
    return EEPROMReadlong(POS_OPEN_EEPROM_ADDR);
}

void setOpenPosition_eeprom(long pos)
{
    EEPROMUpdatelong(POS_OPEN_EEPROM_ADDR, pos);
}

long getClosePosition_eeprom()
{
    return EEPROMReadlong(POS_CLOSE_EEPROM_ADDR);
}

void setClosePosition_eeprom(long pos)
{
    EEPROMUpdatelong(POS_CLOSE_EEPROM_ADDR, pos);
}

void reset_eeprom()
{
    for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
    }
}
