#include <SPI.h>
#include "SdFat.h"
const uint8_t chipSelect = SS;
#define FILE_BASE_NAME "Data"

//=========================
// File system object.
SdFat sd;

// Log file.
SdFile file;
//=============!===========

//==============================================================================
// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))
//------------------------------------------------------------------------------

void setup(){
  Serial.begin(115200);
  const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
  char fileName[13] = FILE_BASE_NAME "00.csv";

  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    sd.initErrorHalt();}

  if (BASE_NAME_SIZE > 6) {
    error("FILE_BASE_NAME too long");}

  while (sd.exists(fileName)) {
    if (fileName[BASE_NAME_SIZE + 1] != '9') {
      fileName[BASE_NAME_SIZE + 1]++;
    } else if (fileName[BASE_NAME_SIZE] != '9') {
      fileName[BASE_NAME_SIZE + 1] = '0';
      fileName[BASE_NAME_SIZE]++;
    } else {
      error("Can't create file name");
    }
  }
  if (!file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) {
    error("file.open");
  }
  DDRD |= B10000000;
  int counter = 0;
  float DATA[10][3] = {};
  for(int i=0;i<sizeof(DATA)/sizeof(float);i++){
    ((float*)DATA)[i] = analogRead(i%6)/1023.0;
  }
  Serial.println("DONE");
  Serial.println(fileName);
  while(counter++<=10000){
    if(counter%1000==0) Serial.println(counter);
    PORTD|=B10000000;
  for(int i=0;i<sizeof(DATA)/sizeof(float);i++){
    file.print(((float*)DATA)[i]);
    }
   file.println();
   PORTD&=B01111111;
  }
//
  if (!file.sync() || file.getWriteError()) {
    error("write error");
  }
//

  //////
  file.close();//AT THE LAST....
  //////
  }
void loop(){
  
}

