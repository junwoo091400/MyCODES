//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

char ValidBuf[16];

void screen_setup(){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
}
void screen_loop(){
  sprintf(ValidBuf,"GPS: %c Alt: %c\n",GPS_Valid?'Y':'N',Altitude_Valid?'Y':'N');
  display.println(ValidBuf);
  display.display();
}

