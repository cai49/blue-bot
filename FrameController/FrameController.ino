#include<Servo.h>
#include<AccelStepper.h>

#include "Configuration.hpp"
#include "HardwareRoutines.hpp"

volatile bool running = false;
volatile bool steppers_notification = false;

volatile double target_x = 0;
volatile double target_y = 0;
volatile double target_z = 0;

volatile bool draw_shape = false;
volatile double X1 = 0;
volatile double Y1 = 0;
volatile double X2 = 0;
volatile double Y2 = 0;

int shape_state = 0;

volatile bool completed_movement = false;

volatile bool run_ = true;

volatile bool stop_all = false;

volatile bool set = false;

volatile bool has_home = false;
bool set_home = false;

bool update_done = false;

void setup() 
{
  Serial.begin(115200);
  while(!Serial) {}

  Serial.println("Serial Initialized Correctly");
}

void loop()
{
  if (stop_all)
  {
    shut_device_off();
  }

  while (run_)
  {
    if (Serial.available() > 0)
    {
      String message = Serial.readStringUntil('\n');
      
      Serial.print("Received: ");
      Serial.println(message);

      if (message == "#RUN")
      {
        run_ = false;
        report_done();
      }
    }
  }

  if (Serial.available() > 0)
  {
    update_done = true;
    String message = Serial.readStringUntil('\n');

    String directive = message.substring(0,1);

    if (directive == "#")
    {
      String command = message.substring(1, message.length());

      Serial.println(command);
      
      if (command == "STOP")
      {
        stop_all = true;
      }
      else if (command == "HOME")
      {
        set_home = true;
      }
    }
    else if (directive == "-")
    {
      int command_separator = message.lastIndexOf(":");
      String command = message.substring(1, command_separator);
      command.toLowerCase();
      String argument = message.substring(command_separator+1, message.length());

      Serial.print(command);
      Serial.print(" -> ");
      Serial.println(argument);

      if (command == "pset")
      {
        String list = argument.substring(1, argument.length()-1);

        // Serial.println(list); // 1.00,0.00,0.00

        int _x_idx = list.indexOf(','); // 4
        int _y_idx = list.indexOf(',', _x_idx+1); // 9

        String _target_x = list.substring(0, _x_idx);
        String _target_y = list.substring(_x_idx+1, _y_idx);
        String _target_z = list.substring(_y_idx+1, list.length());

        Serial.println(_target_x + " : " + _target_y + " : " + _target_z);

        target_x = _target_x.toDouble();
        target_y = _target_y.toDouble();
        target_z = _target_z.toDouble();

        set = true;
      }
      else if (command == "drect") // -drect:[X1,Y1,X2,Y2]
      {
        String list = argument.substring(1, argument.length()-1);

        int _x1_idx = list.indexOf(',');
        int _y1_idx = list.indexOf(',', _x1_idx+1);
        int _x2_idx = list.indexOf(',', _y1_idx+1);

        String _x1 = list.substring(0, _x1_idx);
        String _y1 = list.substring(_x1_idx+1, _y1_idx);
        String _x2 = list.substring(_y1_idx+1, _x2_idx);
        String _y2 = list.substring(_x2_idx+1, list.length());

        Serial.println(_x1 + " : " + _y1 + " : " + _x2 + " : " + _y2);

        X1 = _x1.toDouble();
        Y1 = _y1.toDouble();
        X2 = _x2.toDouble();
        Y2 = _y2.toDouble();

        draw_shape = true;
      }
    }
  }
}

Servo end_effector;

AccelStepper *steppers[STP_N];
AccelStepper stp0(AccelStepper::DRIVER, stp0_PINS[STP_STEP], stp0_PINS[STP_DIR]);
AccelStepper stp1(AccelStepper::DRIVER, stp1_PINS[STP_STEP], stp1_PINS[STP_DIR]);
AccelStepper stp2(AccelStepper::DRIVER, stp2_PINS[STP_STEP], stp2_PINS[STP_DIR]);


void setup1()
{
  pinMode(LED_BUILTIN, OUTPUT);

  servoMillis = millis();

  end_effector.attach(SERVO_PIN, 544, 2400);

  while (run_) {}

  digitalWrite(LED_BUILTIN, HIGH);
    configure_steppers();
    configure_limit_switches_pinmode();
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
}

int pos = 0;
int dir = 1;

void loop1()
{
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

  if (stop_all)
  {
    shut_device_off();
  }

  if (set)
  {
    set_position(target_x, target_y, target_z);
    set = false;
  }

  if (set_home)
  {
    if (has_home == false)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      find_zeros();
      digitalWrite(LED_BUILTIN, LOW);
      has_home = true;
    }
    else
    {
      set_position(0., 0., 0.);
    }

    set_home = false;
  }

  if (draw_shape)
  {
    if (completed_movement)
    {
      switch (shape_state)
      {
      case 0:
        set_position(X1, Y1, PEN_UP);
        shape_state += 1;
        break;
      case 1:
        set_position(X1, Y1, PEN_DOWN);
        shape_state += 1;
        break;
      case 2:
        set_position(X2, Y1, PEN_DOWN);
        shape_state += 1;
        break;
      case 3:
        set_position(X2, Y2, PEN_DOWN);
        shape_state += 1;
        break;
      case 4:
        set_position(X1, Y2, PEN_DOWN);
        shape_state += 1;
        break;
      case 5:
        set_position(X1, Y1, PEN_DOWN);
        shape_state += 1;
        break;
      case 6:
        set_position(X1, Y1, PEN_UP);
        shape_state += 1;
        break;
      case 7:
        report_done();
        draw_shape = false;
        shape_state = 0;
        break;
      }
    }
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

void read_limit_switches_state(void)
{
  for (int i = 0; i < LS_N; ++i)
  {
    ls_states[i] = 1 - digitalRead(LS_PINS[i]);
  }
}

bool _scan_for_zero(int axis)
{
  read_limit_switches_state();
  
  if (ls_states[axis])
  {
    steppers[axis]->setCurrentPosition(0);
    return true;
  }
  return false;
}

void find_zeros()
{
  for (int i = 0; i < LS_N; ++i)
  {
    steppers[i]->setSpeed(ZERO_SPEED);

    // 0:X | 1:Y | 2:Z
    while (!_scan_for_zero(i))
    {
      steppers[i]->runSpeed();
    }

    steppers[i]->moveTo(400);

    while(steppers[i]->distanceToGo() != 0)
    {
      steppers[i]->run();
    }

    steppers[i]->setSpeed(ZERO_SPEED);
    while (!_scan_for_zero(i))
    {
      steppers[i]->runSpeed();
    }

    steppers[i]->moveTo(400);

    while(steppers[i]->distanceToGo() != 0)
    {
      steppers[i]->run();
    }

    configure_stepper(i);

    // String message = "Axis {" + String(i) + "} found Zero";
    // Serial.println(message);
  }
}

float _get_position_meters(int axis)
{
  float _steps = steppers[axis]->currentPosition();

  float turns = _steps / PPR; // revs

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
  long _steps = 0;
  for (int i = 0; i < STP_N; ++i)
  {
    long steps_to_go = steppers[i]->distanceToGo();
    _steps += steps_to_go;

    if (steps_to_go != 0)
    {
      steppers[i]->run();
    }
  }

  if (_steps == 0)
  {
    completed_movement = true;
  }
  else
  {
    completed_movement = false;
  }
}

void report_done(void)
{
  Serial.println("OK");
}

