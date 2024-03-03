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

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
boolean pirUpdate = false;


enum OutputPins
{
  UNDEFINED = -1,
  LED = 13,
  BUZZER = 27
};

void printLocalTime();

OutputPins parseIntToOutputPin(int value);

    
typedef struct measurement {
    float temperature;
    float humidity;
    uint16_t audio;
    char* pirDetection;
    
} Measurement;

Measurement meas;

Adafruit_HTU21DF htu = Adafruit_HTU21DF();


// Web server running on port 80
WebServer server(80);
StaticJsonDocument<1024> jsonDocument;

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
 
void addJsonObject(char *name, float value, char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["name"] = name;
  obj["value"] = value;
  obj["unit"] = unit; 
}

void addTimeJsonObject(char *name, char* str, char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["name"] = name;
  obj["value"] = str;
  obj["unit"] = unit; 
}


void getValues() {
  Serial.println("Get all values");
  jsonDocument.clear(); // Clear json buffer
  addJsonObject("audio", meas.audio, "raw");
  addJsonObject("temp", meas.temperature, "°C");
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
    server.send(BAD_REQUEST, "text/plain", "Invalid pin selected " + pin);
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
  server.on("/setStatus", HTTP_POST, handlePost);
  server.on("/getDigitalPinState", getDigitalPinState);
  server.on("/setDigitalPin", HTTP_POST, setDigitalPin);
 
  // start server
  server.begin();
}

//TODO - asi prasárna tohle dávat do přerušení ale co už 
void IRAM_ATTR pirInterrupt() {
    Serial.println("PIR noticed something");
    pirUpdate = true;
    detachInterrupt(PIR_OUTPUT);
    
}



void setup() {
  // put your setup code here, to run once:
  SetupPins();
  
  // Creation of own TwoWire object due to mismatched pins on the HTU21D module 
  TwoWire I2CBME = TwoWire(0);
  I2CBME.begin(I2C_SDA, I2C_SCL, 400000);
  Serial.begin(115200);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  if(!htu.begin(&I2CBME))
  {
    Serial.println("HTU21D module not found");
    esp_restart();
  }

  attachInterrupt(PIR_OUTPUT, pirInterrupt, RISING);

 
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // it is a good practice to make sure your code sets wifi mode how you want it.

  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  // wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("Connected...yeey :)");
  }


  setupApi();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();

  if(pirUpdate)
  {
    struct tm timeinfo;
    getLocalTime(&timeinfo);
    meas.pirDetection = asctime(&timeinfo);
    Serial.println(meas.pirDetection);
    pirUpdate = false;
    attachInterrupt(PIR_OUTPUT, pirInterrupt, RISING);
  }

  meas.audio = analogRead(MIC_OUTPUT);
 // meas.temperature = htu.readTemperature(); 
  delay(DEFAULT_DELAY);

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

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}


