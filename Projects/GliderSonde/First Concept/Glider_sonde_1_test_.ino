/*
 Glider_sonde_1 sketch
 first created in 160817 by Junwoo HWANG(GPS is from iforce2d's sketch)
 This sketch doesn't have any yaw value controls because it is dedicated to a 2 elevon only aircraft and so yaw can only be changed by changing the roll.

<This is the Glider_sonde_1_test_ sketch>

Acknowledgements: acc_data is quite bumpy. So it should be added with minimal amount to the filtered angle we are dealing with!
Therefore, additional angles like Gyro_angle[3] is not needed I guess?? (('Gyro' is the foundation!))


What is modified:
1. Gyro_setpoints will be calculated seperately and used for pid.(setpoint = 0.7 * setpoint + 0.3 * gyro_[])
--> I am not sure why Joop used this so I'm going to test it out

2. pid max values are added + pid i mem max value is added(we can't go crazy far away!) => But... hold on... It is not on pitch and roll on iforce2d's sketch... hmm.... let's figure it out!!

3.HMC5883L 3-D magnetometer is used(address: 0x1e)

4.Magnetometer tilt compensation with filtered angles is added

<RULES>
heading is calculated as: -180 <= heading < 180
pitch is calculated as: -90 <= pitch < 90
roll is calculated as: -90 <= roll < 90
*/
 
#include<Wire.h>
#include <SoftwareSerial.h>
#include<SD.h>

// Connect the GPS RX/TX to arduino pins 3 and 5
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
  //0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x13,0xBE, //NAV-POSLLH on
  //0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x14,0xC5, //NAV-STATUS on

  // Rate
  0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x01,0x00,0x7A,0x12, //(10Hz)
  //0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A, //(5Hz)
  //0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39 //(1Hz)
};
const unsigned char UBX_HEADER[] = { 0xB5, 0x62 };
///Magnetometer values////
const int Mag_addr=0x1e;//HMC5883L
int MgX,MgY,MgZ;

////Gyro values/////
const int Gyro_addr=0x68;//MPU6050
int16_t AcX, AcY, AcZ, Tmp;
float GyX, GyY, GyZ;/////gyros need some precision
float Gyro_cal[3];//calibration values

//////////////////PID values//////////////////////////
float pid_roll_Kp=0;
float pid_roll_Ki=0;
float pid_roll_Kd=0;
float pid_roll_imem=0;
float pid_roll_dmem=0;
int pid_max_roll = 400;
//int pid_max_roll_imem = 100;////////////////////////Check! This is different to all planes!! Please Check!!//////////////////////

float pid_pitch_Kp=0;
float pid_pitch_Ki=0;
float pid_pitch_Kd=0;
float pid_pitch_imem=0;
float pid_pitch_dmem=0;
int pid_max_pitch = 400;
//int pid_max_pitch_imem = 100;//////////////////////////Check! This is different to all planes!! Please Check!!///////////////////

int pid_roll_output;
int pid_pitch_output;//saves the output values

#define roll_setpoint_MAX 25
float roll_setpoint,pitch_setpoint;//determines the angular velocity (deg/s) 
float roll_desired_angle,pitch_desired_angle;//desired angle to go(deg)

/////////////////////////////////////////////////////

float Acc_angle[2],total_vector;//Saves the calculated angle for gyro and accelometer
float filtered_roll=0,filtered_pitch=0;
float ALPHA=0.004;//How much acc angle affects the filtered angle

float heading_now,heading_nav,heading_error;//used for auto control
unsigned long loop_timer;//checks the loop running time
int loop_counter=0;//Checks how many loops we have spun

int cal_count=0;//how many calculation we took properly(should be 2000) in normal cases

int CS_pin=;//Chip Select

///////////////////////NAV_PVT///////////////////////////////////////////////////////////
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
  char valid;                  // Validity Flags (see graphic below)
  unsigned long tAcc;          // Time accuracy estimate (UTC) (ns)
  long nano;                   // Fraction of second, range -1e9 .. 1e9 (UTC) (ns)
  unsigned char fixType;       // GNSSfix Type, range 0..5
  char flags;                  // Fix Status Flags
  unsigned char reserved1;     // reserved
  unsigned char numSV;         // Number of satellites used in Nav Solution
  
  long lon;                    // Longitude (deg)
  long lat;                    // Latitude (deg)
  long height;                 // Height above Ellipsoid (mm)
  long hMSL;                   // Height above mean sea level (mm)
  unsigned long hAcc;          // Horizontal Accuracy Estimate (mm)
  unsigned long vAcc;          // Vertical Accuracy Estimate (mm)
  
  long velN;                   // NED north velocity (mm/s)
  long velE;                   // NED east velocity (mm/s)
  long velD;                   // NED down velocity (mm/s)
  long gSpeed;                 // Ground Speed (2-D) (mm/s)
  long heading;                // Heading of motion 2-D (deg)
  unsigned long sAcc;          // Speed Accuracy Estimate
  unsigned long headingAcc;    // Heading Accuracy Estimate
  unsigned short pDOP;         // Position dilution of precision
  short reserved2;             // Reserved
  unsigned long reserved3;     // Reserved
};

