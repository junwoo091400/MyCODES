/////////////////<DESCRIPTION>/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * First 진지 MAIN_PROCESSOR Sketch for my GliderSonde project by Junwoo HWANG.
 * <Ver 0.0.1>===============
 * 1)SENSORs list
 * -DHT 22(Temp & Humidity)
 * -GY80
 *  -Accelerometer(ADXL345,0x53)
 *  -Gyroscope(L3G4200D,0x69)
 *  -Barometer(BMP085,0x77)
 *  -Magnetometer(HMC5883L,0x1E)
 * -NEO M8N GPS Module(Serial Communication)
 * 
 * 2)Loggers list
 * -Micro SD Card Module(5V compatible,SPI)
 * -Micro SD Card(2G)
 * 
 * 3)Processors list
 * -Arduino Pro Mini
 * 
 * 4)Running 
 * 
 * [WHAT IT DOES]:
 * 1.Reads in Data from GPS, DHT, GY80
 * 2.Decides which direction & Angle it should be at.
 * 3.Calculates it's attitude and apply gyro_stabilization
 * 4.Log Temp&Humidity and Time and so on... to the MicroSDCard!
 * 5.Check for any Malfunction in the system
 * 6.Measures Battery Voltage.
 * ==========================
 */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/////////////////<LIBRARIES>////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include<Wire.h>
#include<SPI.h>
#include<DHT22.h>
#include"SdFat.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/////////////////<VARIABLES>/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//0.Universal Variables
unsigned long loop_timer = 0;//start time of the loop(at each stage) : VERY GLOBAL!
bool Gyro_Updated = false;
bool Mag_Updated = false;
bool Acc_Updated = false;
bool Baro_Updated = false;
bool Gps_Updated = false;
bool Dht_Updated = false;

bool Gps_Fixed = false;

bool Gyro_Error = false;

// .Attitude State Variables
float Vehicle_Roll = 0;
float Vehicle_Pitch = 0;
//float Vehicle_Yaw = 0;
float Vehicle_Heading = 0;

float Vehicle_Acc_Roll = 0;
float Vehicle_Acc_Pitch = 0;
//float Vehicle_Acc_Yaw = 0;

float Vehicle_Battery_Voltage = 0;

float Pid_Kp_Roll = 0;
float Pid_Ki_Roll = 0;
float Pid_Kd_Roll = 0;
float Pid_d_mem_Roll = 0;
float Pid_i_mem_Roll = 0;
float Pid_setpoint_Roll = 0;
int Pid_Output_Roll = 0;
int Pid_max_Roll = 400;

float Pid_Kp_Pitch = 0;
float Pid_Ki_Pitch = 0;
float Pid_Kd_Pitch = 0;
float Pid_d_mem_Pitch = 0;
float Pid_i_mem_Pitch = 0;
float Pid_setpoint_Pitch;
int Pid_Output_Pitch = 0;
int Pid_max_Pitch = 400;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/////////////////<SENSORS>////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//0.Universal Sensor Variables.

#define GYRO_ADDRESS 0x69//address of gyro(L3G4200D,SD0pin(4) held HIGH)
#define MAG_ADDRESS 0x1E//address of magnetometer(HMC5883L)
#define ACC_ADDRESS 0x53//address of accelerometer(ADXL345,ALT ADDRESS(12) held LOW)
#define BARO_ADDRESS 0x77//address of barometer(BMP085)

//0.Universal Sensor-related Functions

bool WriteByte(byte address,byte reg,byte data){
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(data);
  if(!Wire.endTransmission())//let's not give the stop sign for fun!!!
  return true;//successfully written
  else
  return false;
}

byte ReadByte(byte address,byte reg){//passes '1 Byte' of data from 'reg' from I2C device with 'address'
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission(false);//keep connection(read mode)
  Wire.requestFrom(address,1);
  while(!Wire.available());//wati until '1 byte' is recieved.
  return Wire.read();
}

bool Sensor_Present_Check(byte address){
  Wire.beginTransmission(address & 0x7F);//Clear the 7th index Bit.
  byte c = Wire.endTransmission();
  if(c==0)
    return true;//success.
  else
    return false;//NACK on transmit of address OR other error..
}

bool Sensors_Init(){
  //1.see if sensors are present.
  if(!Sensor_Present_Check(GYRO_ADDRESS))
    return false;
   if(!Sensor_Present_Check(MAG_ADDRESS))
    return false;
   if(!Sensor_Present_Check(ACC_ADDRESS))
    return false;

   //2. Set correct Values for each register
   

   //3. Calibrate Sensor.
   Gyro_getCalibrationData();
    
   return true;//everything right, return true!
}

