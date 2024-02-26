#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_HTU21DF.h"
#include "ESP32Setup.h"

Adafruit_HTU21DF htu = Adafruit_HTU21DF();

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SetupPins();
  TestMicrophoneWithBuzzer();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Ono to nejak funguje ¯\\_(ツ)_/¯");
  delay(1000);
}

int myFunction(int x, int y) {
  return x + y;
} 