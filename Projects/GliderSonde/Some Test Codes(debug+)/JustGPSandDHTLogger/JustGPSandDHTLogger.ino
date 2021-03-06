#include "DHT22.h"
#include <SPI.h>
#include "SdFat.h"
#include <SoftwareSerial.h>

#define GPS Serial
const unsigned char UBLOX_INIT[] PROGMEM = {
  // Disable NMEA
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x24, // GxGGA off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x2B, // GxGLL off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x02,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x32, // GxGSA off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x39, // GxGSV off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x04,0x40, // GxRMC off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x05,0x00,0x00,0x00,0x00,0x00,0x01,0x05,0x47, // GxVTG off

  // Disable UBX
  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x17,0xDC, //NAV-PVT off
  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0xB9, //NAV-POSLLH off
  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x13,0xC0, //NAV-STATUS off

  // Enable UBX
  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x18,0xE1, //NAV-PVT on
  //0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x13,0xBE, //NAV-POSLLH on
  //0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x14,0xC5, //NAV-STATUS on

  // Rate
 // 0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x01,0x00,0x7A,0x12, //(10Hz)
 //0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A, //(5Hz)
  0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39 //(1Hz)
};

const unsigned char UBX_HEADER[] = { 0xB5, 0x62 };
const unsigned char NAV_PVT_clsid[]={0x01,0x07};
struct NAV_PVT {
  unsigned char cls;
  unsigned char id;
 unsigned short len;
  unsigned long iTOW;          // GPS time of week of the navigation epoch (ms)
  
  unsigned short year;         // Year (UTC) 
  unsigned char month;         // Month, range 1..12 (UTC)
  unsigned char day;           // Day of month, range 1..31 (UTC)
  unsigned char hour;          // Hour of day, range 0..23 (UTC)
  unsigned char minute;        // Minute of hour, range 0..59 (UTC)
  unsigned char second;        // Seconds of minute, range 0..60 (UTC)
  char valid;                  // Validity Flags <Bit2:validDate Bit1:validTime Bit0:fullyResolved(TimeofDay fully resolved)>
  unsigned long tAcc;          // Time accuracy estimate (UTC) (ns)
  long nano;                   // Fraction of second, range -1e9 .. 1e9 (UTC) (ns)
  unsigned char fixType;       
  // <GNSSfix Type> /0: no fix /1: dead reckoning only /2: 2D-fix /3: 3D-fix /4: GNSS + dead reckoning combined /5: time only fix 
  char flags;                  // Fix Status Flags <X1> Bitfield.
  char flags2;     // reserved<??>
  unsigned char numSV;         // Number of satellites used in Nav Solution !!!!!!!! 여기까지 26 Byte Check OK.
  
  long lon;                    // Longitude (deg)
  long lat;                    // Latitude (deg)
  long height;                 // Height above Ellipsoid (mm)
  long hMSL;                   // Height above mean sea level (mm)
  unsigned long hAcc;          // Horizontal Accuracy Estimate (mm)
  unsigned long vAcc;          // Vertical Accuracy Estimate (mm) !!!!!!!!!! 여기까지 50byte Check OK.
  long velN;                   // NED north velocity (mm/s)
  long velE;                   // NED east velocity (mm/s)
  long velD;                   // NED down velocity (mm/s)
  long gSpeed;                 // Ground Speed (2-D) (mm/s)
  long headMot;                // Heading of motion 2-D (deg) !!!! 여기까지 70 byte.
  
  unsigned long sAcc;          // Speed Accuracy Estimate
  unsigned long headAcc;    // Heading Accuracy Estimate <Both motion and vehicle>
  unsigned short pDOP;         // Position dilution of precision
  unsigned char reserved1[6];             // Reserved
  long headVeh; //heading of Vehicle <2D>?
  unsigned char reserved2[4];     // Reserved
  //NAV_PVT total 94 byte including cls&id. CHECKED(170301)
};

NAV_PVT pvt;