///////////////////////////////////////////////////////////////////////

//1.GyroScope Code
#define GYRO_CALIBRATE_COUNT 1000 // DONT, PUT, SEMICOLON, AFTER MACRO. DONT DONT DONT! You wasted my 30 minute !! 1703061735
#define GYRO_THRESHOLD_MULTIPLE 4//Determines Threshold Value

float Gyro_dps_per_digit = 0.0175;//0.0175 degree/s = 1 digit.
int Gyro_RAW[3];//X,Y,Z value straight from the sensor.
int Gyro_Cal[3];//calibration data
float Gyro_dps[3];//final result
int Gyro_Threshold[3];//threshold for Gyro_RAW
int Gyro_Cal_count = 0;

float Dir[3]={0,0,0};

bool Gyro_Init(){
  if(!WriteByte(GYRO_ADDRESS,0x23,B10010000))//<CTRL_REG4> set FS10 bit(5,4) = 0,1. AND BDU bit(7) =1<BLOCK DATA UPDATE>. (500 dps scale.)
      return false;
   if(!WriteByte(GYRO_ADDRESS,0x20,B10001111))//<CTRL_REG1> set PD bit(3) to 1.(normal/sleep mode) + DataRate 400Hz.
      return false;
   if(!WriteByte(GYRO_ADDRESS,0x27,B00000000))//This, must be the default value, but Logic Analyzer tells me it is '11111111'.
      return false;
}

void Gyro_readData(){
  //while(!ReadByte(GYRO_ADDRESS,0x27)&(1<<3));//wait until <STATUS_REG> 'ZYXDA(x,y,z-axis new data available' flag turns 0;(in the loop, if set to 0.<not ready>)
  Wire.beginTransmission(GYRO_ADDRESS);
  Wire.write(0x28|1<<7);//Read from <OUT_X_L>. AND, MSB for the 'auto-increment' OR ELSE, auto_increment won't happen and Slave will keep giving the same data!
  Wire.endTransmission(false);//sending address&write_bit(0) completed. FALSE for keeping connection.
  Wire.requestFrom(GYRO_ADDRESS,6);//read 6 bytes, then give 'stop'
  while(Wire.available()<6);//wait until all data arrives
   for(int i=0;i<3;i++){
    Gyro_RAW[i] = Wire.read()|Wire.read()<<8;//RAW data reading...
    if(Gyro_Cal_count==GYRO_CALIBRATE_COUNT)//Only apply calibration value after calibration.
    Gyro_dps[i]=(abs(Gyro_RAW[i])>Gyro_Threshold[i])?((Gyro_RAW[i]-Gyro_Cal[i])*Gyro_dps_per_digit):(0);//if Inside Threshold, return 0.
    }
}

void Gyro_getCalibrationData(){
  long SUM[3]={};//place to store sum of all data
  unsigned long Squared[3]={};//place to store all squared data, at GYRO_CALIBRATE_COUNT==1000, each squared shouldn't exceed: (2^32-1)/1000.
  //Which means that each number shouldn't exceed 2072.43. and that is, 36.267 deg/s. So I consider this safe for now.
 //!!!!!!!!!!!!!!!! Doing ={} Ensures that 'SUM' and 'Squared' all set to 0. But to even ensure, I am going to reset them AGAIN.
 for(int i=0;i<3;i++){SUM[i] = 0; Squared[i] = 0;}
 //Because this gave me so much pain in the ass. Almost another HALF a hour,, 1703061754...
  for(Gyro_Cal_count=0;Gyro_Cal_count<GYRO_CALIBRATE_COUNT;Gyro_Cal_count++){
     Gyro_readData();
     for(int j=0;j<3;j++){//for each axis
        SUM[j] += Gyro_RAW[j];
        Squared[j]+=Gyro_RAW[j]*Gyro_RAW[j];
     }//for each axis
   }//Calibration Read/Save Process
   
   for(byte j=0;j<3;j++){
        Gyro_Cal[j]=SUM[j]/GYRO_CALIBRATE_COUNT;//int type
        Gyro_Threshold[j] = abs(sqrt(Squared[j]/GYRO_CALIBRATE_COUNT-Gyro_Cal[j]*Gyro_Cal[j])*GYRO_THRESHOLD_MULTIPLE);//(+) Positive Value
        // sqrt {sigma_squared/totalNum - Mean^2 } * CONSTANT
        }
}

