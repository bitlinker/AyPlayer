#include "PsgDecoder.h"
#include "AyControl.h"

//Offset Number of byte Description
//+ 0 3 Identifier 'PSG'
//+ 3 1 Marker вЂњEnd of TextвЂќ(1Ah)
//+ 4 1 Version number
//+ 5 1 Player frequency(for versions 10 + )
//+ 6 10 Data
//
//Data вЂ” РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕСЃС‚Рё РїР°СЂ Р±Р°Р№С‚РѕРІ Р·Р°РїРёСЃРё РІ СЂРµРіРёСЃС‚СЂ.
//РџРµСЂРІС‹Р№ Р±Р°Р№С‚ вЂ” РЅРѕРјРµСЂ СЂРµРіРёСЃС‚СЂР°(РѕС‚ 0 РґРѕ 0x0F), РІС‚РѕСЂРѕР№ вЂ” Р·РЅР°С‡РµРЅРёРµ.
//Р’РјРµСЃС‚Рѕ РЅРѕРјРµСЂР° СЂРµРіРёСЃС‚СЂР° РјРѕРіСѓС‚ Р±С‹С‚СЊ СЃРїРµС†РёР°Р»СЊРЅС‹Рµ РјР°СЂРєРµСЂС‹ : 0xFF, 0xFE РёР»Рё 0xFD
//0xFD вЂ” РєРѕРЅРµС† РєРѕРјРїРѕР·РёС†РёРё.
//0xFF вЂ” РѕР¶РёРґР°РЅРёРµ 20 РјСЃ.
//0xFE вЂ” СЃР»РµРґСѓСЋС‰РёР№ Р±Р°Р№С‚ РїРѕРєР°Р·С‹РІР°РµС‚ СЃРєРѕР»СЊРєРѕ СЂР°Р· РІС‹Р¶РґР°С‚СЊ РїРѕ 80 РјСЃ.

static const uint8_t CMD_INTERRUPT = 0xFF;
static const uint8_t CMD_INTERRUPT_MULTIPLE = 0xFE;
static const uint8_t CMD_END = 0xFD;

PsgDecoder::PsgDecoder()
  : mSkipTicks(0)
{
}

// TODO: dbg
//uint8_t buf[2048];
//uint16_t bufPos = 2048;

// TODO: buffered stream
//inline uint8_t readBuffered(File &f) {
//  if (bufPos == 2048) {
//    Serial.println("fread");
//    f.read(buf, 2048);
//    bufPos = 0;
//    if (!f.available())
//    {
//      Serial.println("PSG: eof");
//      return 0L; // TODO: stop here
//    }
//  }
//  return buf[bufPos++];
//}

// 20ms delay...
bool PsgDecoder::tick(File &f, AyControl& out)
{
  if (mSkipTicks)
  {
    --mSkipTicks;
    return true;
  }

  while (true)
  {
    // TODO: proper eof check
    if (!f.available())
    {
      Serial.println("PSG: eof");
      return false;
    }
    
    uint8_t cmd = f.read();    
    if (cmd == CMD_END)
    {
      Serial.println("PSG: end cmd");
      return false;
    }
    
    else if (cmd == CMD_INTERRUPT)
    {
      //Serial.println("PSG: interrupt");
      return true;
    }
    else if (cmd == CMD_INTERRUPT_MULTIPLE)
    {
      //Serial.println("PSG: multiple interrupts");
      mSkipTicks = 4 * f.read() - 1;
      return true;
    }
    else if (cmd < 16)
    {
      uint8_t value = f.read();
      if (cmd == 13 && value == 255) continue;  // Envelope bugfix
      out.write(cmd, value);
    }
    else
    {
      Serial.print("PSG: Unknown cmd: ");
      Serial.println(cmd);
      return false;
    }
  }
}


