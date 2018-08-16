#include "DHT22.h"
#include <SPI.h>
#include "SdFat.h"
#include <Wire.h>
#include <SoftwareSerial.h>

//#include"JL_printf.h"

//#include "U8glib.h"
//#include"U8g2lib.h"
//#include"JL_screenu8g.h"
///////////////////////////////////////
//Chipselect = 10 / SPI = 11,12,13 / DHT = 4, 5 / Serial = 0,1 / mySerial = 2,3 / DEBUGorWARNING = 7 / 
////////////////////////////////////////
boolean GPS_Valid = false;
boolean Altitude_Valid = false;
boolean DHT_No1_Valid = false;
boolean DHT_No2_Valid = false;
//......................
unsigned long loop_timer = 0;
boolean Max_Altitude_Reached = false;
boolean Min_Altitude_Cutoff = false;
int32_t Min_Altitude_Value = 500;//5m.
int32_t Max_Altitude_Value = 50000;//[cm]
////////////////////////////////////////////
DHT22 DHT_No1(4);
DHT22 DHT_No2(5);
byte currentDHT = 0;
////////////////////////////////////////
#include"JL_msp.h"
#include"JL_sd.h"
#include"JL_PPM.h"

////////////////////////////////////////////////////
//bool DEBUG_ENABLE = false;
#define DEBUG_PIN_PORT B10000000//DEBUG PIN at PD7!
#define DEBUG_PIN_ON PORTD |= DEBUG_PIN_PORT;
#define DEBUG_PIN_OFF PORTD &= ~DEBUG_PIN_PORT;
///////////////////////////////////////////////////
SoftwareSerial mySerial(2,3);//rx,tx

//
void setup() {
  mySerial.begin(9600);
  Serial.begin(115200);
  DDRD |= DEBUG_PIN_PORT;
  ///////////
  PPM_init();
  if(!Sd_init()){DEBUG_PIN_ON;while(true){delay(100);}}//init fail.
  ///////////
  PPM_Data[4] = 2000;//AUX1 channel up to ARM.
  byte Fix_count = 0;
  
  while(true){
    mspRequest(MSP_RAW_GPS);
    delay(20);
    if(mspGet(MSP_RAW_GPS)&&MspmsgIn.msploc.fixType==1) Fix_count++;
    else{ DEBUG_PIN_ON; delay(180);DEBUG_PIN_OFF; }
    if(Fix_count++>=10) break;
    mySerial.println(Fix_count);
  }
  delay(2000);
  loop_timer = micros();
}
////////////////////////////////////////////////////////////////////
void loop() {
  GPS_Valid = false;
  Altitude_Valid = false;
  DHT_No1_Valid = false;
  DHT_No2_Valid = false;
  while(micros()-loop_timer < 1000000);//1s.
  loop_timer = micros();

  ExportHeader();
  mspRequest(MSP_ALTITUDE);
  delay(20);
  if(mspGet(MSP_ALTITUDE)){ Altitude_Valid = true; mySerial.print("Alt Suc.");}
  ExportAlt();
  
  if(!Max_Altitude_Reached&&Altitude_Valid&&MspmsgIn.mspalt.alt>Max_Altitude_Value){
    Max_Altitude_Reached = true;
    //Lossen up the String.
    PPM_Data[5]=2000;//AUX2 UP = RTHmode
  }
  if(Max_Altitude_Reached&&Altitude_Valid&&MspmsgIn.mspalt.alt<Min_Altitude_Value||loop_timer>100000000){//Forcing to stop!!!!
    Sd_close();
    Min_Altitude_Cutoff = true;
    byte Val = 0;
    while(true){///////////////////////////////Infinite Loop
      Val++;
      analogWrite(7,Val);//DEBUG PIN. LED.
      delay(10);
    }
  }
  
  mspRequest(MSP_RAW_GPS);
  delay(20);
  if(mspGet(MSP_RAW_GPS)&&MspmsgIn.msploc.fixType==1){ GPS_Valid = true; mySerial.print("GPS Suc.");}
  ExportGPS();
  if(currentDHT){if(DHT_No1.readData()==DHT_ERROR_NONE) DHT_No1_Valid = true; mySerial.print("DHT1 Suc.");}
  else{if(DHT_No2.readData()==DHT_ERROR_NONE) DHT_No2_Valid = true; mySerial.print("DHT2 Suc.");}
  currentDHT=(currentDHT+1)%2;//0,1 toggle
  ExportDHT();
}
