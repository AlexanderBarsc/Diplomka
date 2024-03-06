#include <Arduino.h>
#include <Wire.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include "Adafruit_HTU21DF.h"
#include "ESP32Setup.h"
#include "ESP32WebCommunication.h"
#include "time.h"

#define DEBUG
#define HTU21DF_I2CADDR (0x40)
#define HTU21DF_RESET (0xFE)

void readTemp();

// Web server running on port 80
WebServer server(80);
StaticJsonDocument<1024> jsonDocument;

TwoWire I2CBME = TwoWire(0);


const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

boolean pirUpdate = false;
Measurement meas;
Adafruit_HTU21DF htu = Adafruit_HTU21DF();

char buffer[1024];

void handlePost() {
  if (server.hasArg("plain") == false) {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
  
  // Respond to the client
  server.send(200, "application/json", "{}");
}

void createJson(char *name, float value, char *unit) {  
  jsonDocument.clear();
  jsonDocument["name"] = name;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);  
}
 
void addJsonObject(const char *name, float value,const char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["name"] = name;
  obj["value"] = value;
  obj["unit"] = unit; 
}

void addTimeJsonObject(const char *name, char* str, const char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["name"] = name;
  obj["value"] = str;
  obj["unit"] = unit; 
}


void getValues() {
  Serial.println("Get all values");
  jsonDocument.clear(); // Clear json buffer
  addTimeJsonObject("timestamp", meas.timestamp, "date");
  addJsonObject("audio", meas.audio, "raw");
  addJsonObject("gas", meas.gas, "raw");
  addJsonObject("photoTransistor", meas.photoTransistor, "raw");
  addTimeJsonObject("lastPirDetection", meas.pirDetection, "time");

  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}

void getDigitalPinState() {

  if(server.args() == 0)
  {
    server.send(BAD_REQUEST, "text/plain", "No query arguments in URL");
    return;
  }
  
   String pinFromInput = server.arg(0);
   #ifdef DEBUG
    Serial.println(pinFromInput);
   #endif
   
   OutputPins pin = parseIntToOutputPin(pinFromInput.toInt());

   if(pin == UNDEFINED)
   {
    server.send(BAD_REQUEST, "text/plain", "Invalid pin selected");
    return;
   }
    
   jsonDocument.clear();
   JsonObject obj = jsonDocument.createNestedObject();
   obj["name"] = pin;
   obj["state"] = digitalRead(pin);

   serializeJson(obj, buffer); 
   server.send(OK, "application/json", buffer);
   return;
  
}
void setDigitalPin()
{
   if (server.hasArg("plain") == false) {
    //handle error here
  }

  StaticJsonDocument<200> doc;
  deserializeJson(doc,server.arg("plain"));

  // TODO - Works but maybe think about undefined behaviour
  uint8_t pinNumber = doc["pin"];
  uint8_t value = doc["value"];

  OutputPins pin = parseIntToOutputPin(pinNumber);

  if(pin == UNDEFINED)
  {
    server.send(BAD_REQUEST, "text/plain", "Invalid pin selected");
    return;
  }

  digitalWrite(pin, value);
  server.send(OK);
}


void setupApi() {
  server.on("/getValues", getValues);
  server.on("/getDigitalPinState", getDigitalPinState);
  server.on("/setDigitalPin", HTTP_POST, setDigitalPin);

  server.begin();
}

/// @brief Interrupt which handles rising edge signals from PIR Module
/// @return 
void IRAM_ATTR pirInterrupt() {
    #ifdef DEBUG
    Serial.println("PIR noticed something");
    #endif
    pirUpdate = true;
    detachInterrupt(PIR_OUTPUT);
}


void setup() {
  // put your setup code here, to run once:
  SetupPins();
  
  /*
  if(Wire.begin(I2C_SDA, I2C_SCL) == false)
  {
    Serial.println("I2C init failed");
    esp_restart();
  }
  */
  I2CBME.begin(I2C_SDA, I2C_SCL, 400000);
  htu.begin(&I2CBME);
  // Creation of own TwoWire object due to mismatched pins on the HTU21D module 

  Serial.begin(115200);
  // Inicialization delay
  delay(1500); 


  attachInterrupt(PIR_OUTPUT, pirInterrupt, RISING);


  WiFi.mode(WIFI_STA); 
  WiFiManager wm;
  bool res;

  res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      ESP.restart();
  } 
  else {  
      Serial.println("Connected to WiFi");
  }

  setupApi();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

}

void loop() {
  server.handleClient();

  struct tm timeinfo;
  getLocalTime(&timeinfo);
  char* timeStamp = asctime(&timeinfo);
  strcpy(meas.timestamp, timeStamp);

  if(pirUpdate)
  {
    strcpy(meas.pirDetection, timeStamp);
    pirUpdate = false;
    attachInterrupt(PIR_OUTPUT, pirInterrupt, RISING);
  }
  meas.audio = analogRead(MIC_OUTPUT);
  meas.gas = analogRead(MQ2_OUTPUT);
  meas.photoTransistor = analogRead(PHOTOTRAN_OUTPUT_AD);
  meas.temperature = htu.readTemperature();
  Serial.println(meas.temperature);  
  delay(100);

}

void readTemp()
{

  Wire.write(HTU21DF_RESET);
  Wire.beginTransmission(HTU21DF_I2CADDR);
  Wire.endTransmission();

  delay(20);

 uint8_t val = Wire.read();

  Serial.println(val);
  Serial.println(val == 0x2);

  uint8_t readTemp = 0xE3;

  Wire.write(readTemp);
  Wire.beginTransmission(HTU21DF_I2CADDR);
  Wire.endTransmission();
  delay(50);

  uint8_t buf[3];

  Wire.readBytes(buf, 3);

  for(int i = 0; i < 3; i++)
  {
    Serial.println(buf[i]);
  }

  uint16_t t = buf[0];
  t <<= 8;
  t |= buf[1] & 0b11111100;

   float temp = t;
  temp *= 175.72f;
  temp /= 65536.0f;
  temp -= 46.85f;

  /* Track the value internally in case we need to access it later. */
  Serial.println(temp);

}




