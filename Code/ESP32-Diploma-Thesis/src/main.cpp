#include <Arduino.h>
#include <Wire.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include "Adafruit_HTU21DF.h"
#include "ESP32Setup.h"
#include "ESP32WebCommunication.h"
#include "time.h"
#include "ThingSpeak.h"
#include "Measurement.h"

unsigned long time_now = 0;

#define PERIOD 500
#define WIFI
#define DEBUG

// Web server running on port 80

Measurement meas;
WebServer server(80);
WiFiClient myClient;
WiFiManager wm;

boolean pirUpdate = false;
boolean eraseWifiConfig = false;

TwoWire twoWire = TwoWire(0);
Adafruit_HTU21DF htu = Adafruit_HTU21DF();

char buffer[1024];

void addJsonObject(JsonDocument &jsonDocument, const char *name, float value, const char *unit)
{
  JsonObject obj = jsonDocument.createNestedObject();
  obj["name"] = name;
  obj["value"] = value;
  obj["unit"] = unit;
}

void addTimeJsonObject(JsonDocument &jsonDocument, const char *name, char *str, const char *unit)
{
  JsonObject obj = jsonDocument.createNestedObject();
  obj["name"] = name;
  obj["value"] = str;
  obj["unit"] = unit;
}

void getDigitalPinValue()
{
  StaticJsonDocument<20> jsonDocument;
  if (server.args() == 0)
  {
    server.send(BAD_REQUEST, "text/plain", "No query arguments in URL");
    return;
  }

  String pinFromInput = server.arg(0);
  #ifdef DEBUG
  Serial.println(pinFromInput);
#endif

  OutputPins pin = parseIntToOutputPin(pinFromInput.toInt());

  if (pin == UNDEFINED)
  {
    server.send(BAD_REQUEST, "text/plain", "Invalid pin selected");
    return;
  }

  jsonDocument.clear();
  JsonObject obj = jsonDocument.createNestedObject();
  obj["pin"] = pin;
  obj["value"] = digitalRead(pin);

  serializeJson(obj, buffer);
  server.send(OK, "application/json", buffer);
  return;
}
void setDigitalPin()
{
  if (server.hasArg("plain") == false)
  {
    // handle error here
  }

  StaticJsonDocument<200> doc;
  deserializeJson(doc, server.arg("plain"));

  // TODO - Works but maybe think about undefined behaviour
  uint8_t pinNumber = doc["pin"];
  uint8_t value = doc["value"];

  OutputPins pin = parseIntToOutputPin(pinNumber);

  if (pin == UNDEFINED)
  {
    server.send(BAD_REQUEST, "text/plain", "Invalid pin selected");
    return;
  }

  digitalWrite(pin, value);
  server.send(OK);
}

void setupApi()
{
  server.on("/getDigitalPinState", getDigitalPinValue);
  server.on("/setDigitalPin", HTTP_POST, setDigitalPin);

  server.begin();
}

void IRAM_ATTR buttonPress()
{
  eraseWifiConfig = true;
}

/// @brief Interrupt which handles rising edge signals from PIR Module
/// @return
void IRAM_ATTR pirInterrupt()
{
  pirUpdate = true;
  detachInterrupt(PIR_OUTPUT);
}

void setup()
{

  #ifdef DEBUG
  Serial.println("Enter main");
  #endif

  SetupPins();
  // Inicialization delay
  delay(2000);

  twoWire.begin(I2C_SDA, I2C_SCL, 400000);
  if(htu.begin(&twoWire) == false)
  {
     Serial.println("HTU INIT FAILED");
     esp_restart();
  }

  attachInterrupt(PIR_OUTPUT, pirInterrupt, RISING);
  attachInterrupt(BUTTON_OUTPUT, buttonPress, FALLING);

  WiFi.mode(WIFI_STA);

  bool res;

  res = wm.autoConnect("AutoConnectAP", "password"); // password protected ap

  if (!res)
  {
    Serial.println("Failed to connect");
    ESP.restart();
  }
  else
  {
    Serial.println("Connected to WiFi");
  }

  
  ThingSpeak.begin(myClient);

  #ifdef DEBUG
  Serial.println("Start loop: ");
  #endif
}

void loop()
{
  //TODO - this will overflow after 50 days, fix it
  if(millis() >= time_now + PERIOD)
  {
    #ifdef DEBUG
    Serial.println("Reading values:");
    #endif
    time_now += PERIOD;

    meas.Measure(htu);

    if(meas.index >= ARRAY_SIZE)
    {
      meas.SendMeasurement();
      meas.WipeMeasurements();

    }
  }
  

  
}


