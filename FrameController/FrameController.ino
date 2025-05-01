#include<AccelStepper.h>
#include<Servo.h>

void shut_device_off();

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

// { STEP, DIR, EN }
int pin_1[3] = {7, 8, 9};
int pin_2[3] = {10, 11, 12};
int pin_3[3] = {13, 14, 15};

#define STP_STEP 0
#define STP_DIR 1
#define STP_EN 2

AccelStepper stp1(AccelStepper::DRIVER, pin_1[STP_STEP], pin_1[STP_DIR]);
AccelStepper stp2(AccelStepper::DRIVER, pin_2[STP_STEP], pin_2[STP_DIR]);
AccelStepper stp3(AccelStepper::DRIVER, pin_3[STP_STEP], pin_3[STP_DIR]);

Servo end_effector;

unsigned long currentMillis;
unsigned long servoMillis;

const unsigned long servo_refresh_rate = 15;

void setup1()
{
  servoMillis = millis();

  end_effector.attach(2);

  stp1.setEnablePin(pin_1[STP_EN]);
  stp1.setMaxSpeed(100.0);
  stp1.setAcceleration(50.0);
  stp1.moveTo(200);

  stp2.setEnablePin(pin_2[STP_EN]);
  stp2.setMaxSpeed(200.0);
  stp2.setAcceleration(50.0);
  stp2.moveTo(200);

  stp3.setEnablePin(pin_3[STP_EN]);
  stp3.setMaxSpeed(300.0);
  stp3.setAcceleration(50.0);
  stp3.moveTo(200);
}

int pos = 0;
int reverse = 1;
void loop2()
{
  currentMillis = millis();

  if ((currentMillis - servoMillis >= servo_refresh_rate))
  {
    if (pos >= 180)
    {
      reverse = -1;
    }
    else if (pos <= 0)
    {
      reverse = 1;
    }

    end_effector.write(pos);
    pos += reverse;
    servoMillis = currentMillis;
  }

  int reps = 0;
  if (stp1.distanceToGo() == 0)
  {
    stp1.moveTo(-stp1.currentPosition());
    reps += 1;
  }

  if (!(stp1.distanceToGo() == 0 && stp2.distanceToGo() == 0 && stp3.distanceToGo() == 0))
  {
    running = true;
  }

  stp1.run();
  stp2.run();
  stp3.run();

  if (reps >= 6)
  {
    stp1.disableOutputs();
    running = false;
  }
}

void shut_device_off() { for (;;) {} }
