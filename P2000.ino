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

// #define MOTOR_SPEED 600.0
// #define MOTOR_ACCEL 100.0
// #define MOTOR_MODE  AccelStepper::FULL4WIRE

#define MOTOR_SPEED 1500.0
#define MOTOR_ACCEL 500.0
#define MOTOR_MODE  AccelStepper::HALF4WIRE

// PROTO
void blinkLED(int pin, int times = 1, int freq = 250);

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

AccelStepper Motor (MOTOR_MODE,
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

Button RunButton  (RUN_PIN);
Button HomeButton (HOME_PIN);

void setup()
{
  Serial.begin(9600);

  // upwards max speed at half step + 300g = 840 grand MAX
  // upwards max sans charge at half step 1700

  pinMode(DIRECTION_PIN, INPUT);

  // pinMode(RUN_PIN, INPUT_PULLUP);
  // pinMode(HOME_PIN, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);

  pinMode(STEPPER_PIN_1, OUTPUT);
  pinMode(STEPPER_PIN_2, OUTPUT);
  pinMode(STEPPER_PIN_3, OUTPUT);
  pinMode(STEPPER_PIN_4, OUTPUT);

  setSyncProvider(requestSync); // Set function to call when sync required

  Motor.setMaxSpeed(MOTOR_SPEED);
  Motor.setAcceleration(MOTOR_ACCEL);
  Motor.setCurrentPosition(0);

  HomeButton.begin();
  RunButton.begin();
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

void loop()
{
  btn_read();
  // if (Serial.available())
  // {
  //   processSyncMessage();
  // }

  // printCurrentTime();

  // int value = analogRead(SENSOR_PIN);
  // Serial.println("Current luminosity is " + value);
  // tmp test
  // if (value > 500)
  // {
  //   digitalWrite(LED_PIN, HIGH);
  // }
  // else
  // {
  //   digitalWrite(LED_PIN, LOW);
  // }

  if (btn_runHoming()) {
    runHoming();
    btn_read();
  }

  if (btn_run()) {
    if (isDirectionDown()) {
      Motor.runToNewPosition(homingClose);
    } else {
      Motor.runToNewPosition(homingOpen);
    }

    btn_read();
  }

  if (btn_setHome()) {
    if (isDirectionDown()) {
      homingClose = Motor.currentPosition();
    } else {
      homingOpen = Motor.currentPosition();
    }

    blinkLED(LED_PIN, 2, 500);
    btn_read();
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

bool btn_runHoming()
{
  return RunButton.pressedFor(1000) && HomeButton.pressedFor(1000);
}

bool btn_setHome()
{
  return HomeButton.pressedFor(3000) && RunButton.releasedFor(2000);
}

bool btn_run()
{
  return RunButton.pressedFor(1000) && HomeButton.releasedFor(2000);
}

void btn_read()
{
    HomeButton.read();
    RunButton.read();
}

void printCurrentTime()
{
  char buffer[50];
  sprintf(buffer, "%.2d:%.2d:%.2d", hour(), minute(), second());
  Serial.println(buffer);
}

void runHoming()
{
  int i = isDirectionDown() ? -1 : 1;

  homingLastPos = Motor.currentPosition();
  Motor.setCurrentPosition(0);

  while (btn_runHoming())
  {
    Motor.move(Motor.currentPosition() + i);
    Motor.run();

    btn_read();
  }

  if (isDirectionDown()) {
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

void blinkLED(int pin, int times = 1, int freq = 250)
{
  int lastState = digitalRead(pin);

  if (lastState == HIGH)
  {
    digitalWrite(pin, LOW);
    delay(freq);
  }

  for (size_t i = 0; i < times; i++)
  {
    digitalWrite(pin, HIGH);
    delay(freq);
    digitalWrite(pin, LOW);
    delay(freq);
  }

  digitalWrite(pin, lastState);
}

bool isDirectionUp()
{
  return !digitalRead(DIRECTION_PIN);
}

bool isDirectionDown()
{
  return digitalRead(DIRECTION_PIN);
}
