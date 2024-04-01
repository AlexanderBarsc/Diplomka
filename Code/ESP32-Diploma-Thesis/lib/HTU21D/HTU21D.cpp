#include "Arduino.h"
#include "Wire.h"
#include "HTU21D.h"
#include "esp32-hal-i2c.h"

uint8_t _i2c_num;

boolean HTU21D::Begin(uint8_t i2c_num, int8_t sda, int8_t scl, uint32_t clk_speed)
{
    esp_err_t result = i2cInit(i2c_num, sda, scl, clk_speed);
    boolean resetResult;
  
    if (result != ESP_OK)
    {
        #ifdef DEBUG
        Serial.println("I2C_FAIL");
        #endif
        return false;
    }
    _i2c_num = i2c_num;
    #ifdef DEBUG
    Serial.println("Now executing reset of HTU21D: ");
    #endif

    if(!Reset())
    {
        #ifdef DEBUG
        Serial.println("Reset was unsuccessful: ");
        #endif

        return false;
    }

    return true;
}

uint8_t HTU21D::ReadUserRegister()
{
    uint8_t cmd = HTU21D_READREG;
    uint8_t readInput = 0;
    size_t readBytes = 0;

    i2cWrite(_i2c_num, HTU21D_I2CADDR, &cmd, 1, I2C_TIMEOUT);
    i2cRead(_i2c_num, HTU21D_I2CADDR, &readInput, 1, I2C_TIMEOUT, &readBytes);

    return readInput;
}

boolean HTU21D::TurnOnOffHeater(boolean value)
{
    uint8_t userRegisterValue = 0;
    size_t readBytes;
    uint8_t buf[2];

    if (value)
    {
        int i = 0;
        while (i++ < 10)
        {
            userRegisterValue = ReadUserRegister();
            if (userRegisterValue &= (1 << HEATER_BIT))
            {
                #ifdef DEBUG
                Serial.println("Bit is set to 1, returning... ");
                #endif

                return true;
            }

            buf[0] = HTU21D_WRITEREG;
            userRegisterValue |= (1 << HEATER_BIT);
            buf[1] = userRegisterValue;
            i2cWrite(_i2c_num, HTU21D_I2CADDR, buf, 2, I2C_TIMEOUT);
        }

    }
    else
    {
        int i = 0;
        while (i++ < 10)
        {
            userRegisterValue = ReadUserRegister();
            if ((userRegisterValue &= (1 << HEATER_BIT)) == 0)
            {
                #ifdef DEBUG
                Serial.println("Bit is set to 0, returning... ");
                #endif

                return true;
            }

            buf[0] = HTU21D_WRITEREG;
            userRegisterValue &= ~(0 << HEATER_BIT);
            buf[1] = userRegisterValue;
            i2cWrite(_i2c_num, HTU21D_I2CADDR, buf, 2, I2C_TIMEOUT);
        }

    }

    return false;
}

boolean HTU21D::Reset()
{
    uint8_t cmd = HTU21D_RESET;
    uint8_t readInput = 0;
    size_t readBytes = 0;

    i2cWrite(_i2c_num, HTU21D_I2CADDR, &cmd, 1, I2C_TIMEOUT);
    delay(HTU21D_RESET_INTERVAL);

    return ReadUserRegister() == 0x2;
}

float HTU21D::ReadTemp()
{
    uint8_t cmd = HTU21D_READTEMP;
    size_t readBytes;
    uint8_t buf[3];

    i2cWrite(0, 0x40, &cmd, 1, 1000);
    delay(TEMP_MEASURING_INTERVAL);
    i2cRead(0, 0x40, buf, 3, 1000, &readBytes);

    uint16_t t = buf[0];
    t <<= 8;
    t |= buf[1] & 0b11111100;

    // 3rd byte is the CRC
    float temp = t;
    temp *= 175.72f;
    temp /= 65536.0f;
    temp -= 46.85f;

    return temp;
}

float HTU21D::ReadHumidity()
{
    uint8_t cmd = HTU21D_READHUM;
    size_t readBytes;
    uint8_t buf[3];

    i2cWrite(0, 0x40, &cmd, 1, 1000);
    i2cRead(0, 0x40, buf, 3, 1000, &readBytes);

    uint16_t h = buf[0];
    h <<= 8;
    h |= buf[1] & 0b11111100;

    float hum = h;
    hum *= 125.0f;
    hum /= 65536.0f;
    hum -= 6.0f;

    return hum;
}





