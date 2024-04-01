#include <Arduino.h>
#include <Wire.h>
#include "ESP32Setup.h"
#include "Adafruit_HTU21DF.h"
#include "ThingSpeak.h"
#include "Measurement.h"


//TODO - tohle bude zadavat uzivatel

    float Measurement::CalculateAverage(uint16_t arr[])
    {
        float sum = 0;
        if (index == 0)
        {
            return 0.0f;
        }

        #ifdef DEBUG
            Serial.println("Now printing out calculations for average result for uint16_t array");
        #endif
        
        for (int i = 0; i < index; i++)
        {
            #ifdef DEBUG
            Serial.println(arr[i]);
            #endif
            sum += arr[i];
        }

        float result = sum/index;

        #ifdef DEBUG
        Serial.println(sum);
        Serial.println(result);
        #endif

        return result;
    }

    float Measurement::CalculateAverage(float arr[])
    {
        float sum = 0;
        if (index == 0)
        {
            return 0.0f;
        }

        for (int i = 0; i < index; i++)
        {
            sum += arr[i];
        }

        return sum/index;
        
    }
    Measurement::Measurement()
    {
        Serial.begin(115200);
        #ifdef DEBUG
        Serial.println("Entered Measurement constructor");
        #endif
     
    }

   
    void Measurement::Measure(HTU21D &htu)
    {

        if (index >= ARRAY_SIZE)
        {
            return;
        }

        audio[index] = analogRead(MIC_OUTPUT);
        temperature[index] = htu.ReadTemp();
        humidity[index] = htu.ReadHumidity();
        gas[index] = analogRead(MQ2_OUTPUT);
        photoResistor[index] = analogRead(PHOTOTRAN_OUTPUT_AD);

        if (wasMovementDetected == false)
        {
            wasMovementDetected = digitalRead(PIR_OUTPUT);
        }

        
        #ifdef DEBUG
        Serial.print("We are now measuring at index: ");
        Serial.println(index);
        Serial.print("Audio: ");
        Serial.println(audio[index]);
        Serial.print("Temperature: ");
        Serial.println(temperature[index]);
        Serial.print("Humidity: ");
        Serial.println(humidity[index]);
        Serial.print("Gas: ");
        Serial.println(gas[index]);
        Serial.print("Photoresistor: ");
        Serial.println(photoResistor[index]);
        Serial.print("Was movement detected: ");
        Serial.println(wasMovementDetected);
        #endif
        

        index++;
    }

    void Measurement::WipeMeasurements()
    {
        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            audio[i] = 0;
            temperature[i] = 0;
            humidity[i] = 0;
            gas[i] = 0;
            photoResistor[i] = 0;
        }

        wasMovementDetected = false;
        index = 0;
    }

    void Measurement::SendMeasurement(unsigned long myChannelNumber, const char* myWriteAPIKey)
    {

        ThingSpeak.setField(1, CalculateAverage(temperature));
        ThingSpeak.setField(2, CalculateAverage(humidity));
        ThingSpeak.setField(3, CalculateAverage(audio));
        ThingSpeak.setField(4, CalculateAverage(gas));
        ThingSpeak.setField(5, CalculateAverage(photoResistor));
        ThingSpeak.setField(6, wasMovementDetected);
        int result = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

        Serial.println("Result of sending data to ThingSpeak");
        Serial.println(result);
    }
