#include <Arduino.h>
#include <Wire.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include "Adafruit_HTU21DF.h"
#include "ESP32Setup.h"
#include "ESP32WebCommunication.h"
#include "time.h"
#include "ThingSpeak.h"
#include "Measurement.h"
#include "esp32-hal-i2c.h"
#include "HTU21D.h"


unsigned long time_now = 0;
String existingApiKey = "";
unsigned long existingChannelNumber = 0;
boolean succesfullCredentials = false;

#define PERIOD 100

// Web server running on port 80
Measurement meas;
WebServer server(80);
WiFiClient myClient;
WiFiManager wm;

volatile bool buttonPressed = false;
volatile bool mqSensorTripped = false;

HTU21D htuNew = HTU21D();
void EvaluateButtonPress();

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

void setThingSpeakCredentials()
{
  Serial.println("Set ThingSpeak Credentials..");
  if (server.hasArg("plain") == false)
  {
    // handle error here
  }

  StaticJsonDocument<200> doc;
  deserializeJson(doc, server.arg("plain"));

  // TODO - Works but maybe think about undefined behaviour
  unsigned long channelNumber = doc["channelNumber"];
  const char* apiKey = doc["apiKey"];

  #ifdef DEBUG
  Serial.println(channelNumber);
  Serial.println(apiKey);
  #endif

  EEPROM.writeULong(0, channelNumber);
  EEPROM.writeString(4, apiKey);

  

  int result = 0;

  result = ThingSpeak.writeField(channelNumber, 6, digitalRead(PIR_OUTPUT), apiKey);
  
  if(result != 200)
  {
     String output = String("Cannot write status to ThingSpeak: ");
     output.concat(result);
     server.send(BAD_REQUEST, "text/plain", output);
     return;
  }

  if(!EEPROM.commit())
  {
    server.send(BAD_REQUEST, "text/plain", "Cannot write to EEPROM");
    return;
  }
  
  existingChannelNumber = EEPROM.readULong(0);
  existingApiKey = EEPROM.readString(4);
  
  succesfullCredentials = true;
  server.send(OK);
}

void setupApi()
{
  server.on("/getDigitalPinState", getDigitalPinValue);
  server.on("/setDigitalPin", HTTP_POST, setDigitalPin);
  server.on("/setThingSpeakCredentials", HTTP_POST, setThingSpeakCredentials);

  server.begin();
}

void IRAM_ATTR buttonPress()
{
  detachInterrupt(BUTTON_OUTPUT);
  buttonPressed = true;
}

void IRAM_ATTR mq2Interrupt()
{
  detachInterrupt(MQ2_DIGITAL_OUTPUT);
  digitalWrite(BUZZER_CONTROL, HIGH);
  mqSensorTripped = true;
}

void setup()
{

  #ifdef DEBUG
  Serial.println("Enter main");
  #endif

  SetupPins();
  // Inicialization delay
  delay(INITIAL_DELAY);
  //Init of EEPROM
  EEPROM.begin(EEPROM_SIZE);

  bool result = htuNew.Begin(0, I2C_SDA, I2C_SCL, 400000);

  if(!result)
  {
    Serial.println("HTU21D failure");
    esp_restart();
  }

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

  setupApi();
  ThingSpeak.begin(myClient);

  existingChannelNumber = EEPROM.readULong(0);
  existingApiKey = EEPROM.readString(4);

  #ifdef DEBUG
  Serial.println(existingChannelNumber);
  Serial.println(existingApiKey);
  #endif 

  time_now = millis();
  while(!(ThingSpeak.writeField(existingChannelNumber, 6, digitalRead(PIR_OUTPUT), &existingApiKey[0]) == 200))
  {
    Serial.println("Cannot send message to ThingSpeak, try to reenter your credentials");
    while(!succesfullCredentials)
    {
      server.handleClient();
      if(millis() >= time_now + 100)
      { 
        time_now += 100;
        *(uint32_t*)(GPIO_OUT_REG) ^= (1 << LED_CONTROL);
      }
    }
      break;
  }


  digitalWrite(LED_CONTROL, LOW);

  attachInterrupt(MQ2_DIGITAL_OUTPUT, mq2Interrupt, FALLING);
  attachInterrupt(BUTTON_OUTPUT, buttonPress, FALLING);

  #ifdef DEBUG
  Serial.println("Start loop: ");
  #endif
}

void loop()
{

  server.handleClient();
  //TODO - this will overflow after 50 days, fix it
  if(millis() >= time_now + PERIOD)
  {
    #ifdef DEBUG
    Serial.println("Reading values:");
    #endif
    time_now += PERIOD;

    meas.Measure(htuNew);

    if(meas.index >= ARRAY_SIZE)
    {
      meas.SendMeasurement(existingChannelNumber, &existingApiKey[0]);
      meas.WipeMeasurements();

    }
  }

  if(buttonPressed)
  {
    EvaluateButtonPress();
  }

}

void EvaluateButtonPress()
{
    buttonPressed = false;
    if(mqSensorTripped)
    { 
      mqSensorTripped = false;
      attachInterrupt(MQ2_DIGITAL_OUTPUT, mq2Interrupt, FALLING);
      digitalWrite(BUZZER_CONTROL, LOW); 
    }
    else
    {
      unsigned long currentTime = millis();
      attachInterrupt(MQ2_DIGITAL_OUTPUT, mq2Interrupt, FALLING);
      //While 2 seconds have not elapsed
      while(!(millis() >= currentTime + 2000))
      {
        if(digitalRead(BUTTON_OUTPUT))
        {
          attachInterrupt(BUTTON_OUTPUT, buttonPress, FALLING);
          return;
        }
      }

      wm.erase();
      esp_restart();
    }

    attachInterrupt(BUTTON_OUTPUT, buttonPress, FALLING);
}


