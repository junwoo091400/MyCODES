//Proof of Concept code for HX711 Sd-logging at 80Hz.
//2019.07.17. Wednesday. Junwoo HWANG
//in Pi-ville, lol this is painful.

//2019.11.08. Comment: This was the software that was used for measuring Pressure & Thrust of a Rocket Motor.

#include<SPI.h>
#include<SD.h>
#include "HX711.h" // https://github.com/bogde/HX711

File file;
HX711 loadcell;

// Adjustment settings
long LOADCELL_OFFSET = 0;
float LOADCELL_SCALE = 1.0f; // https://stackoverflow.com/questions/16844657/sublime-text-select-all-instances-of-a-variable-and-edit-variable-name
#define DEFAULT_LOADCELL_SCALE 81.61247f // Haha

#define SD_CS_PIN 10

// 1. HX711 circuit wiring
#define LOADCELL_DOUT_PIN 2
#define LOADCELL_SCK_PIN 3

// LED
#define GREEN_LED_HIGH 7
#define GREEN_LED_LOW 6
#define RED_LED_HIGH 5
#define RED_LED_LOW 4

//#define DEBUG //!!!!!!!!!!!!!!!!!!!!!!!

/////////////////////////////////////////////////////////////////
char print_buffer[40] = {};//0-em-all.
long loadcell_value = 0;
float thrust_inGram = 0.0f;
/////////////////////////////////////////////////////////////////

///

void blinkLed(int count, int interval = 666, void (*func)(int state) = &greenLed) {
  for(int i=0; i<count; i++) {
    func(HIGH);
    delay(interval / 2);
    func(LOW);
    delay(interval / 2);
  }
}

void greenLed(int state) {
  digitalWrite(GREEN_LED_HIGH, state);
}

void redLed(int state) {
  digitalWrite(RED_LED_HIGH, state);
}

////

typedef enum {
  SETUP_FAIL_LOADCELL = 0,
  SETUP_FAIL_SD_CANT_BEGIN,
  SETUP_FAIL_SD_CANT_OPEN_FILE,
  SETUP_FAIL_SD_CANT_WRITE,
  SETUP_FAIL_MODE_COUNT, // count.
} SETUP_FAIL_MODE_e;

#define BIG_INT 30000
void setup_FAIL(SETUP_FAIL_MODE_e mode);//func. prototype auto-place issue from " Arduino 1.7.10"... https://arduino.stackexchange.com/questions/28133/cant-use-enum-as-function-argument

void setup_FAIL(SETUP_FAIL_MODE_e mode) {
  switch(mode) {
    case SETUP_FAIL_LOADCELL:
      blinkLed(BIG_INT, 2000, redLed);//
      break;
    case SETUP_FAIL_SD_CANT_BEGIN:
    case SETUP_FAIL_SD_CANT_OPEN_FILE:
      blinkLed(BIG_INT, 1000, redLed );
      break;
    case SETUP_FAIL_SD_CANT_WRITE:
      blinkLed(BIG_INT, 500, redLed);
      break;
  };
}

//
void led_setup() {
  pinMode(GREEN_LED_HIGH, OUTPUT);
  pinMode(GREEN_LED_LOW, OUTPUT);
  pinMode(RED_LED_HIGH, OUTPUT);
  pinMode(RED_LED_LOW, OUTPUT);

  //set up GROUNDS.
  digitalWrite(GREEN_LED_LOW, LOW);
  digitalWrite(RED_LED_LOW, LOW);
}

void loadcell_setup() {
  Serial.println(F("Starting Loadcell Init."));
  // Init.
  loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, 128);//128 gain for Channel A.

  delay(10);
  //detect Failure
  if(!loadcell.wait_ready_retry(10, 1)) {// 10 retries. 1ms delay between.
    setup_FAIL(SETUP_FAIL_LOADCELL);
  }

  // TARE.
  serialFlush();
  Serial.println(F("Loadcell Ready to Tare? [Enter]"));
  #ifdef DEBUG
  while(Serial.available() < 1);
  #endif
  Serial.println(F("Beginning Tare..."));

  loadcell.set_scale();//default. 1
  loadcell.tare(255);//tare! target time 5 seconds.

  LOADCELL_OFFSET = loadcell.get_offset();//long.

  // SHOW OFFSET.
  Serial.print(F("Offset: "));
  Serial.print(LOADCELL_OFFSET);
  Serial.println(F(""));

  // Set Scale
  serialFlush();
  /*
  #ifdef DEBUG
  Serial.println(F("Loadcell Ready to Scale an object? Enter the weight. [Number##Enter]"));
  while(Serial.available() < 1);
  delay(100);
  
  float real_weight = Serial.parseInt();
  Serial.print(F("GOT: "));
  Serial.println(real_weight, 2);
  
  Serial.println(F("Beginning Scaling..."));
  float rawScaled = loadcell.get_units(255);
  Serial.print(F("Raw Scale Value(with scale=1): "));
  Serial.println(rawScaled,2);

  LOADCELL_SCALE = rawScaled / real_weight;//87.95
  Serial.print(F("New Divider Value: "));
  Serial.println(LOADCELL_SCALE, 2);
  #endif// https://stackoverflow.com/questions/47045629/is-it-legal-to-use-elif-with-ifdef
  */
  
  loadcell.set_scale(DEFAULT_LOADCELL_SCALE);
  
  LOADCELL_SCALE = loadcell.get_scale();//get it.

  loadcell.power_up();
}