void calcChecksum(unsigned char* CK) {
  memset(CK, 0, 2);
  for (int i = 0; i < (int)sizeof(NAV_PVT); i++) {
    CK[0] += ((unsigned char*)(&pvt))[i];
    CK[1] += CK[0];
  }
}
boolean processGPS() {
  static int fpos = 0;
  static unsigned char checksum[2];
  const int payloadSize = sizeof(NAV_PVT);
//PORTD |= 1<<7;
  digitalWrite(7,HIGH);
  while ( GPS.available() ) {
    byte c = GPS.read();
    if ( fpos < 2 ) {
      if ( c == UBX_HEADER[fpos] )
        fpos++;
      else
        fpos = 0;
    }
    else {      
      if ( (fpos-2) < payloadSize )
        ((unsigned char*)(&pvt))[fpos-2] = c;
      fpos++;

      if ( fpos == (payloadSize+2) ) {
        calcChecksum(checksum);
      }
      else if ( fpos == (payloadSize+3) ) {
        if ( c != checksum[0] )
          fpos = 0;
      }
      else if ( fpos == (payloadSize+4) ) {
        fpos = 0;
        if ( c == checksum[1] ) {
          return true;
          digitalWrite(7,LOW);
        }
      }
      else if ( fpos > (payloadSize+4) ) {
        fpos = 0;
      }
    }
  }
  return false;
}

//#include"JL_printf.h"

//#include "U8glib.h"
//#include"U8g2lib.h"
//#include"JL_screenu8g.h"
///////////////////////////////////////
//Chipselect = 10 / SPI = 11,12,13 / DHT = 4, 5 / Serial = 0,1 / mySerial = 2,3 / DEBUGorWARNING = 7 / 
////////////////////////////////////////
boolean GPS_Valid = false;
boolean DHT_No1_Valid = false;
boolean DHT_No2_Valid = false;
//......................
unsigned long loop_timer = 0;
////////////////////////////////////////////
DHT22 DHT_No1(4);
DHT22 DHT_No2(5);
byte currentDHT = 0;
////////////////////////////////////////
#include"JL_sd.h"

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
  Serial.begin(38400);
  DDRD |= DEBUG_PIN_PORT;
  ///////////
  delay(1000);
  mySerial.println("Gonna send CONFIG...");
  delay(1000);
  for(unsigned int i = 0; i < sizeof(UBLOX_INIT); i++) {                        
    GPS.write( pgm_read_byte(UBLOX_INIT+i) );
    delay(5);
  }

  ///////////
  byte fix_count = 0;
  while(false){
    if(processGPS()){
      if(pvt.fixType==3) {fix_count+=1;}
    }
    if(fix_count>10) break;
    mySerial.print(pvt.fixType);
    mySerial.write(',');
    mySerial.println(fix_count);
    delay(1000);
  }
  
  if(!Sd_init()){DEBUG_PIN_ON;while(true){delay(100);}}//init fail.

  delay(2000);
  loop_timer = micros();
}
////////////////////////////////////////////////////////////////////
void loop() {
  GPS_Valid = false;
  DHT_No1_Valid = false;
  DHT_No2_Valid = false;
  while(micros()-loop_timer < 1000000);//1s.
  loop_timer = micros();

  mySerial.println("Loop~");
  
  ExportHeader();
  
  if(loop_timer>120000000){//Forcing to stop!!!! 120 seconds
    Sd_close();
    byte Val = 0;
    while(true){///////////////////////////////Infinite Loop
      Val++;
      analogWrite(7,Val);//DEBUG PIN. LED.
      delay(10);
    }
  }

  if(processGPS()){ GPS_Valid = true; mySerial.println("processGPS OK");}
  ExportGPS();
  
  if(currentDHT){if(DHT_No1.readData()==DHT_ERROR_NONE) DHT_No1_Valid = true; mySerial.print("DHT1 Suc.");}
  else{if(DHT_No2.readData()==DHT_ERROR_NONE) DHT_No2_Valid = true; mySerial.print("DHT2 Suc.");}
  currentDHT=(currentDHT+1)%2;//0,1 toggle
  ExportDHT();
}
