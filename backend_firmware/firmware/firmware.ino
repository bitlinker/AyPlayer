static const long     SERIAL_CLOCK      = 115200;
static const uint8_t  MAX_AY_REGISTERS  = 16;
static const uint8_t  REGISTER_RESET    = 255;

static const uint8_t  PIN_BC1     = 0b00100000;
static const uint8_t  PIN_BC2     = 0b00001000;
static const uint8_t  PIN_BDIR    = 0b00010000;
static const uint8_t  PIN_RESET   = 0b00000100;

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

void writeRegisterAY(uint8_t reg, uint8_t data)
{
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
  // This gives 2Mhz on pin 9...
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 5;   // toggle after counting to 6
  TCCR1A |= (1 << COM1A0);   // Toggle OC1A on Compare Match.
  TCCR1B |= (1 << WGM12);    // CTC mode
  TCCR1B |= (1 << CS10);     // clock on, no pre-scaler
}

void setup() {
  initPins();
  resetAY();
  initClock();
 
  Serial.begin(SERIAL_CLOCK); 
}

void loop() {
  if (Serial.available() > 1) { // 2 bytes command
    uint8_t reg = Serial.read();
    uint8_t val = Serial.read();

    if (reg <= MAX_AY_REGISTERS)
    {
      writeRegisterAY(reg, val);
    }
    else if (reg == REGISTER_RESET)
    {
      resetAY();
    }
  }
}