///////////////////////////////////////////////////////////////////////

//2.Magnetometer Code

byte Mag_GN = 1;//GAIN setting
int Mag_RAW[3] = {};//Raw output data from Magnetometer RANGE: '0xF800(-2048) ~ 0x07FF(2047)'
unsigned int Mag_selfTestResult[3];//Positive Self Test Bias on each Axis

bool Mag_Init(){
  
}

bool Mag_Identification(){
  //Checks 3 Identification Registers of HMC5883L to confirm no errors.
  if(ReadByte(MAG_ADDRESS,0x0A)!='H')
    return false;
  if(ReadByte(MAG_ADDRESS,0x0B)!='4')
    return false;
  if(ReadByte(MAG_ADDRESS,0x0C)!='3')
    return false;
  return true;
}

bool Mag_SelfTest(){
  //Initiates Self test on Magnetometer to validate it's accuracy
  
}
//Earth's magnetic Field Strength varies from 0.25 ~ 0.65 Gauss.

void Mag_readData(){
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.write(0x03);//X_MSB Register
  Wire.endTransmission(false);
  Wire.requestFrom(MAG_ADDRESS,6);
  while(Wire.available()<6);
  Mag_RAW[0] = Wire.read()<<8|Wire.read();//X
  Mag_RAW[2] = Wire.read()<<8|Wire.read();//Z
  Mag_RAW[1] = Wire.read()<<8|Wire.read();//Y
  Mag_Updated = true;
}

///////////////////////////////////////////////////////////////////////

//3.Acceleromter Code

bool Acc_Init(){
  
}

///////////////////////////////////////////////////////////////////////

//4.Barometer Code
#define BMP085_AC1_REG 0xAA//AC1: First MSB part of Calibration Data stored in EEPROM.
#define BARO_TempRead_Interval 1000000UL//Interval of time Temperature data will be read.[us]
typedef struct{
  /*
   *Stores the data read from EEPROM of BMP085. Used for calibration
   */
short AC1;
short AC2;
short AC3;
unsigned short AC4;
unsigned short AC5;
unsigned short AC6;
short BB1;
short B2;
short MB;
short MC;
short MD;
}BMP085_CAL;

BMP085_CAL BMcal;//Cal Data Struct Variable

long Baro_UPP;//Pressure DATA
long Baro_UT;//Temperature DATA
short Baro_oss = 3;//oversampling setting(0~3), Can be added as 6&7th bit in 0xF4(default) Register.

const float p0 = 101325;//Pressure at sea level(Pa)
long Baro_final_T;//in 0.1 degCel.
long Baro_final_P;//int Pa. At Sea Level : 101300.

float Calculate_Altitude(long pressure){
  float altitude = (float)44330*(1 - pow( ((float)pressure/p0) , 0.190295));
  return altitude;//[m]
}

void Baro_getCalibrationData(){
  //Gets EEPROM Data 
  Wire.beginTransmission(BARO_ADDRESS);
  Wire.write(BMP085_AC1_REG);//first register of EEPROM Data
  Wire.endTransmission(false);//don't release the I2C line.
  Wire.requestFrom(BARO_ADDRESS,22);//total 176 bit. (16 bit chunk)*Total of 11 members!
  while(Wire.available()<22);//wait until all data gets received.
  for(int i=0;i<sizeof(BMP085_CAL)/sizeof(short);i++){//2 Bit COOKIE~~~! Cuz Arduino puts LSB first on 'insertion'
    ((short*)&BMcal)[i] =Wire.read()<<8|Wire.read();
  }
}

