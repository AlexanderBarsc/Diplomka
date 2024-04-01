#ifndef HTU21D_H
#define HTU21D_H

#include <esp_err.h>
#include "ESP32Setup.h"
/** Default I2C address for the HTU21D. */
#define HTU21D_I2CADDR (0x40)
/** Read temperature register. */
#define HTU21D_READTEMP (0xE3)
/** Read humidity register. */
#define HTU21D_READHUM (0xE5)
/** Write register command. */
#define HTU21D_WRITEREG (0xE6)
/** Read register command. */
#define HTU21D_READREG (0xE7)
/** Reset command. */
#define HTU21D_RESET (0xFE)

#define I2C_TIMEOUT 1000
#define HTU21D_RESET_INTERVAL 15
#define TEMP_MEASURING_INTERVAL 35
#define HEATER_BIT 2


class HTU21D{
public:
  boolean Begin(uint8_t i2c_num, int8_t sda, int8_t scl, uint32_t clk_speed);
  boolean Reset();
  uint8_t ReadUserRegister();
  boolean TurnOnOffHeater(boolean value);
  float ReadTemp();
  float ReadHumidity();
private:
  uint8_t _i2c_num;
};

#endif