void sd_setup(){
  char fileString[12] = {};//https://forum.arduino.cc/index.php?topic=269288.0
  int count = 0;
  
  pinMode(SD_CS_PIN, OUTPUT);
  
  if(!SD.begin(SD_CS_PIN))
    setup_FAIL(SETUP_FAIL_SD_CANT_BEGIN);
  else
    Serial.println(F("SD card is ready to use."));

  sprintf(fileString, "%03d.csv", count);
  do {
   sprintf(fileString, "%03d.csv", count++);//https://www.quora.com/What-is-the-difference-between-2d-and-02d-in-C-language-What-is-the-logic
   if(count == 1001) {
    break;
   }
  } while (SD.exists(fileString));

  file = SD.open(fileString, FILE_WRITE);
  
  #ifdef DEBUG
  Serial.println(fileString);
  #endif
  
  if(!file)
    setup_FAIL(SETUP_FAIL_SD_CANT_OPEN_FILE);
  else
    Serial.println(F("File write pipe opening successful"));
}

void sd_write_header() {
  file.print(F("Load-cell Offset, Scale\n"));
  sprintf(print_buffer, "%ld, ", loadcell.get_offset());
  dtostrf(loadcell.get_scale(), 2, 2, &print_buffer[strlen(print_buffer)]);///yaas. Stop that overflow int :)
  strcat(print_buffer, "\n");
  
  bool result = file.write(print_buffer, strlen(print_buffer));//okay.
  #ifdef DEBUG
  Serial.print(F("File Header Write "));
  #endif
  if(result){
    #ifdef DEBUG
    Serial.print(F("Sccessful\n"));
    #endif
  }
  else {
    #ifdef DEBUG
    Serial.print(F("Un-successful\n"));
    #endif
    setup_FAIL(SETUP_FAIL_SD_CANT_WRITE);
  }
}

void setup() {
  Serial.begin(115200);
  loadcell_setup();
  led_setup();
  sd_setup();
  sd_write_header();
  delay(100);
  Serial.println(F("Proceeding into the Main Loop!"));
}

void loop() {
  if(loadcell_loop()) {
    sd_loop();
    led_loop();
  }
}

/////////////////////////////////////////////////////////////////////
void led_loop() {
  static int counter = 0;
  static int state = LOW;
  if(++counter >= 50) {//approx 550ms.
    greenLed(state);
    state = !state;//invert.
    counter = 0;
  }
}

void sd_loop() {
  static byte counter = 0;
  unsigned long cur = micros();

  //TEMP!
  int presRead = analogRead(A0);//A0 pin.
  
  sprintf(print_buffer, "%10ld, %7ld, %d\n", cur, loadcell_value, presRead);
  //dtostrf(thrust_inGram, 2, 2, &print_buffer[strlen(print_buffer)]);//append. theoratical 32(?) max strlen.
  //https://www.arduino.cc/en/Reference/UsingAVR
  //strcat(print_buffer, "\n");
  
  bool write_result = false;
  write_result = file.write(print_buffer, strlen(print_buffer));//https://www.allaboutcircuits.com/projects/reading-and-writing-files-from-an-sd-card-with-an-arduino/
  if(!write_result) {
    setup_FAIL(SETUP_FAIL_SD_CANT_WRITE);
  }
  counter += 1;
  
  #ifdef DEBUG
  unsigned long cur2 = micros();
  Serial.print(F("SD Write/Flush: "));
  Serial.print(cur2 - cur);
  Serial.print(F(", "));
  #endif
  if(counter >= 10) {
    file.flush();
    #ifdef DEBUG
    Serial.print(micros() - cur2);
    #endif
    counter = 0;//init.
    /*
    Serial.println(thrust_inGram);
    Serial.println(print_buffer);
    */
  }
  #ifdef DEBUG
  Serial.println(F("\n"));
  #endif
}

///
#define LOADCELL_INTERVAL_US 12500 // 12.5ms. 80Hz.

bool loadcell_loop() {
  static unsigned long last_read = 0;
  unsigned long cur = micros();
  if(cur - last_read > LOADCELL_INTERVAL_US && loadcell.is_ready()) {
    loadcell_value = loadcell.read();//raw 24bit.
    thrust_inGram = (loadcell_value - LOADCELL_OFFSET) / LOADCELL_SCALE;
    return true;
  }
  return false;//not ready, or func. called to quick.
}

void serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}
