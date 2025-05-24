#include<Servo.h>
#include<AccelStepper.h>

#include "Configuration.hpp"
#include "HardwareRoutines.hpp"

volatile bool running = false;
volatile bool steppers_notification = false;

void setup() 
{
  // Serial.begin(115200);
  // while(!Serial) {}

  // Serial.println("Serial Initialized Correctly");
  
  // pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // if (Serial.available() > 0)
  // {
  //   String message = Serial.readStringUntil('\n');

  //   Serial.print("Received: ");
  //   Serial.println(message);

  //   int message_value = message.toInt();
  //   Serial.print("Value from mm: ");
  //   Serial.println(_get_pulses(message_value));

  //   Serial.println("ok");
  // }

  // if (!running)
  // {
  //   if (steppers_notification)
  //   {
  //     Serial.println("STEPPERS STOPPED");
  //     steppers_notification = false;
  //   }
  // }

  // if (running)
  // {
  //   steppers_notification = true;
  // }
}

Servo end_effector;

AccelStepper *steppers[STP_N];
AccelStepper stp0(AccelStepper::DRIVER, stp0_PINS[STP_STEP], stp0_PINS[STP_DIR]);
AccelStepper stp1(AccelStepper::DRIVER, stp1_PINS[STP_STEP], stp1_PINS[STP_DIR]);
AccelStepper stp2(AccelStepper::DRIVER, stp2_PINS[STP_STEP], stp2_PINS[STP_DIR]);


void setup1()
{
  Serial.begin(115200);
  while(!Serial) {}

  Serial.println("Serial Initialized Correctly");

  servoMillis = millis();

  end_effector.attach(SERVO_PIN, 544, 2400);

  configure_steppers();
  configure_limit_switches_pinmode();

  // find_zeros();

  delay(500);

  set_position(50., -50., 50.);
}

int pos = 0;
int dir = 1;
bool run_ = true;
void loop1()
{
  while (run_)
  {
    if (Serial.available() > 0)
    {
      String message = Serial.readStringUntil('\n');
      
      Serial.print("Received: ");
      Serial.println(message);

      if (message == "run")
      {
        run_ = false;
      }
    }
  }

  currentMillis = millis();

  if (currentMillis - servoMillis >= servo_refresh_rate)
  {
    servoMillis = currentMillis;
    pos += dir;

    if (pos == 0 || pos == 180)
    {
      dir = -dir;
    }

    end_effector.write(pos);
  }

  update_steppers();
}

void shut_device_off(void) { for (;;) {} }

void configure_stepper(int axis)
{
  steppers[axis]->setMaxSpeed(STP_MAX_SPEED);
  steppers[axis]->setAcceleration(STP_MAX_ACCEL);
}

void configure_steppers(void)
{
  steppers[0] = &stp0;
  steppers[1] = &stp1;
  steppers[2] = &stp2;

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
    ls_states[i] = 1 - digitalRead(LS_PINS[i]);
  }
}

bool _scan_for_zero(int axis)
{
  read_limit_switches_state(ls_states);
  
  if (ls_states[axis])
  {
    steppers[axis]->setCurrentPosition(0);
    return true;
  }
  return false;
}

void find_zeros()
{
  for (int i = 0; i < STP_N; ++i)
  {
    steppers[i]->setSpeed(ZERO_SPEED);

    // 0:X | 1:Y | 2:Z
    while (!_scan_for_zero(i))
    {
      steppers[i]->run();
    }

    steppers[i]->moveTo(400);

    while(steppers[i]->distanceToGo() != 0)
    {
      steppers[i]->run();
    }

    steppers[i]->setSpeed(ZERO_SPEED);
    while (!_scan_for_zero(i))
    {
      steppers[i]->run();
    }

    // String message = "Axis {" + String(i) + "} found Zero";
    // Serial.println(message);
  }
}

float _get_position_meters(int axis)
{
  float steps = steppers[axis]->currentPosition();

  float turns = steps / PPR; // revs

  float travel = turns * SCREW_PITCH; // mm

  return travel;
}

float _get_pulses(float travel)
{
  float turns = travel / SCREW_PITCH; // revs
  
  float pulses = turns * PPR; // pulses

  return pulses;
}

bool set_position(double x, double y, double z)
{
  long x_pul = _get_pulses(x);
  long y_pul = _get_pulses(y);
  long z_pul = _get_pulses(z);

  long positions[3] = { x_pul, y_pul, z_pul };

  for (int i = 0; i < STP_N; ++i)
  {
    steppers[i]->moveTo(positions[i]);

    if (steppers[i]->distanceToGo() != 0)
    {
      steppers[i]->run();
    }
  }

  return true;
}

void set_position_now(double x, double y, double z)
{
  long x_pul = _get_pulses(x);
  long y_pul = _get_pulses(y);
  long z_pul = _get_pulses(z);

  long positions[3] = { x_pul, y_pul, z_pul };

  for (int i = 0; i < STP_N; ++i)
  {
    steppers[i]->moveTo(positions[i]);

    while (steppers[i]->distanceToGo() != 0)
    {
      steppers[i]->run();
    }
  }
}

void update_steppers(void)
{
  for (int i = 0; i < STP_N; ++i)
  {
    if (steppers[i]->distanceToGo() != 0)
    {
      steppers[i]->run();
    }
  }
}
