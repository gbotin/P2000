Infos :
Stepper : 28byj-48

http://suncalc.net/#/50.6244,2.6875,9/2023.12.01/15:34

FIFO readings
https://github.com/rlogiacco/CircularBuffer

https://github.com/adafruit/RTClib

### TODO :
- update stepper lib to accelstepper
- buy rtc module
- homing step motor - buy homing switch

### Utils :
- Read current time : echo "time_print" > /dev/tty.usbserial-1230
- Send current time : echo "time_set $(date +%s)" > /dev/tty.usbserial-1440
- Reset EEPROM : echo 'eeprom_reset' > /dev/tty.usbserial-1230

### Infos :
- Stepper : 28byj-48

### FIFO readings info
- https://github.com/rlogiacco/CircularBuffer

### Libs 

- https://github.com/PaulStoffregen/Time
- http://github.com/JChristensen/Timezone
- https://github.com/arduino-libraries/Stepper
- https://github.com/dmkishi/Dusk2Dawn



### Power Consumption

Mode SET idle : 0.0358V
Motor running 

sleep 0.146


max 0.2620
