#include<SoftwareSerial.h>

SoftwareSerial serial = SoftwareSerial(3,5);

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
 // 0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x13,0xBE, //NAV-POSLLH on
  //0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x14,0xC5, //NAV-STATUS on

  // Rate
  //0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x01,0x00,0x7A,0x12, //(10Hz)
  0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A, //(5Hz)
  //0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39 //(1Hz)
};

const unsigned char UBX_HEADER[] = { 0xB5, 0x62 };
/*
struct NAV_POSLLH {
  unsigned char cls;
  unsigned char id;
  unsigned short len;
  unsigned long iTOW;
  long lon;
  long lat;
  long height;
  long hMSL;
  unsigned long hAcc;
  unsigned long vAcc;
};

NAV_POSLLH posllh;
*/

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

bool processGPS() {
  static int fpos = 0;
  static unsigned char checksum[2];
  const int payloadSize = sizeof(NAV_PVT);

  while ( serial.available() ) {
    byte c = serial.read();
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
        if ( c != checksum[0] ){
          fpos = 0;/*
       Serial.print("Doesn't equal(checksum,c): ");
       Serial.print(checksum[0],HEX);
       Serial.print(" , ");
       Serial.println(c,HEX);*/
          }
      }
      else if ( fpos == (payloadSize+4) ) {
        fpos = 0;
        if ( c == checksum[1] ) {/*
          Serial.print("Winner_1!(checksum,c)");
          Serial.print(checksum[1],HEX);
       Serial.println(c,HEX);*/
          return true;
        }
      }
      else if ( fpos > (payloadSize+4) ) {
        fpos = 0;
      }
    }
  }
  return false;
}

unsigned long time[2];

void setup(){
  Serial.begin(9600);
  serial.begin(9600);

  for(unsigned int i = 0; i < sizeof(UBLOX_INIT); i++) {                        
    serial.write( pgm_read_byte(UBLOX_INIT+i) );
    delay(5); // simulating a 38400baud pace (or less), otherwise commands are not accepted by the device.
  }
  
  Serial.println("Done Uploading!");
time[0] = millis();
}
int loop_counter = 0;

void loop(){
if(processGPS()){
   /*Serial.print("iTOW:");      Serial.print(posllh.iTOW);
    Serial.print(" lat/lon: "); Serial.print(posllh.lat/10000000.0f); Serial.print(","); Serial.print(posllh.lon/10000000.0f);
    Serial.print(" height: ");  Serial.print(posllh.height/1000.0f);
    Serial.print(" hMSL: ");    Serial.print(posllh.hMSL/1000.0f);
    Serial.print(" hAcc: ");    Serial.print(posllh.hAcc/1000.0f);
    Serial.print(" vAcc: ");    Serial.print(posllh.vAcc/1000.0f);
    Serial.println();
    */
    loop_counter++;
Serial.print("iTOW: ");
Serial.println(pvt.iTOW);
Serial.println(pvt.lat);
Serial.println(pvt.lon);
}
  if(loop_counter==100){
    time[1] = millis()-time[0];
    Serial.print("100 Actions in ");
    Serial.print(time[1]/1000);
    Serial.println(" seconds.");
    delay(10000);
  }
}

