#include "DHT22.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

DHT22 DHT_Sensor(7);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();//clear
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.clearDisplay();
}

void loop() {
  delay(2100);
  display.clearDisplay();
  display.setCursor(0,0);
  switch(DHT_Sensor.readData()){
    case DHT_ERROR_NONE:
    display.print(DHT_Sensor.getTemperatureC());
    display.println(" C");
    display.print(DHT_Sensor.getHumidity());
    display.println(" %");
      break;
    case DHT_ERROR_CHECKSUM:
    display.println("CHECKSUM");
      break;
    case DHT_BUS_HUNG:
    display.println("BUS_HUNG");
      break;
    case DHT_ERROR_NOT_PRESENT:
    display.println("NOT_PRESENT");
      break;
    case DHT_ERROR_ACK_TOO_LONG:
      display.println("ACK_TOO_LONG");
      break;
    case DHT_ERROR_SYNC_TIMEOUT:
      display.println("SYNC_TIMEOUT");
      break;
    case DHT_ERROR_TOOQUICK:
      display.println("TOO QUICK");
      break;
  }
  display.display();
}
