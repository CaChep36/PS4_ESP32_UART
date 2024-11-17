#include <Arduino.h>
#include <PS4Controller.h>
#include "stdint.h"

#define LED_PIN 2

#pragma pack(push, 1)
typedef struct
{
  uint16_t button;
  uint8_t l2_analog;
  uint8_t r2_analog;
  int8_t l_stick_x;
  int8_t l_stick_y;
  int8_t r_stick_x;
  int8_t r_stick_y;
  uint8_t sync;
} PS4_Dat;
#pragma pack(pop)

void Send_PS4_UART(PS4_Dat *data)
{
  data->sync = 0xff;
  uint8_t buffer[sizeof(PS4_Dat)];
  memcpy(buffer, data, sizeof(PS4_Dat));
  Serial.write(buffer, sizeof(PS4_Dat));
}

PS4_Dat getPS4Data()
{
  PS4_Dat data;
  data.button = (PS4.Right() << 0) |
                (PS4.Left() << 1) |
                (PS4.Up() << 2) |
                (PS4.Down() << 3) |
                (PS4.Cross() << 4) |
                (PS4.Circle() << 5) |
                (PS4.Square() << 6) |
                (PS4.Triangle() << 7) |
                (PS4.L1() << 8) |
                (PS4.R1() << 9) |
                (PS4.Share() << 10) |
                (PS4.Options() << 11) |
                (PS4.L3() << 12) |
                (PS4.R3() << 13) |
                (PS4.PSButton() << 14) |
                (PS4.Touchpad() << 15);
  data.l2_analog = PS4.L2Value();
  data.r2_analog = PS4.R2Value();
  data.l_stick_x = PS4.LStickX();
  data.l_stick_y = PS4.LStickY();
  data.r_stick_x = PS4.RStickX();
  data.r_stick_y = PS4.RStickY();
  return data;
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  PS4.begin("12:34:56:78:89:90");
  digitalWrite(LED_PIN, 1);
  while (!PS4.isConnected())
  {
  }
  PS4.setLed(0, 255, 0);
  PS4.sendToController();
}

void loop()
{
  static unsigned long previousMillis = 0;
  static bool ledState = LOW;
  const unsigned long blinkInterval = 80;

  if (PS4.isConnected())
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= blinkInterval)
    {
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }

    if (Serial.available())
    {
      char receivedChar = Serial.read();
      if (receivedChar == 'p')
      {
        PS4_Dat ps4_data = getPS4Data();
        Send_PS4_UART(&ps4_data);
      }
    }
  }
  else
  {
    digitalWrite(LED_PIN, HIGH);
  }
}
