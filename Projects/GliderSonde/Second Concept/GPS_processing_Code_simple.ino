/*
 Connections:
 GPS TX -> Arduino 0 (disconnect to upload this sketch)
 GPS RX -> Arduino 1
 Screen SDA -> Arduino A4
 Screen SCL -> Arduino A5
*/
#include <Wire.h>
//#include<SoftwareSerial.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

//SoftwareSerial GPS = SoftwareSerial(3,5);
#define GPS Serial

#define PAGE_TIME 0
#define PAGE_GPS_LatLon 1
#define PAGE_SPEED 2
#define PAGE_Accuracy 3
#define PAGE_Interval 4
#define TOTAL_PAGE_NUM 5

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
  0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A, //(5Hz)
 // 0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39 //(1Hz)
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
char tempChecksum[2];
bool processGPS() {
  static int fpos = 0;
  static unsigned char checksum[2];
  const int payloadSize = sizeof(NAV_PVT);
//PORTD |= 1<<7;
  digitalWrite(9,HIGH);
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
        tempChecksum[0]=checksum[0];
        tempChecksum[1]=checksum[1];
      }
      else if ( fpos == (payloadSize+3) ) {
        if ( c != checksum[0] )
          fpos = 0;
      }
      else if ( fpos == (payloadSize+4) ) {
        fpos = 0;
        if ( c == checksum[1] ) {
          return true;
          digitalWrite(9,LOW);
        }
      }
      else if ( fpos > (payloadSize+4) ) {
        fpos = 0;
      }
    }
  }
  return false;
}

unsigned long lastScreenUpdate = 0;
char* spinner = "/-\\|";
byte screenRefreshSpinnerPos = 0;
byte gpsUpdateSpinnerPos = 0;
unsigned long Loop_Interval;

void updateScreen(int where){
  display.clearDisplay();//why not use 'clear'?
  display.setCursor(0,0);
  display.print("GPS");
  display.print(spinner[gpsUpdateSpinnerPos]);
  display.print(" SCREEN");
  display.print(spinner[screenRefreshSpinnerPos]);
  switch(where){
    case PAGE_TIME:
    display.println(" TIME");
    display.print("Y");
    display.print(pvt.year);
    display.print(" M");
    display.print(pvt.month);
    display.print(" D");
    display.print(pvt.day);
    display.print(" H");
    display.print(pvt.hour);
    display.print(" M");
    display.print(pvt.minute);
    display.print(" S");
    display.println(pvt.second);
    break;
    case PAGE_GPS_LatLon:
    display.println(" GPS_LL");
    display.print("Lat ");
    display.println(pvt.lat);
    display.print("Lon ");
    display.println(pvt.lon);
    display.print("Fix ");
    display.print(pvt.fixType);
    display.print(" NumSV ");
    display.println(pvt.numSV);
    break;
    case PAGE_SPEED:
    display.println(" SPEED");
  /*  display.print("veN ");
   display.println(pvt.velN);
    display.print("veE ");
    display.println(pvt.velE);
    display.print("veDown ");
    display.println(pvt.velD);
   */
    display.print("gSpeed ");
    display.println(pvt.gSpeed);
    display.print("Heading ");
    display.println(pvt.headMot);
    break;
    case PAGE_Accuracy: 
    display.println(" Accu");
    display.println(pvt.tAcc);
    display.println(pvt.hAcc);
    display.println(pvt.vAcc);
    break;
    case PAGE_Interval://'check' 'checksum' state!
    display.print("Interval: ");
    display.println(Loop_Interval);
    break;
  };
  display.display();//update
}

void setup() 
{
  GPS.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();//clear
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  /////////////////////////////////////////////
  display.setCursor(0,0);
  display.println("GPS_simplecode");
  display.println("Sending Config.");//MAX 21 letters.
  display.display();//update screen
  
  // send configuration data in UBX protocol
  for(unsigned int i = 0; i < sizeof(UBLOX_INIT); i++) {                        
    GPS.write( pgm_read_byte(UBLOX_INIT+i) );
    delayMicroseconds(209); // simulating a 38400baud pace (or less), otherwise commands are not accepted by the device.
  }
  display.println("Upload done!");
  display.display();
  //////////////////////////////////////////////
 pinMode(9,OUTPUT);
}
int page;
void loop() {
  
  if ( processGPS() ) {
    gpsUpdateSpinnerPos = (gpsUpdateSpinnerPos + 1) % 4;
  }

  Loop_Interval = millis()-lastScreenUpdate;
  if (Loop_Interval > 100 ) {
    lastScreenUpdate+=Loop_Interval;
    page = (int)(TOTAL_PAGE_NUM*analogRead(0)/1023.0);//0~4
    screenRefreshSpinnerPos = (screenRefreshSpinnerPos + 1) % 4;
    updateScreen(page);
  }
}

