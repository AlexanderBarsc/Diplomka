#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_HTU21DF.h"

Adafruit_HTU21DF htu = Adafruit_HTU21DF();

#define PHOTOTRAN_CONTROL 25
#define BUZZER_CONTROL 27
#define LED_CONTROL 13
#define PHOTOTRAN_OUTPUT_AD 32 
#define BUTTON_OUTPUT 26
#define TEMP_CONTROL 19
#define DEFAULT_DELAY 1000
#define MIC_CONTROL 4
#define MIC_OUTPUT 39
#define MQ2_OUTPUT 36
#define PIR_OUTPUT 34

#define I2C_SDA 22
#define I2C_SCL 21

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(TEMP_CONTROL, OUTPUT);
  digitalWrite(TEMP_CONTROL, HIGH);

  delay(2000);

  TwoWire I2CBME = TwoWire(0);

  I2CBME.begin(I2C_SDA, I2C_SCL, 400000);

  pinMode(BUTTON_OUTPUT, INPUT);
  

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
  pinMode(BUZZER_CONTROL, OUTPUT);
  digitalWrite(BUZZER_CONTROL, HIGH);
  delay(DEFAULT_DELAY);
  digitalWrite(BUZZER_CONTROL, LOW);
  
  Serial.println("Testuji LED");
  pinMode(LED_CONTROL, OUTPUT);
  digitalWrite(LED_CONTROL, HIGH);

  Serial.println("Nyni testuji tlacitko a fototranzistor");


  pinMode(PHOTOTRAN_CONTROL, OUTPUT);
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
  pinMode(MIC_CONTROL, OUTPUT);
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
  pinMode(PIR_OUTPUT,INPUT);

  while(digitalRead(BUTTON_OUTPUT) == true)
  {
    Serial.println(digitalRead(PIR_OUTPUT));
    delay(100);
  }


  Serial.println("Vse otestovano, nyni pokracuji v behu programu");



}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Ono to nejak funguje ¯\\_(ツ)_/¯");
  delay(1000);
}

int myFunction(int x, int y) {
  return x + y;
} 