void Baro_readData(){
  //Internally updates 'Baro_Currently_Processing' to take on 4 steps in sensor data reading.
  static unsigned long Baro_lastTempReadSETUPtime;
  static unsigned long Baro_lastPressureReadSETUPtime;
  static byte Baro_Currently_Processing = 0;//0: PresREG Write / 1: ReadPres
  static bool Baro_TempRead_NEEDED = true;//because Temp reading need to be only 1 sample/sec, this variable will control the system.

  if((micros()-Baro_lastTempReadSETUPtime)>=BARO_TempRead_Interval){//More than 'BARO_TempRead_Interval' microseconds has passed
    WriteByte(BARO_ADDRESS,0xF4,0x2E);//Start reading Temperature Data}
    Baro_lastTempReadSETUPtime = micros();
    Baro_TempRead_NEEDED = true;
    }
    
  if(Baro_TempRead_NEEDED==true)//At least try to read the Temp.Data Bro :)
    if((micros()-Baro_lastTempReadSETUPtime)>=4500){//After this amount of time, Temp. can be read
        Wire.beginTransmission(BARO_ADDRESS);
        Wire.write(0xF6);//address of Temperature Register.
        Wire.endTransmission(false);//keep on talking.
        Wire.requestFrom(BARO_ADDRESS,2);//2 Byte Temp.data
        while(Wire.available()<2);
        Baro_UT = Wire.read()<<8|Wire.read();//MSB first
        Baro_TempRead_NEEDED = false;//no need to read after this... until 1 sec passes.
        } 
   
  if(Baro_Currently_Processing==0){
      WriteByte(BARO_ADDRESS,0xF4,(0x34|Baro_oss<<6));//Write (0x34|oss<<6) into 0xF4 register!!
      Baro_lastPressureReadSETUPtime = micros();
      Baro_Currently_Processing+=1;
      }
  else if(Baro_Currently_Processing==1){
      switch(Baro_oss){
        case 0://Ultra Low Power (ULP)
          if((micros()-Baro_lastPressureReadSETUPtime)<4500){return;}//at least 4.5ms
          break;
        case 1://Standard (Std)
          if((micros()-Baro_lastPressureReadSETUPtime)<7500){return;}//at least 7.5ms
          break;
        case 2://High Resolution (HR)
          if((micros()-Baro_lastPressureReadSETUPtime)<13500){return;}//at least 13.5ms
          break;
        case 3://Ultra High Resolution (UHR)
          if((micros()-Baro_lastPressureReadSETUPtime)<25500){return;}//at least 25.5ms
          break;
      }
      Wire.beginTransmission(BARO_ADDRESS);
      Wire.write(0xF6);//address of Pressure Register.
      Wire.endTransmission(false);//keep on talking.
      Wire.requestFrom(BARO_ADDRESS,3);//3 Byte Pressure.data
      while(Wire.available()<3);//[MSB,LSB,XLSB]
      Baro_UPP = ( ((unsigned long)Wire.read()<<8|Wire.read())<<8|Wire.read() )>>(8-Baro_oss);
      Baro_Currently_Processing=0;
      }
  else{Baro_Currently_Processing=0;}//if it is not 0~3, then it is an ERROR :( => set to 0
  
}

void Baro_applyCalibration(){
  long X1 = ((Baro_UT-BMcal.AC6)*BMcal.AC5)>>15;//UT is 'long' so no worries
  long X2 = ((long)BMcal.MC<<11)/(X1+BMcal.MD);//If no 'long', than bit disappears!!(Magic!)
  long B5 = X1+X2;
  Baro_final_T = (B5+8)>>4;//ex) 150 = 14.0(degC)
  //Calculate <Temperature>

  long B6 = B5 - 4000;
  X1 = (BMcal.B2*(B6*B6)>>12)>>11;
  X2 = (BMcal.AC2*B6)>>11;
  long X3 = X1+X2;
  long B3 = ( (( (long)BMcal.AC1*4 + X3)<<Baro_oss) +2 ) >>2;//uses 'X3'
  X1 = (BMcal.AC3*B6)>>13;
  X2 = (BMcal.BB1*(B6*B6)>>12)>>16;
  X3 = (X1+X2+2)>>2;
  unsigned long B4 = (BMcal.AC4*(unsigned long)(X3+32768))>>15;
  unsigned long B7 = ((unsigned long)(Baro_UPP-B3)*(50000>>Baro_oss));
  
  if(B7<0x80000000)
    Baro_final_P = (B7<<1)/B4;
  else
    Baro_final_P = (B7/B4)<<1;
  
  X1 = (Baro_final_P>>8)*(Baro_final_P>>8);//upper bit Squared.
  X1 = (X1*3038) >> 16;
  X2 = (-7357*Baro_final_P)>>16;
  Baro_final_P += (X1+X2+3791)>>4;
  //Calculate <Pressure>
}
///////////////////////////////////////////////////////////////////////

//5.DHT22 Code(DHT)

#define DHT22_PIN 7//pint to Talk with 1-wire Protocol

DHT22 Dht(DHT22_PIN);//create instance.
DHT22_ERROR_t Dht_Error;
//short Dht_RAW_Temperature;//[0.1degC]
//short Dht_RAW_Pressure;//[0.1%]
float Dht_Temperature;//[degC]
float Dht_Pressure;//[%]

