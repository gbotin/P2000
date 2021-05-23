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

// https://github.com/JChristensen/JC_Button
#include <JC_Button.h>

// Board Pins
#define LED_PIN 2
#define SENSOR_PIN A0

#define STEPPER_PIN_1 3
#define STEPPER_PIN_2 4
#define STEPPER_PIN_3 5
#define STEPPER_PIN_4 6

#define DIRECTION_PIN 7
#define RUN_PIN 9
#define HOME_PIN 10

// Config
#define TIME_HEADER "T" // Header tag for serial time sync message
// #define STEPPER_SPEED 10 // From 1 to 15
#define STEPPER_SPEED 6 // From 1 to 15
#define STEPPER_STEPS 2048 // Full step mode for more torque
// #define STEPPER_STEPS 4096
#define TRAVEL_REVOLUTIONS 24

#define MOTOR_SPEED 600.0
#define MOTOR_ACCEL 100.0

// STEPPER CONFIG
#define STEPPER_STEPS_PER_REV 2048

// DOOR CONSTS
// #define DOOR_STATE_UNDEF 0
// #define DOOR_STATE_CLOSED 1
// #define DOOR_STATE_OPENED 2

struct BusinessHours
{
  uint8_t opening;
  uint8_t closing;
};

const TimeChangeRule CEST = { "CEST", Last, Sun, Mar, 2, 120 }; // Central European Summer Time
const TimeChangeRule CET = { "CET", Last, Sun, Oct, 3, 60 };    // Central European Standard Time
Timezone CE(CEST, CET);

const BusinessHours dst_hours = { 7, 22 };
const BusinessHours st_hours = { 7, 22 };

AccelStepper Motor (AccelStepper::FULL4WIRE,
  STEPPER_PIN_1,
  STEPPER_PIN_3,
  STEPPER_PIN_2,
  STEPPER_PIN_4
);

bool isDST = false; // Daylight Saving Time (nb. Summer hours)

// short int state = DOOR_STATE_UNDEF;
// long homing = -1;
long homingLastPos = 0;
long homingOpen = 0;
long homingClose = 0;

void setup()
{
  Serial.begin(9600);

  // upwards max speed at half step + 300g = 840 grand MAX
  // upwards max sans charge at half step 1700

  pinMode(DIRECTION_PIN, INPUT);

  pinMode(RUN_PIN, INPUT_PULLUP);
  pinMode(HOME_PIN, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);

  pinMode(STEPPER_PIN_1, OUTPUT);
  pinMode(STEPPER_PIN_2, OUTPUT);
  pinMode(STEPPER_PIN_3, OUTPUT);
  pinMode(STEPPER_PIN_4, OUTPUT);

  setSyncProvider(requestSync); // Set function to call when sync required

  Motor.setMaxSpeed(MOTOR_SPEED);
  Motor.setAcceleration(MOTOR_ACCEL);
  Motor.setCurrentPosition(0);
}

// long getCurrentPosition_eeprom()
// {

// }

// void setCurrentPosition_eeprom()
// {

// }

// void setOpenPosition_eeprom()
// {

// }

// void setClosePosition_eeprom()
// {

// }

// long get

void loop()
{
  // if (Serial.available())
  // {
  //   processSyncMessage();
  // }

  // printCurrentTime();

  int value = analogRead(SENSOR_PIN);

  // Serial.println("Current luminosity is " + value);
  // tmp test
  if (value > 500)
  {
    digitalWrite(LED_PIN, HIGH);
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
  }

  if (shouldRunHoming()) {
    runHoming();
  }

  if (!digitalRead(RUN_PIN) && digitalRead(DIRECTION_PIN)) {
    Motor.runToNewPosition(homingClose);
  }

  if (!digitalRead(RUN_PIN) && !digitalRead(DIRECTION_PIN)) {
    Motor.runToNewPosition(homingOpen);
  }

  if (!digitalRead(HOME_PIN) && digitalRead(DIRECTION_PIN)) {
    homingClose = Motor.currentPosition();
  }

  if (!digitalRead(HOME_PIN) && !digitalRead(DIRECTION_PIN)) {
    homingOpen = Motor.currentPosition();
  }

  delay(100);
}

void processSyncMessage()
{
  unsigned long pctime;
  unsigned long localtime;

  if (Serial.find(TIME_HEADER))
  {
    pctime = Serial.parseInt();
    localtime = CE.toLocal(pctime);

    isDST = CE.locIsDST(localtime);
    setTime(localtime);
  }
}

BusinessHours getBusinessHours ()
{
  return isDST ? dst_hours : st_hours;
}

time_t requestSync()
{
  return 0; // Time will be sent later in response to serial msg
}

// bool shouldOpen() {
//   TODO
// }

// bool shouldClose() {
//   TODO
// }

bool shouldRunHoming()
{
  return !digitalRead(RUN_PIN) && !digitalRead(HOME_PIN);
}

void printCurrentTime()
{
  char buffer[50];
  sprintf(buffer, "%.2d:%.2d:%.2d", hour(), minute(), second());
  Serial.println(buffer);
}

void runHoming()
{
  homingLastPos = Motor.currentPosition();
  Motor.setCurrentPosition(0);

  while (shouldRunHoming())
  {
    if (digitalRead(DIRECTION_PIN)) {
      Motor.move(Motor.currentPosition() - 1);
    } else {
      Motor.move(Motor.currentPosition() + 1);
    }

    Motor.run();
  }

  if (digitalRead(DIRECTION_PIN)) {
    Motor.setCurrentPosition(homingLastPos - Motor.currentPosition());
  } else {
    Motor.setCurrentPosition(homingLastPos + Motor.currentPosition());
  }
}

void setHoming()
{
//   Serial.println("Initiating Homing...");

//   Motor.setCurrentPosition(0);  // Set the current position as zero for now
//   Motor.setMaxSpeed(100.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
//   Motor.setAcceleration(100.0);  // Set Acceleration of Stepper
//   homing = -1;

//   while (digitalRead(HOMING_PIN))
//   {
//     // Serial.println(homing);
//     Motor.moveTo(homing);
//     homing--;
//     // Motor.setSpeed(800);
//     // Motor.runSpeed();
//     Motor.run();

//     delay(5);
//   }

//   // Motor.setCurrentPosition(0);
//   // homing = 1;

//   Motor.setCurrentPosition(0);  // Set the current position as zero for now
//   Motor.setMaxSpeed(100.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
//   Motor.setAcceleration(100.0);  // Set Acceleration of Stepper
//   homing = 1;

//   while (!digitalRead(HOMING_PIN)) {
//     // Serial.println(homing);
//     Motor.moveTo(homing);
//     homing++;
//     // Motor.setSpeed(800);
//     // Motor.runSpeed();
//     Motor.run();

//     delay(5);
//   }

//   Motor.setCurrentPosition(0);
//   // homing = -1;

//   Serial.println("Homing complete.");
}
