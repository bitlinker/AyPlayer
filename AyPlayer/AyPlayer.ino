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

/*
 * The WeMos Micro SD Shield uses:
 * D5, D6, D7, D8, 3V3 and G
 *
 * The shield uses SPI bus pins:
 * D5 = CLK
 * D6 = MISO
 * D7 = MOSI
 * D8 = CS
 */
 
#include <SPI.h>
#include <SD.h>
#include "SoftwareSerial.h"
#include "PsgDecoder.h"
//#include <Adafruit_SSD1306.h>

//Adafruit_SSD1306 display(LED_BUILTIN);

// TODO: spi?
//D1
SoftwareSerial ayControl(SW_SERIAL_UNUSED_PIN, 5); // RX, TX

const int PIN_CS = D8;

bool playFile(File& file)
{
  Serial.print("Playing: ");
  Serial.println(file.name());

  // Send reset
  ayControl.write((uint8_t)0xFF);
  ayControl.write((uint8_t)0x00);

  file.seek(16); // Skip header TODO: read; in decoder

  PsgDecoder decoder;
  while (true)
  {
    if (Serial.available() && Serial.read() == 'N') break;
    
    unsigned long startTm = millis();
    if (!decoder.tick(file, ayControl)) break;
    unsigned long endTm = millis();
    delay(20 - (endTm - startTm)); // Wait
  }
}

void parseDir(File& dir)
{
  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry) break;

    if (entry.isDirectory())
    {
      parseDir(entry);
    }
    else
    {
      Serial.println(entry.name());
      if (String(entry.name()).endsWith("PSG"))  // TODO: add to list
      {
        playFile(entry);
      }
    }
    entry.close();
    yield();
  }
}

void setup() {
  WiFi.mode(WIFI_OFF);
    
  Serial.begin(115200);   
  ayControl.begin(115200); // TODO: clock
  
  delay(500);

//  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
//
//  display.display();
//  delay(2000);
//
//  // Clear the buffer.
//  display.clearDisplay();
//
//  // draw a single pixel
//  display.drawPixel(10, 10, WHITE);
//  // Show the display buffer on the hardware.
//  // NOTE: You _must_ call display after making any drawing commands
//  // to make them visible on the display hardware!
//  display.display();
//  delay(2000);
//  display.clearDisplay();
//  
  if (!SD.begin(PIN_CS)) {
    Serial.println("SD card not found!");
    return;
  }
  Serial.println("SD card initialization done.");

  File root = SD.open("/");
  parseDir(root);
  root.close();
}

void loop() {
  // put your main code here, to run repeatedly:

}

