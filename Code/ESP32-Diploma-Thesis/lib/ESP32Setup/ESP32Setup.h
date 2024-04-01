#ifndef ESP32SETUP_H
#define ESP32SETUP_H

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
#define MQ2_DIGITAL_OUTPUT 23
#define PIR_OUTPUT 34
#define EEPROM_SIZE 22
#define INITIAL_DELAY 2000

#define I2C_SDA 22
#define I2C_SCL 21

#define DEBUG

enum OutputPins
{
  UNDEFINED = -1,
  LED = 13,
  BUZZER = 27
};

void SetupPins();
OutputPins parseIntToOutputPin(int value);

#endif
