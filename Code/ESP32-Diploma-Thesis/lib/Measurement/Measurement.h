#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_HTU21DF.h"
#include "ThingSpeak.h"
#include "HTU21D.h"

#define ARRAY_SIZE 100

class Measurement {
private:
    float temperature[ARRAY_SIZE];
    float humidity[ARRAY_SIZE];
    uint16_t audio[ARRAY_SIZE];
    uint16_t gas[ARRAY_SIZE];
    uint16_t photoResistor[ARRAY_SIZE];
    boolean wasMovementDetected;
    Adafruit_HTU21DF htu;

    float CalculateAverage(uint16_t arr[]);
    float CalculateAverage(float arr[]);

public:
    uint16_t index;
    Measurement();
    void Measure(HTU21D &htu);
    void WipeMeasurements();
    void SendMeasurement(unsigned long myChannelNumber, const char* myWriteAPIKey);

};

#endif // MEASUREMENT_H