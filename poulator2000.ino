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

// https://github.com/arduino-libraries/Stepper
#include <Stepper.h>

// Board Pins
#define LED_PIN 2
#define SENSOR_PIN A0

#define STEPPER_PIN_1 3
#define STEPPER_PIN_2 4
#define STEPPER_PIN_3 5
#define STEPPER_PIN_4 6

#define HOMING_PIN 12

// Config
#define TIME_HEADER "T" // Header tag for serial time sync message
// #define STEPPER_SPEED 10 // From 1 to 15
#define STEPPER_SPEED 6 // From 1 to 15
#define STEPPER_STEPS 2048 // Full step mode for more torque
// #define STEPPER_STEPS 4096
#define TRAVEL_REVOLUTIONS 24

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

const int steps = STEPPER_STEPS * TRAVEL_REVOLUTIONS;

Stepper stepper (STEPPER_STEPS,
  STEPPER_PIN_1,
  STEPPER_PIN_3,
  STEPPER_PIN_2,
  STEPPER_PIN_4
);

bool isDST = false; // Daylight Saving Time (nb. Summer hours)

int homing = 0;



void setup()
{
  Serial.begin(9600);

  stepper.setSpeed(STEPPER_SPEED);

  pinMode(HOMING_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  setSyncProvider(requestSync); // Set function to call when sync required

  setHoming();
}

void loop()
{
  if (Serial.available())
  {
    processSyncMessage();
  }

  printCurrentTime();

  int value = analogRead(SENSOR_PIN);
  Serial.println("Current luminosity is " + value);

  // tmp test
  if (value > 500)
  {
    digitalWrite(LED_PIN, HIGH);
    // stepper.step(stepsPerRevolution);
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
    // stepper.step(-stepsPerRevolution);
  }

  delay(1000);
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

void printCurrentTime()
{
  char buffer[50];
  sprintf(buffer, "%.2d:%.2d:%.2d", hour(), minute(), second());
  Serial.println(buffer);
}

void setHoming()
{
  Serial.println("Initiating Homing...");

  while (digitalRead(HOMING_PIN))
  {
    stepper.step(homing);
    homing--;
    delay(1);
  }

  homing = 1;

  while (!digitalRead(HOMING_PIN)) {
    stepper.step(homing);
    homing++;
    delay(1);
  }

  homing = 0;
  Serial.println("Homing complete.");
}
