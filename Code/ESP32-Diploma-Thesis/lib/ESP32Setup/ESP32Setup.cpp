#include "Arduino.h"
#include "ESP32Setup.h"
#include <Wire.h>
#include <Adafruit_HTU21DF.h>
#include <WiFi.h>

/// @brief Nastaví piny jenž jsou požadovány pro korektní funkcionalitu.
void SetupPins()
{
  pinMode(TEMP_CONTROL, OUTPUT);
  pinMode(BUTTON_OUTPUT, INPUT);
  pinMode(BUZZER_CONTROL, OUTPUT);
  pinMode(LED_CONTROL, OUTPUT);
  pinMode(PHOTOTRAN_CONTROL, OUTPUT);
  pinMode(MIC_CONTROL, OUTPUT);
  pinMode(PIR_OUTPUT, INPUT);
  pinMode(MQ2_DIGITAL_OUTPUT, INPUT);
  
  // Zapne napájení senzorům na desce jenž jsou napájeny přes GPIO
  digitalWrite(MIC_CONTROL, HIGH);
  digitalWrite(PHOTOTRAN_CONTROL, HIGH);
  digitalWrite(TEMP_CONTROL, HIGH);

  // Nastaví výstupy na logickou nulu
  digitalWrite(BUZZER_CONTROL, LOW);
  digitalWrite(LED_CONTROL, LOW);
}


OutputPins parseIntToOutputPin(int value)
{
  switch (value)
  {
    case LED:
    case BUZZER:
      return static_cast<OutputPins>(value);
    default:
      return UNDEFINED;
  }
}



