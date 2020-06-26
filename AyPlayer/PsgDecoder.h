#pragma once 

#include <Arduino.h>
#include <SD.h>

class AyControl;

class PsgDecoder
{
  public:
    PsgDecoder();
    bool tick(File &f, AyControl& out);
  
  private:
    uint16_t mSkipTicks;
};


