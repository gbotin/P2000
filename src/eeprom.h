long EEPROMReadlong(long address);
void EEPROMUpdatelong(int address, long value);

long getCurrentPosition_eeprom();
void setCurrentPosition_eeprom(long pos);

long getOpenPosition_eeprom();
void setOpenPosition_eeprom(long pos);

long getClosePosition_eeprom();
void setClosePosition_eeprom(long pos);

void reset_eeprom();
