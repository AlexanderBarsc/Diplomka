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

#define DEBUG
#define HTU21DF_I2CADDR (0x40)
#define HTU21DF_RESET (0xFE)

void readTemp();

// Web server running on port 80
WebServer server(80);
TwoWire I2CBME = TwoWire(0);
WiFiClient myClient;
hw_timer_t *My_timer = NULL;
WiFiManager wm;

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

boolean pirUpdate = false;
boolean eraseWifiConfig = false;
Measurement meas;
Adafruit_HTU21DF htu = Adafruit_HTU21DF();

char buffer[1024];

unsigned long myChannelNumber = 2463153;
const char * myWriteAPIKey = "AXMLMV4283OWJT1Z";



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

void getValues()
{
  StaticJsonDocument<50> jsonDocument;
  Serial.println("Get all values");
  jsonDocument.clear(); // Clear json buffer
  addTimeJsonObject(jsonDocument,"timestamp", meas.timestamp, "date");
  addJsonObject(jsonDocument,"audio", meas.audio, "raw");
  addJsonObject(jsonDocument,"gas", meas.gas, "raw");
  addJsonObject(jsonDocument,"photoTransistor", meas.photoTransistor, "raw");
  addTimeJsonObject(jsonDocument,"lastPirDetection", meas.pirDetection, "time");
  addJsonObject(jsonDocument,"temperature", meas.temperature, "°C");
  addJsonObject(jsonDocument,"relative humidity", meas.humidity, "%");

  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
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
  server.on("/getValues", getValues);
  server.on("/getDigitalPinState", getDigitalPinValue);
  server.on("/setDigitalPin", HTTP_POST, setDigitalPin);

  server.begin();
}

void IRAM_ATTR onTimer(){
digitalWrite(LED_CONTROL, !digitalRead(LED_CONTROL));
}

void IRAM_ATTR buttonPress()
{
  eraseWifiConfig = true;
}

/// @brief Interrupt which handles rising edge signals from PIR Module
/// @return
void IRAM_ATTR pirInterrupt()
{
#ifdef DEBUG
  Serial.println("PIR noticed something");
#endif
  pirUpdate = true;
  detachInterrupt(PIR_OUTPUT);
}

void setup()
{
  // put your setup code here, to run once:
  SetupPins();

  I2CBME.begin(I2C_SDA, I2C_SCL, 400000);
  htu.begin(&I2CBME);

  if(htu.begin(&I2CBME) == false)
  {
     Serial.println("HTU INIT FAILED");
     esp_restart();
  }

  Serial.begin(115200);
  // Inicialization delay
  delay(2000);

  attachInterrupt(PIR_OUTPUT, pirInterrupt, RISING);
  attachInterrupt(BUTTON_OUTPUT, buttonPress, FALLING);

  My_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 1000000, true);
  timerAlarmEnable(My_timer);

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

  timerAlarmDisable(My_timer);
  ThingSpeak.begin(myClient);

  *(volatile uint32_t *)(GPIO_OUT_REG) |= ((1 << LED_CONTROL));


  setupApi();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Start loop: ");
}

void loop()
{
  server.handleClient();

  struct tm timeinfo;
  getLocalTime(&timeinfo);
  char *timeStamp = asctime(&timeinfo);
  strcpy(meas.timestamp, timeStamp);

  #ifdef DEBUG
  Serial.println("Reading values:");
  #endif

  meas.audio = analogRead(MIC_OUTPUT);
  meas.gas = analogRead(MQ2_OUTPUT);
  meas.photoTransistor = analogRead(PHOTOTRAN_OUTPUT_AD);
  meas.temperature = htu.readTemperature();
  meas.humidity = htu.readHumidity(); 
  delay(500);

  if (pirUpdate)
  {
    strcpy(meas.pirDetection, timeStamp);
    pirUpdate = false;
    ThingSpeak.setField(1, meas.temperature);
    ThingSpeak.setField(2, meas.humidity);
    ThingSpeak.setField(3, meas.audio);
    ThingSpeak.setField(4, meas.gas);
    ThingSpeak.setField(5, meas.photoTransistor);
    ThingSpeak.setField(6, true);
    int result = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    Serial.println(result);
    attachInterrupt(PIR_OUTPUT, pirInterrupt, RISING);
  }

  if(eraseWifiConfig)
  {
    wm.erase();
    esp_restart();
  }
  
}
