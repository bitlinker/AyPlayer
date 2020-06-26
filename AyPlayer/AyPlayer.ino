#include <BlockDriver.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <sdios.h>
#include <SysCall.h>

#include <ESP8266WiFi.h>
//#include <ESP8266WiFiAP.h>
//#include <ESP8266WiFiGeneric.h>
//#include <ESP8266WiFiMulti.h>
//#include <ESP8266WiFiScan.h>
//#include <ESP8266WiFiSTA.h>
//#include <ESP8266WiFiType.h>
//#include <WiFiClient.h>
//#include <WiFiClientSecure.h>
//#include <WiFiServer.h>
//#include <WiFiUdp.h>
#include "GyverButton.h"

#include "AyControl.h"

/*
   The WeMos Micro SD Shield uses:
   D5, D6, D7, D8, 3V3 and G

   The shield uses SPI bus pins:
   D5 = CLK
   D6 = MISO
   D7 = MOSI
   D8 = CS

   I2C:
   D1 - SDA
   D2 - SCL

   Buttons:
   D0
   A0
   D3
   D4
*/

#include <Wire.h>

// SD
#include <SPI.h>
#include <SD.h>

#include "PsgDecoder.h"
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(LED_BUILTIN);

const int PIN_CS = D8;



static AyControl ayControl;

static GButton btnLeft(D3, HIGH_PULL, NORM_OPEN);
static GButton btnRight(D4, HIGH_PULL, NORM_OPEN);
//static GButton btnUp(D0, HIGH_PULL, NORM_OPEN);
//static GButton btnDown(1, HIGH_PULL, NORM_OPEN);

bool playFile(File& file)
{
  Serial.print("Playing: ");
  Serial.println(file.name());

  // Send reset
  ayControl.reset();
  ayControl.setFreq(AyControl::FREQ_1_777MHZ);

  file.seek(16); // Skip header TODO: read; in decoder

  PsgDecoder decoder;

  // TODO: use timer
  //  timer1_attachInterrupt(onTimerISR);
  //    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  //    timer1_write(600000); //120000 us

  while (true)
  {
    if (Serial.available() && Serial.read() == 'N') break;

    unsigned long startTm = micros();
    ayControl.start();
    if (!decoder.tick(file, ayControl)) break;
    unsigned long endTm = micros();
    ayControl.end();
    delayMicroseconds(20000L - (endTm - startTm)); // Wait
    //yield(); // TODO: needed?
  }
}

void parseDir(File& dir)
{
  Serial.print("Checking: ");
  Serial.println(dir.name());
  while (true)
  {
    //yield();
    File entry = dir.openNextFile();
    if (!entry) {
      Serial.println("Next is empty");
      break;
    }

    if (entry.isDirectory())
    {
      Serial.println("Dir");
      parseDir(entry);
    }
    else
    {
      Serial.print("File: ");
      Serial.println(entry.name());
      //      if (String(entry.name()).endsWith("psg"))  // TODO: add to list; case insensitive compare
      //      {
      //        //playFile(entry);
      //      }
    }
    entry.close();
  }
}

void printDirectory(File& dir, int numTabs) {
  int colcnt = 0;
  while (true) {
    File entry =  dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    if (numTabs > 0) {
      for (uint8_t i = 0; i <= numTabs; i++) {
        Serial.print('\t');
      }
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      //Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else
    {
      // files have sizes, directories do not
      Serial.print("\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void setup() {
  WiFi.mode(WIFI_OFF);

  Serial.begin(115200);
  Serial.println("Serial port started.");

  ayControl.init();
  ayControl.reset();

  //  delay(500);
  //
  //  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  //
  //  delay(2000);
  //
  //  display.clearDisplay();
  //  display.drawPixel(0, 0, WHITE);
  //  display.display();
  //  delay(2000);
  //  display.setCursor(0, 0);
  //  display.setTextSize(1);
  //  display.setTextColor(WHITE);
  //  display.println("AY");
  //  display.display();
  //
  //  if (!SD.begin(PIN_CS)) {
  //    Serial.println("SD card not found!");
  //    return;
  //  }
  //  Serial.println("SD card initialization done.");
  //
  //  File root = SD.open("/");
  //  parseDir(root);
  //  root.close(); // TODO

  if (!SD.begin(PIN_CS)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  File root = SD.open("/");
  root.rewindDirectory();

  printDirectory(root, 0);

  Serial.println("done!");
}

void loop() {
  // put your main code here, to run repeatedly:
  btnLeft.tick();
  btnRight.tick();
  //btnUp.tick();
  //btnDown.tick();

  if (btnLeft.isSingle()) Serial.println("btnLeft");
  if (btnRight.isSingle()) Serial.println("btnRight");
  //if (btnUp.isSingle()) Serial.println("btnUp");
  //if (btnDown.isSingle()) Serial.println("btnDown");
}

