#include "Arduino.h"
#include "ESP32Setup.h"
#include <Wire.h>
#include <Adafruit_HTU21DF.h>
#include <WiFi.h>

void Test()
{
  Adafruit_HTU21DF htu = Adafruit_HTU21DF();
  Serial.begin(115200);

  digitalWrite(TEMP_CONTROL, HIGH);

  delay(2000);

  TwoWire I2CBME = TwoWire(0);

  I2CBME.begin(I2C_SDA, I2C_SCL, 400000);

  if (!htu.begin(&I2CBME)) {
    Serial.println("Check circuit. HTU21D not found!");
    while(1)
    {
      
    }
  }

 while(digitalRead(BUTTON_OUTPUT) == true)
  {
    Serial.println("Namerena teplota je: ");
    Serial.println(htu.readTemperature());
    Serial.println("Namerena vlhkost je: ");
    Serial.println(htu.readHumidity());
    delay(DEFAULT_DELAY);
  }

  Serial.println("Testuji zvonek");
  digitalWrite(BUZZER_CONTROL, HIGH);
  delay(DEFAULT_DELAY);
  digitalWrite(BUZZER_CONTROL, LOW);
  
  Serial.println("Testuji LED");
  digitalWrite(LED_CONTROL, HIGH);

  Serial.println("Nyni testuji tlacitko a fototranzistor");

  digitalWrite(PHOTOTRAN_CONTROL, HIGH);

  int photoValue = 0;
  while(digitalRead(BUTTON_OUTPUT) == true)
  {
    photoValue = analogRead(PHOTOTRAN_OUTPUT_AD);
    Serial.println(photoValue);
    delay(DEFAULT_DELAY);
  }
  Serial.println("Konec testovani");
  delay(DEFAULT_DELAY);
  digitalWrite(MIC_CONTROL, HIGH);

  Serial.println("Nyni testuji mikrofon.");

  while(digitalRead(BUTTON_OUTPUT) == true)
  {
    photoValue = analogRead(MIC_OUTPUT);
    Serial.println(photoValue);
    delay(DEFAULT_DELAY);
  }

  Serial.println("Konec testovani");
  delay(1000);

  Serial.println("Nyni testuji MQ2.");

  while(digitalRead(BUTTON_OUTPUT) == true)
  {
    photoValue = analogRead(MQ2_OUTPUT);
    Serial.println(photoValue);
    delay(DEFAULT_DELAY);
  }

  Serial.println("Nyni testuji PIR");
  delay(1000);

  while(digitalRead(BUTTON_OUTPUT) == true)
  {
    Serial.println(digitalRead(PIR_OUTPUT));
    delay(100);
  }

  Serial.println("Vse otestovano, nyni pokracuji v behu programu");
}

void SetupPins()
{
  pinMode(TEMP_CONTROL, OUTPUT);
  pinMode(BUTTON_OUTPUT, INPUT);
  pinMode(BUZZER_CONTROL, OUTPUT);
  pinMode(LED_CONTROL, OUTPUT);
  pinMode(PHOTOTRAN_CONTROL, OUTPUT);
  pinMode(MIC_CONTROL, OUTPUT);
  pinMode(PIR_OUTPUT, INPUT);
  
  // Powers up sensors on board which have their voltage pin connected to GPIO
  digitalWrite(MIC_CONTROL, HIGH);
  digitalWrite(PHOTOTRAN_CONTROL, HIGH);
  digitalWrite(TEMP_CONTROL, HIGH);
}

/// @brief Test Microphone if it works with buzzer
void TestMicrophoneWithBuzzer()
{
  uint16_t micValue;
  uint16_t arr[20];
  digitalWrite(MIC_CONTROL, HIGH);
  digitalWrite(BUZZER_CONTROL, HIGH);
  
  for(int i = 0; i < 10; i++)
  {
    arr[i] = analogRead(MIC_OUTPUT);
    delay(DEFAULT_DELAY);
  }

  digitalWrite(BUZZER_CONTROL, LOW);
  for(int i = 10; i < 20; i++)
  {
    arr[i] = analogRead(MIC_OUTPUT);
    delay(DEFAULT_DELAY);
  }

  for(int i = 0; i < 20; i++)
  {
    Serial.println(arr[i]);
  }
}

void GetWifiPasswordFromAp(const char* ssid, const char* password)
{
  WiFi.softAP(ssid, password);
  Serial.println("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  
}