// Utils :
// Send current time via Serial : date +T%s\n > /dev/tty.usbserial-141410

// TODO :
// - update stepper lib to accelstepper
// - buy rtc module
// - homing step motor - buy homing switch

// Infos :
// Stepper : 28byj-48

// FIFO readings
// https://github.com/rlogiacco/CircularBuffer

// https://github.com/PaulStoffregen/Time
#include <Time.h>
#include <TimeLib.h>

// http://github.com/JChristensen/Timezone
#include <Timezone.h>

// https://www.pjrc.com/teensy/td_libs_AccelStepper.html
#include <AccelStepper.h>

// Board Pins
#define LED_PIN 2

// #define SENSOR_PIN A0

#define STEPPER_PIN_1 3
#define STEPPER_PIN_2 4
#define STEPPER_PIN_3 5
#define STEPPER_PIN_4 6

// Config
#define TIME_HEADER "T" // Header tag for serial time sync message

// #define MOTOR_SPEED 600.0
// #define MOTOR_ACCEL 100.0
// #define MOTOR_MODE  AccelStepper::FULL4WIRE

#define MOTOR_SPEED 1000.0 //1500.0
#define MOTOR_ACCEL 500.0
#define MOTOR_MODE AccelStepper::HALF4WIRE

#include "src/buttons.h"
#include "src/eeprom.h"
#include "src/led.h"

// PROTO
// void blinkLED(int pin, int times = 1, int freq = 250);

// struct BusinessHours
// {
//   uint8_t opening;
//   uint8_t closing;
// };

// const TimeChangeRule CEST = { "CEST", Last, Sun, Mar, 2, 120 }; // Central European Summer Time
// const TimeChangeRule CET = { "CET", Last, Sun, Oct, 3, 60 };    // Central European Standard Time
// Timezone CE(CEST, CET);

// const BusinessHours dst_hours = { 7, 22 };
// const BusinessHours st_hours = { 7, 22 };

AccelStepper Motor(
  MOTOR_MODE,
  STEPPER_PIN_1,
  STEPPER_PIN_3,
  STEPPER_PIN_2,
  STEPPER_PIN_4
);

// bool isDST = false; // Daylight Saving Time (nb. Summer hours)

// short int state = DOOR_STATE_UNDEF;
// long homing = -1;
long currentPosition = 0;
long openPosition    = 0;
long closePosition   = 0;

void setup()
{
  Serial.begin(9600);

  currentPosition = getCurrentPosition_eeprom();
  openPosition    = getOpenPosition_eeprom();
  closePosition   = getClosePosition_eeprom();

  // upwards max speed at half step + 300g = 840 grand MAX
  // upwards max sans charge at half step 1700

  pinMode(LED_PIN, OUTPUT);

  pinMode(STEPPER_PIN_1, OUTPUT);
  pinMode(STEPPER_PIN_2, OUTPUT);
  pinMode(STEPPER_PIN_3, OUTPUT);
  pinMode(STEPPER_PIN_4, OUTPUT);

  //   setSyncProvider(requestSync); // Set function to call when sync required

  Motor.setMaxSpeed(MOTOR_SPEED);
  Motor.setAcceleration(MOTOR_ACCEL);
  Motor.setCurrentPosition(currentPosition);


  btn_setup();
}

void loop()
{
  btn_read();

//   Serial.println("---");
//   Serial.println("currpos : " + String(Motor.currentPosition()) + " / " + String(currentPosition));
//   Serial.println("openpos : " + String(openPosition));
//   Serial.println("closepos : " + String(closePosition));
//   Serial.println(isDirectionDown() ? "down" : "up");
//   Serial.println("---");

  if (Serial.available())
  {
    // processSyncMessage();

    String command = Serial.readString();
    command.trim();

    if (command == "reset_eeprom")
    {
      Serial.println("Resetting EEPROM...");

      reset_eeprom();

      Serial.println("Done.");
      led_blink(LED_PIN, 2, 500);
    }
  }

  // printCurrentTime();

  // working
  // showLuminosity();

  if (btn_runHoming())
  {
    Serial.println("btn_runHoming");
    runHoming();
    setCurrentPosition_eeprom(Motor.currentPosition());
    btn_read();
  }

  if (btn_run())
  {
    if (isDirectionDown())
    {
      Serial.println("btn_run close");
      Motor.runToNewPosition(closePosition);
    }

    if (isDirectionUp())
    {
      Serial.println("btn_run open");
      Motor.runToNewPosition(openPosition);
    }

    btn_read();
  }

  if (btn_setHome())
  {
    if (isDirectionDown() && closePosition != Motor.currentPosition())
    {
      Serial.println("btn_setHome close");
      closePosition = Motor.currentPosition();
      setClosePosition_eeprom(closePosition);
    }

    if (isDirectionUp() && openPosition != Motor.currentPosition())
    {
      Serial.println("btn_setHome open");
      openPosition = Motor.currentPosition();
      setOpenPosition_eeprom(openPosition);
    }

    // blinkLED(LED_PIN, 2, 500);
    btn_read();
  }

  delay(100);
}

void runHoming()
{
  int i = isDirectionDown() ? -1 : 1;

  currentPosition = Motor.currentPosition();
  Motor.setCurrentPosition(0);

  while (btn_runHoming())
  {
    Motor.move(Motor.currentPosition() + i);
    Motor.run();

    btn_read();
  }

  currentPosition = currentPosition + Motor.currentPosition();
  Motor.setCurrentPosition(currentPosition);
}








// bool shouldOpen() {
//   TODO
// }

// bool shouldClose() {
//   TODO
// }

// void printCurrentTime()
// {
//   char buffer[50];
//   sprintf(buffer, "%.2d:%.2d:%.2d", hour(), minute(), second());
//   Serial.println(buffer);
// }



// void processSyncMessage()
// {
//   unsigned long pctime;
//   unsigned long localtime;

//   if (Serial.find(TIME_HEADER))
//   {
//     pctime = Serial.parseInt();
//     localtime = CE.toLocal(pctime);

//     isDST = CE.locIsDST(localtime);
//     setTime(localtime);
//   }
// }

// BusinessHours getBusinessHours ()
// {
//   return isDST ? dst_hours : st_hours;
// }

// working - not needed
// void showLuminosity()
// {
//     int value = analogRead(SENSOR_PIN);
//   Serial.println("Current luminosity is " + value);

//   if (value > 500)
//   {
//     digitalWrite(LED_PIN, HIGH);
//   }
//   else
//   {
//     digitalWrite(LED_PIN, LOW);
//   }
// }


time_t requestSync()
{
  return 0; // Time will be sent later in response to serial msg
}