NAV_PVT pvt;
//////////////////////////////////////////////////////////////////////////////////

void calcChecksum(unsigned char* CK) {
  memset(CK, 0, 2);
  for (int i = 0; i < (int)sizeof(NAV_PVT); i++) {
    CK[0] += ((unsigned char*)(&pvt))[i];
    CK[1] += CK[0];
  }
}

///////////////////////processGPS/////////////////////////////////////////////////
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
        if ( c != checksum[0] )
          fpos = 0;
      }
      else if ( fpos == (payloadSize+4) ) {
        fpos = 0;
        if ( c == checksum[1] ) {
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
///////////////////////////////////////////////////////////////////////////////////////
struct geoloc{
  float lat;
  float lon;
};

float geoDistance(struct geoloc &a, struct geoloc &b) {
  const float R = 6371000; // km
  float p1 = a.lat * DEG_TO_RAD;
  float p2 = b.lat * DEG_TO_RAD;
  float dp = (b.lat-a.lat) * DEG_TO_RAD;
  float dl = (b.lon-a.lon) * DEG_TO_RAD;

  float x = sin(dp/2) * sin(dp/2) + cos(p1) * cos(p2) * sin(dl/2) * sin(dl/2);
  float y = 2 * atan2(sqrt(x), sqrt(1-x));

  return R * y;
}

float geoBearing(struct geoloc &a, struct geoloc &b) {// a is the start
  float y = sin(b.lon-a.lon) * cos(b.lat);
  float x = cos(a.lat)*sin(b.lat) - sin(a.lat)*cos(b.lat)*cos(b.lon-a.lon);
  return atan2(y, x) * RAD_TO_DEG;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

void Gyro_init(){//Initialize the gyro
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void Mag_init(){//Initialize the magnetometer
  Wire.beginTransmission(Mag_addr);
  Wire.write(0x02);//Mode registor
  Wire.write(0x00);//Continuous mode
  Wire.endTransmission();////////////////////////////////////!!!!!!! Does it really need 'true'??//////////////////
}

void Gyro_read(){//Read the gyro values, apply calibration and turn it's demension into degree/s
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  
  //while(Wire.available()<14);
  
  AcX=Wire.read()<<8|Wire.read(); 
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();
  Tmp=Wire.read()<<8|Wire.read();
  GyX=Wire.read()<<8|Wire.read();//////////////////////////////////! Just check whether it is possible to save int 16 data in "float" format.
  GyY=Wire.read()<<8|Wire.read();
  GyZ=Wire.read()<<8|Wire.read();

  GyX -= Gyro_cal[0];
  //GyX /= 131;
  GyY -= Gyro_cal[1];
  //GyY /= 131;
  GyZ -= Gyro_cal[2];
  //GyZ /= 131;////////////////////////////////////////////////////!!!! Check if /=131 is right in registor!!!/////////////////////////////////
}

void Mag_read(){////////////////////////////////Reads the magnetometer values(+- 0.88 Ga is detected(1370 Lsb/Gauss) => Earth's magnetic field is 0.25~0.65 Gauss)
  Wire.beginTransmission(Mag_addr);
  Wire.write(0x03);//X msb registor
  Wire.endTransmission();
  Wire.requestFrom(Mag_addr,6);
if(Wire.available() >= 6){
  MgX = Wire.read()<<8|Wire.read();
  MgZ = Wire.read()<<8|Wire.read();
  MgY = Wire.read()<<8|Wire.read();
  }
}

void Mag_rotate(){//Magnetometer has to be vertical with the gravity so it kinda needs compensation for pitch and roll I guess??(I thought this is quite important)
 float temp_trash[3]={MgX,MgY,MgZ};///////////////!!!!!!!!!!!!assuming that it is same axis with the MPU6050 sensor!!///////////////////////////////////////////////////////////////////////////////////////////
 float sytaX = -filtered_pitch * DEG_TO_RAD,sytaY = -filtered_roll * DEG_TO_RAD
  MgX = temp_trash[0] * cos(sytaY) + temp_trash[2] * sin(sytaY);
  MgY = temp_trash[0] * sin(sytaX) * sin(sytaY) + temp_trash[1] * cos(sytaX) - temp_trash[2] * sin(sytaX) * cos(sytaY);
  MgZ = -temp_trash[0] * sin(sytaY) * cos(sytaX) + temp_trash[1] * sin(sytaX) + temp_trash[2] * cos(sytaX) * cos(sytaY);
}//Just for notice, this was tested in visual studio Rotation_Matrix.

void IMU_calc(){//Involves filtering
  total_vector = sqrt( AcX*AcX + AcY*AcY + AcZ*AcZ );
Acc_angle[0] = asin( AcY/total_vector) * RAD_TO_DEG;//X-axis which is Pitch/////////////////////////////// Be careful with 'NaN' what is it?? Research it!!/////////////////////////
Acc_angle[1] = -asin(AcX/total_vector) * RAD_TO_DEG;//Y-axis which is Roll

//dt should be 1/250 = 0.004 seconds!
filtered_pitch += GyX * 0.004/131;
filtered_roll += GyY * 0.004/131;

filtered_pitch -= filtered_roll * sin(GyZ * 0.004/131 * DEG_TO_RAD);                  //If the IMU has yawed transfer the roll angle to the pitch angel.
filtered_roll += filtered_pitch * sin(GyZ * 0.004/131 * DEG_TO_RAD); //If the IMU has yawed transfer the pitch angle to the roll angel.

filtered_pitch = filtered_pitch * (1 - ALPHA) + Acc_angle[0] * ALPHA;
filtered_roll = filtered_roll * (1 - ALPHA) + Acc_angle[1] * ALPHA;
}

void roll_setpoint_set(){//Calculates the roll_setpoint from heading offset
  heading_error = heading_now - heading_nav;//must be in degrees
  if(heading_error>=180)
  heading_error -= 360;
  if(heading_error<-180)
  heading_error += 360;//heading_error is between -180 ~ 180
  roll_desired_angle = heading_error/2;
 roll_desired_angle = constrain(roll_desired_angle,-roll_setpoint_MAX,roll_setpoint_MAX);//limits the max angle to tilt(-25 ~ +25)/////////////////////This kinda needs some experiments!!////////////////////
  roll_setpoint = roll_desired_angle - filtered_roll;//it is degree/s dimension We need to(without any pid calc.) get to that angle so angle_desired - current angle (setpoint!!) Max:50 or sth
}

void calculate_PID(){
  pid_roll_output=0;//reset the output because we need to add/////////////////////////////!!! Check if int(output) deals well with float(pid calculations)!!!!////////////////////////////////
  float temp_error = GyY/131 - roll_setpoint;//Calculation of roll output
  pid_roll_output -= temp_error * pid_roll_Kp;
  pid_roll_imem += temp_error * pid_roll_Ki;//add value to pid_roll_imem
  pid_roll_output -= pid_roll_imem;//ki is already multiplied
  pid_roll_output -= (temp_error - pid_roll_dmem) * pid_roll_Kd;
  pid_roll_dmem = temp_error;//calculation of roll_output done.
 pid_roll_output = constrain(pid_roll_output,-pid_max_roll,pid_max_roll);//////////////////Max value set
   
  pid_pitch_output=0;
  temp_error = GyX/131;//Calculation of pitch output////////////////////Where is pitch_setpoint?? LOL////////////////
  pid_pitch_output -= temp_error * pid_pitch_Kp;
  pid_pitch_imem += temp_error * pid_pitch_Ki;//add value to pid_pitch_imem
  pid_pitch_output -= pid_pitch_imem;//ki is already multiplied
  pid_pitch_output -= (temp_error - pid_pitch_dmem) * pid_pitch_Kd;
  pid_pitch_dmem = temp_error;//calculation of pitch_output done.
 pid_pitch_output = constrain(pid_pitch_output,-pid_max_pitch,pid_max_pitch);
  
}

void setup() {

  Wire.begin();
  
  Gyro_init();//Initialize the gyro
  Mag_init();//Initialize the magnetometer
  TWBR = 12;/////////////////////////////////////////////I2C speed boosted!!/////////////////////////////

for(int i=0;i<3;i++){//Reset calibration data for gyro
      Gyro_cal[i]=0;
      }
 
  for(cal_count = 0;cal_count < 2000; cal_count++){//Start calibration for gyro
    Gyro_read();
    Gyro_cal[0] += GyX;
    Gyro_cal[1] += GyY;
    Gyro_cal[2] += GyZ;
    //delay(1);
    }

    for(int i=0;i<3;i++)
      Gyro_cal[i]/=cal_count;
    
  for(unsigned int i = 0; i < sizeof(UBLOX_INIT); i++) {                        
    serial.write( pgm_read_byte(UBLOX_INIT+i) );
    delay(5); // simulating a 38400baud pace (or less), otherwise commands are not accepted by the device.
  }

  File dataFile = SD.open("log.txt",FILE_WRITE);//Open File
  
  loop_timer = micros();//First value for the loop!!

}
void loop() {

Gyro_read();//read from gyro
IMU_calc();//uses gyro data-> filtered angles are calculated!

Mag_read();
Mag_rotate();//I hope this doesn't take much time....

heading_now = atan2(MgY,-MgX);//I hope this is right for MPU 6050 kinda coordinate.


roll_setpoint_set();//uses compass data and gps data

calculate_PID();//uses gyro data-> pid outputs are calculated!

while(micros() < loop_timer + 4000){
  if(micros() - loop_timer < 0)//If overflow detected
  loop_timer = unsigned long max - loop_timer///////////////////////////////////////!! This will give negative vaule, check if it is possible!////////////////////////////////...YUP! Check it!
};//wait until 4ms is passed

loop_timer = micros();
/////////////Setting of Servos Needed//////////////
if(loop_counter%5==0)


///////////////////////////////////////////////////
if(loop_counter == 250){
  //SD card logging(every 1 second)
  loop_counter = 0;//initialize the counter
}
loop_counter++;
//loop_timer = micros();//Let's just say that we didn't see above two if statements. Because it is considered as a time-warp!!(Actually... I'm just going with original for several reasons..)

}

