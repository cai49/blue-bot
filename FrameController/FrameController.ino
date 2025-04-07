#include "device_config.hpp"
#include "workflow_state.hpp"

bool led_on = false;

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

    Serial.print("CPU Temp: ");
    Serial.print(tempmonGetTemp());
    Serial.println("°C");

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
}

void shut_device_off() { for (;;) {} }