void Dht_readData(){
  Dht_Error = Dht.readData();
  if(Dht_Error == DHT_ERROR_NONE){//Correct Reading
    Dht_Temperature = Dht.getTemperatureC();
    Dht_Pressure = Dht.getHumidity();
    Dht_Updated = true;
  }
}

///////////////////////////////////////////////////////////////////////

//6.NEO-M8N Code(GPS)

//#include<SoftwareSerial.h>
//SoftwareSerial GPS = SoftwareSerial(3,5);
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
  0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A, //(5Hz)
 // 0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39 //(1Hz)
};

void Gps_Init(){
  for(int i=0;i<sizeof(UBLOX_INIT);i++){
    GPS.write(pgm_read_byte(UBLOX_INIT+i));//Read Data from PROGMEM
    delay(5);//about 200Hz => 1600 baudrate.
  }
}

const unsigned char UBX_HEADER[] = { 0xB5, 0x62 };
const unsigned char NAV_PVT_clsid[]={0x01,0x07};//Correct Class ID for NAV_PVT
typedef struct{
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
}NAV_PVT;

NAV_PVT Gps_pvt;

void calcChecksum(unsigned char* CK) {
  memset(CK, 0, 2);
  for (int i = 0; i < (int)sizeof(NAV_PVT); i++) {
    CK[0] += ((unsigned char*)(&pvt))[i];
    CK[1] += CK[0];
  }
}

void Gps_getData() {//Function that reads in GPS Data
  static int fpos = 0;
  static unsigned char checksum[2];
  static const int payloadSize = sizeof(NAV_PVT);

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
        ((unsigned char*)(&Gps_pvt))[fpos-2] = c;
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
          Gps_Updated = true;
        }
      }
      else if ( fpos > (payloadSize+4) ) {
        fpos = 0;
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/////////////////<NAVIGATION>////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct{
  float lat;
  float lon;
  float Height;//Height above Ellipsoid.
}Geo_Location;

Geo_Location Vehicle;//Flying wing. It is.
Geo_Location HOME;//No.1 Prior Landing Coordinate.
Geo_Location Emergency[3];//Emergency Landing coordinates

float geoDistance(Geo_Location &a,Geo_Location &b) {
  const float R = 6371000; // m
  float p1 = a.lat * DEG_TO_RAD;
  float p2 = b.lat * DEG_TO_RAD;
  float dp = (b.lat-a.lat) * DEG_TO_RAD;
  float dl = (b.lon-a.lon) * DEG_TO_RAD;

  float x = sin(dp/2) * sin(dp/2) + cos(p1) * cos(p2) * sin(dl/2) * sin(dl/2);
  float y = 2 * atan2(sqrt(x), sqrt(1-x));

  return R * y;
}

float geoBearing(Geo_Location &a,Geo_Location &b) {
  float y = sin((b.lon-a.lon)*DEG_TO_RAD) * cos(b.lat*DEG_TO_RAD);
  float x = cos(a.lat*DEG_TO_RAD)*sin(b.lat*DEG_TO_RAD) - sin(a.lat*DEG_TO_RAD)*cos(b.lat*DEG_TO_RAD)*cos((b.lon-a.lon)*DEG_TO_RAD);
  return atan2(y, x) * RAD_TO_DEG;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/////////////////<PID_CALCULATION>/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pid_calculate_Setpoint(Geo_Location &target){
  static float Error_Temp;
  Pid_setpoint_Heading = geoBearing(Vehicle,target);
  Error_Temp = Pid_setpoint_Heading - Vehicle_Heading;
  Pid_i_mem_Heading += Error_Temp*Pid_Ki_Heading;
  if(Pid_i_mem_Heading>Pid_max_Heading){ Pid_i_mem_Heading = Pid_max_Heading; } // 'Pid_max_Heading' is actually not so much about Heading...
  else if(Pid_i_mem_Heading<-1*Pid_max_Heading){ Pid_i_mem_Heading = -Pid_max_Heading; }
  Pid_setpoint_Roll = 0;//This Heading-error calculation directly influences 'Pid_setpoint_Roll'!
  Pid_setpoint_Roll = (Error_Temp)*Pid_Kp_Heading + Pid_i_mem_Heading + (Gyro_dps[1]-Pid_d_mem_Heading)*Pid_Kd_Heading;
  if(Pid_setpoint_Roll>Pid_max_Heading){ Pid_setpoint_Roll = Pid_max_Heading; }
  else if(Pid_setpoint_Roll<-1*Pid_max_Heading){ Pid_setpoint_Roll = -Pid_max_Heading; }
  Pid_d_mem_Heading = Error_Temp;
}

