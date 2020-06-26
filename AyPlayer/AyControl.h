#include <Wire.h>



class AyControl {
  public:
    static const uint8_t  FREQ_1MHZ         = 1;
    static const uint8_t  FREQ_1_777MHZ     = 2;
    static const uint8_t  FREQ_2MHZ         = 3;
    static const uint8_t  FREQ_3_2MHZ       = 4;
    static const uint8_t  FREQ_4MHZ         = 5;

  private:
    static const uint8_t I2C_ADDRESS        = 0x8;
    static const uint64_t I2C_SPEED         = 400000L;

    static const uint8_t NUM_REGISTERS      = 16;

    uint16_t mValues[NUM_REGISTERS] = {0};

  public:
    void init() {
      Wire.begin(D1, D2); // TODO: pins
      Wire.setClock(I2C_SPEED);
    }

    void setFreq(uint8_t value) {
      Wire.beginTransmission(I2C_ADDRESS);
      write(0xFE, value);
      Wire.endTransmission();
    }

    void reset()
    {
      Wire.beginTransmission(I2C_ADDRESS);
      write(0xFF, 0x00);
      Wire.endTransmission();
    }

    void start() {
      for (uint8_t i = 13; i < NUM_REGISTERS; ++i) {
        mValues[i] = 0xFFFF;
      }
    }

    void end() {
      Wire.beginTransmission(I2C_ADDRESS);
      for (int8_t i = NUM_REGISTERS - 1; i >= 0; --i) {
        uint16_t data = mValues[i];
        if (data == 0xFFFF) continue;
        Wire.write(i);
        Wire.write(static_cast<uint8_t>(data));
      }
      Wire.endTransmission();
    }

    void write(uint8_t reg, uint8_t data)
    {
      mValues[reg] = data;
    }
};
