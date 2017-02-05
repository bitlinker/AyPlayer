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
 
//#include <SPI.h>
//#include <SD.h>
#include <Adafruit_SSD1306.h>


Adafruit_SSD1306 display(LED_BUILTIN);

//const int chipSelect = D8;
//File myFile;

void setup() {
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)

  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  // draw a single pixel
  display.drawPixel(10, 10, WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  display.display();
  delay(2000);
  display.clearDisplay();
  
//  if (!SD.begin(chipSelect)) {
//    Serial.println("initialization failed!");
//    return;
//  }
//  Serial.println("initialization done.");

//  // print the type of card
//  Serial.print("\nCard type: ");
//  switch (card.type()) {
//    case SD_CARD_TYPE_SD1:
//      Serial.println("SD1");
//      break;
//    case SD_CARD_TYPE_SD2:
//      Serial.println("SD2");
//      break;
//    case SD_CARD_TYPE_SDHC:
//      Serial.println("SDHC");
//      break;
//    default:
//      Serial.println("Unknown");
//  }

//  myFile = SD.open("music/kuku.psg");
//  if (myFile) {
//    Serial.println("kuku:");
//
//    // read from the file until there's nothing else in it:
//    int cnt = 0;
//    while (myFile.available()) {
//      myFile.read();
//      ++cnt;      
//    }
//    // close the file:
//    myFile.close();
//
//    Serial.print("Size:");
//    Serial.println(cnt);
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening test.txt");
//  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