void Pid_calculate_Output(){//Pid_setpoint_Roll&Pitch must be set before this!
  //Roll CALCULATION
  static float Error_Temp = Pid_setpoint_Roll-Gyro_dps[1];//Roll = Y Axis.
  Pid_i_mem_Roll += Error_Temp*Pid_Ki_Roll;
  if(Pid_i_mem_Roll>Pid_max_Roll){ Pid_i_mem_Roll = Pid_max_Roll; }
  else if(Pid_i_mem_Roll<-1*Pid_max_Roll){ Pid_i_mem_Roll = -Pid_max_Roll; }
  Pid_Output_Roll = 0;
  Pid_Output_Roll = (Error_Temp)*Pid_Kp_Roll + Pid_i_mem_Roll + (Gyro_dps[1]-Pid_d_mem_Roll)*Pid_Kd_Roll;
  if(Pid_Output_Roll>Pid_max_Roll){ Pid_Output_Roll = Pid_max_Roll; }
  else if(Pid_Output_Roll<-1*Pid_max_Roll){ Pid_Output_Roll = -Pid_max_Roll; }
  Pid_d_mem_Roll = Error_Temp;

  //Pitch CALCULATION
  Error_Temp = Pid_setpoint_Pitch-Gyro_dps[0];//Pithc = X Axis.
  Pid_i_mem_Pitch += Error_Temp*Pid_Ki_Pitch;
  if(Pid_i_mem_Pitch>Pid_max_Pitch){ Pid_i_mem_Pitch = Pid_max_Pitch; }
  else if(Pid_i_mem_Pitch<-1*Pid_max_Pitch){ Pid_i_mem_Pitch = Pid_max_Pitch*-1; }
  Pid_Output_Pitch = 0;
  Pid_Output_Pitch = (Error_Temp)*Pid_Kp_Pitch + Pid_i_mem_Pitch + (Gyro_dps[1]-Pid_d_mem_Pitch)*Pid_Kd_Pitch;
  if(Pid_Output_Pitch>Pid_max_Pitch){ Pid_Output_Pitch = Pid_max_Pitch; }
  else if(Pid_Output_Pitch<-1*Pid_max_Pitch){ Pid_Output_Pitch = -Pid_max_Pitch*-1; }
  Pid_d_mem_Pitch = Error_Temp;
  
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/////////////////<LOGGERS>//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SdFat Sd;
SdFile Sd_File;
const int CS = 4;

bool Sd_Init(){
  pinMode(10,OUTPUT);
  if(!Sd.begin(CS)){ return false;}
  Sd_File = Sd.open("test.txt",FILE_WRITE);
  Gps_Fixed
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/////////////////<ETC.>//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const int Parachute_Pin = 5;

void Open_Parachute(){
  
}

const int Buzzer_Pin = 3;

enum BUZZER_SOUNDS{
  ErrorAlarm_OneSec,
  Beep_HalfSec,
  Song_Success,
  Emergency_OneSec,
}

const int Led_Pin = 2;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/////////////////<RUN_LEVELS>//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum RUN_LEVEL{
  BEFORE_LAUNCH,
  /*  <<Before Launch>>
   *  - 
   *  To be qualified to go onto (B_L => A_L_T_D): GPS 3d-fixed & Sensors Calibrated & Battery Charged & Physicaly Tested*/
  AFTER_LAUNCH_TO_DETATCH,
  //To be qualified to go onto (A_L_T_D => R_H): Negative rising speed Detected | Max Altitude Reached
  RETURNING_HOME,
  //To be qualified to go onto (R_H => O_P): Min Altitude Reached & Distance to HOME smaller than Reached_Home_Distance. 
  OPEN_PARACHUTE,
  /* <<Open Parachute>>
   *  1) In Emergency Situation:
   *  - During Mission, if battery voltage is lower than Battery_Min
   *  - If the glider decides that it can't reach the Target based on it's Velocity, Position(Lat,Lon,Height) 
   *  
   */
};

RUN_LEVEL master_runlevel;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/////////////////<SENSORS>////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  Wire.begin();//Start I2C communication
  Serial.begin(9600);//Serial for GPS Processing
  master_runlevel = BEFORE_LAUNCH;
  if(!Sensors_Init())
 }
void loop(){
  
}






/////////////////<__>////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------------------------------------------------------------------------------
