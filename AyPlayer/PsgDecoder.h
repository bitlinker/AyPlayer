#include <Arduino.h>
#include <SD.h>

class PsgDecoder
{
  public:
    PsgDecoder();
    bool tick(File &f, Stream& out);
  
  private:
    uint16_t mSkipTicks;
};


