#include "Arduino.h"
#include "ESP32Setup.h"
#include <Wire.h>
#include <Adafruit_HTU21DF.h>
#include <WiFi.h>

/// @brief Setups pins which are required for the system to work correctly.
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
  
  // Powers up sensors on board which have their voltage pin connected to GPIO
  digitalWrite(MIC_CONTROL, HIGH);
  digitalWrite(PHOTOTRAN_CONTROL, HIGH);
  digitalWrite(TEMP_CONTROL, HIGH);

  // Set outputs to low
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



