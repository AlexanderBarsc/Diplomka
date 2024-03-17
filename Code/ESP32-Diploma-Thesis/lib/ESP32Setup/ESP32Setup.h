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

#define I2C_SDA 22
#define I2C_SCL 21

enum OutputPins
{
  UNDEFINED = -1,
  LED = 13,
  BUZZER = 27
};

typedef struct measurement {
    char timestamp[50];
    float temperature;
    float humidity;
    uint16_t audio;
    uint16_t gas;
    uint16_t photoTransistor;
    char pirDetection[50];
    
} Measurement;

void SetupPins();
OutputPins parseIntToOutputPin(int value);

