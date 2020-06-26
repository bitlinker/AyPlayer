/**
   Receives commands from I2C or Seraial interfaces and plays them at AY--3-8910
   Command format:
    - 8 bit register
    - 8 bit value
   There is a special register value (REGISTER_RESET) used to reset the chip
   Board is Arduino Nano
*/
#include <Wire.h>

static const long     SERIAL_CLOCK      = 115200;
static const uint8_t  MAX_AY_REGISTERS  = 16;
static const uint8_t  REGISTER_RESET    = 0xFF;
static const uint8_t  REGISTER_FREQ     = 0xFE;

static const uint8_t  FREQ_1MHZ         = 1;
static const uint8_t  FREQ_1_777MHZ     = 2;
static const uint8_t  FREQ_2MHZ         = 3;
static const uint8_t  FREQ_3_2MHZ       = 4;
static const uint8_t  FREQ_4MHZ         = 5;

static const uint8_t  I2C_ADDRESS       = 0x8;
static const uint64_t I2C_SPEED         = 100000L;

static const uint8_t  PIN_BC1           = 0b00100000;
static const uint8_t  PIN_BC2           = 0b00001000;
static const uint8_t  PIN_BDIR          = 0b00010000;
static const uint8_t  PIN_RESET         = 0b00000100;

static const uint8_t  BUS_DELAY_US   = 3;

inline void writeData(uint8_t data)
{
  PORTC = ((data & 0xF0) >> 4);
  PORTD = ((data & 0x0F) << 4);
}

inline void cmdInactive()
{
  PORTB = (PIN_BC2 | PIN_RESET);
  delayMicroseconds(BUS_DELAY_US);
}

inline void cmdLatchAddr()
{
  PORTB = (PIN_BC1 | PIN_BC2 | PIN_BDIR | PIN_RESET);
}

inline void cmdLatchData()
{
  PORTB = (PIN_BC2 | PIN_BDIR | PIN_RESET);
}

inline void writeRegisterAY(uint8_t reg, uint8_t data)
{
  if (reg == 13 && data == 0xFF) return;  // Envelope bugfix
  
  cmdInactive();
  writeData(reg);
  cmdLatchAddr();
  cmdInactive();

  cmdInactive();
  writeData(data);
  cmdLatchData();
  cmdInactive();
}

void resetAY()
{
  PORTB = PIN_BC2;
  delayMicroseconds(BUS_DELAY_US);
  PORTB = PIN_BC2 | PIN_RESET;
}

void initPins()
{
  DDRB = 0xFF;
  PORTB = 0;

  DDRC = 0xFF;
  PORTC = 0;

  DDRD = 0xFF;
  PORTD = 0;
}

void initClock ()
{
  cli();

  // Clock output:
  pinMode(3, OUTPUT);
  TCCR2A = 0x23; // (COM2B1) Clear OC2B on Compare Match | (WGM20,21,22) Fast PWM: Top OCRA
  TCCR2B = 0x09; // (WGM22) | (CS20) No prescaling

  setFreq(FREQ_1_777MHZ);

  sei();
}

void setFreq(uint8_t val) {
  switch (val) {
    case 1:  // 1 MHz
      OCR2A = 15;
      OCR2B = 7;
      break;
    case 2:  // 1.777(7) MHz
      OCR2A = 8;
      OCR2B = 3;
      break;
    case 3:  // 2 MHz
      OCR2A = 7;
      OCR2B = 3;
      break;
    case 4:  // 3.2 MHz
      OCR2A = 4;
      OCR2B = 1;
      break;
    case 5:  // 4 MHz
      OCR2A = 3;
      OCR2B = 1;
      break;
  }
}

void wireIsrHandler() {
  // Nothing here, but need handler for Wire lib to work...
}

void wireHandler() {
  while (Wire.available() >= 2)
  {
    uint8_t reg = Wire.read();
    uint8_t val = Wire.read();
    process(reg, val);
  }
}

void serialRequestHandler() {
  while (Serial.available() >= 2) {
    uint8_t reg = Serial.read();
    uint8_t val = Serial.read();
    process(reg, val);
  }
}

void process(uint8_t reg, uint8_t val) {
  cli();

  if (reg <= MAX_AY_REGISTERS)
  {
    writeRegisterAY(reg, val);
  } else {
    switch (reg) {
      case REGISTER_FREQ:
        setFreq(val);
        break;
      case REGISTER_RESET:
        resetAY();
        break;
    }
  }

  sei();
}

void initWire() {
  Wire.begin(I2C_ADDRESS);
  Wire.setClock(I2C_SPEED);
  Wire.onReceive(wireIsrHandler);
}

void setup() {
  initPins();
  resetAY();
  initWire();
  Serial.begin(SERIAL_CLOCK);
  initClock();
}

void loop() {
  serialRequestHandler();
  wireHandler();
}
