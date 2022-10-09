### TODO :
- update stepper lib to accelstepper
- buy rtc module
- homing step motor - buy homing switch

### Utils :
- Send current time via Serial : echo "time_set $(date +%s)" > /dev/tty.usbserial-1220
- Reset EEPROM : echo 'eeprom_reset' > /dev/tty.usbserial-1220

### Infos :
- Stepper : 28byj-48

### FIFO readings info
- https://github.com/rlogiacco/CircularBuffer

### Libs 

- https://github.com/PaulStoffregen/Time
- http://github.com/JChristensen/Timezone
- https://github.com/arduino-libraries/Stepper
- https://github.com/dmkishi/Dusk2Dawn
