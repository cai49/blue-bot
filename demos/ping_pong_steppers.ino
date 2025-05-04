#include<AccelStepper.h>

#define STP_STEP 0
#define STP_DIR 1

const int STP1_PINS[2] = {2, 3};
const int STP2_PINS[2] = {4, 5};
const int PPR = 800;

unsigned long current_millis;
unsigned long blink_millis;
unsigned long blink_period = 500;

AccelStepper stp1(AccelStepper::DRIVER, STP1_PINS[STP_STEP], STP1_PINS[STP_DIR]);
AccelStepper stp2(AccelStepper::DRIVER, STP2_PINS[STP_STEP], STP2_PINS[STP_DIR]);

void ToggleLED();

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);

	stp1.setMaxSpeed(400);
	stp1.setAcceleration(800);
	stp1.moveTo(800);

	stp2.setMaxSpeed(800);
	stp2.setAcceleration(400);
	stp2.moveTo(-400);

	blink_millis = millis();
}

int pos = 0;
void loop()
{
	current_millis = millis();

	if (current_millis - blink_millis >= blink_period)
	{
		ToggleLED();
		blink_millis = current_millis;
	}

	if (stp1.distanceToGo() == 0)
	{
		stp1.moveTo(-stp1.currentPosition());
	}

	if (stp2.distanceToGo() == 0)
	{
		stp2.moveTo(-stp2.currentPosition());
	}
	
	stp1.run();
	stp2.run();
}

bool led_state = false;
void ToggleLED(void)
{
	if (led_state)
	{
		digitalWrite(LED_BUILTIN, HIGH);
		led_state = false;
	}
	else
	{
		digitalWrite(LED_BUILTIN, LOW);
		led_state = true;
	}
}
