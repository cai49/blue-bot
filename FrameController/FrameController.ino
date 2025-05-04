#include<AccelStepper.h>
#include<Servo.h>

#include "HardwareRoutines.hpp"

volatile bool running = false;
volatile bool steppers_notification = false;

void setup() 
{
  Serial.begin(115200);
  while(!Serial) {}
  
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  if (Serial.available() > 0)
  {
    String message = Serial.readStringUntil('\n');

    Serial.print("Received: ");
    Serial.println(message);

    int message_value = message.toInt();
    if (message_value < 75 && message_value > 50)
    {
      Serial.println("Turning Off Serial");
      Serial.println("off");
      Serial.end();

      shut_device_off();
    }

    Serial.println("ok");
  }

  if (!running)
  {
    if (steppers_notification)
    {
      Serial.println("STEPPERS STOPPED");
      steppers_notification = false;
    }
  }

  if (running)
  {
    steppers_notification = true;
  }
}

/// Stepper Motors
  #define STP_STEP 0
  #define STP_DIR 1

  // { STEP, DIR }
  const int stp0_PINS[2] = { 7, 8 };
  const int stp1_PINS[2] = { 10, 11 };
  const int stp2_PINS[2] = { 13, 14 };

  const int STP_MAX_SPEED = 400;
  const int STP_MAX_ACCEL = 800;

  const int PPR = 800;       // pulse/rev
  const int SCREW_PITCH = 2; // mm/rev

  const int STP_N = 3;

/// Limit Switches
  const int LS_N = 3;
  const int LS_PINS[LS_N] = { 18, 19, 20 }; // X, Y, Z Axis
  const bool LS_ACTIVE = true;
  
  int ls_states[3] = { false, false, false };

/// Servo Motor
  const int SERVO_PIN = 2;

  unsigned long currentMillis;
  unsigned long servoMillis;

  const unsigned long servo_refresh_rate = 15;

Servo end_effector;

AccelStepper steppers[STP_N];
AccelStepper stp0(AccelStepper::DRIVER, stp0_PINS[STP_STEP], stp0_PINS[STP_DIR]);
AccelStepper stp1(AccelStepper::DRIVER, stp1_PINS[STP_STEP], stp1_PINS[STP_DIR]);
AccelStepper stp2(AccelStepper::DRIVER, stp2_PINS[STP_STEP], stp2_PINS[STP_DIR]);


void setup1()
{
  servoMillis = millis();

  end_effector.attach(SERVO_PIN);

  steppers[0] = stp0;
  steppers[1] = stp1;
  steppers[2] = stp2;

  configure_steppers();

  configure_limit_switches_pinmode();
  read_limit_switches_state(ls_states);
}

int pos = 0;
int reverse_dir = 1;
void loop1()
{
  currentMillis = millis();

  if ((currentMillis - servoMillis >= servo_refresh_rate))
  {
    if (pos >= 180)
    {
      reverse_dir = -1;
    }
    else if (pos <= 0)
    {
      reverse_dir = 1;
    }

    end_effector.write(pos);
    pos += reverse_dir;
    servoMillis = currentMillis;
  }
}

void shut_device_off(void) { for (;;) {} }

float get_position_meters(int axis)
{
  long steps = steppers[axis].currentPosition();

  float turns = steps / PPR; // revs

  return turns * SCREW_PITCH;
}

void configure_stepper(int axis)
{
  steppers[axis].setMaxSpeed(STP_MAX_SPEED);
  steppers[axis].setAcceleration(STP_MAX_ACCEL);
}

void configure_steppers(void)
{
  for (int i = 0; i < STP_N; ++i)
  {
    configure_stepper(i);
  }
}

void configure_limit_switches_pinmode(void)
{
  for (int i = 0; i < LS_N; ++i)
  {
    pinMode(LS_PINS[i], INPUT);
  }
}

void read_limit_switches_state(int *ls_states)
{
  for (int i = 0; i < LS_N; ++i)
  {
    ls_states[i] = digitalRead(LS_PINS[i]);
  }
}

void scan_for_zero(int axis)
{
  read_limit_switches_state(ls_states);

  if (ls_states[axis])
  {
    steppers[axis].setCurrentPosition(0);
    steppers[axis].moveTo(400);

    configure_stepper(axis);
  }
}
