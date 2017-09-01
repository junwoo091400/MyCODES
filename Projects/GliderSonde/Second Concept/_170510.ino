#include<Wire.h>
#include <SPI.h>
#include "SdFat.h"
//////////////////
bool DEBUGGING_MODE = true;
#define DEBUGGING_PIN_PORT B00000100//DEBUGGING PIN at 2!
#define DEBUGGING_PIN_SET_OUTPUT  DDRD |= DEBUGGING_PIN_PORT;

#define DEBUGGING_PIN_ON PORTD |= DEBUGGING_PIN_PORT;
#define DEBUGGING_PIN_OFF PORTD &= ~DEBUGGING_PIN_PORT;
//////////////////
#define PPM_PIN 9
#define PPM_CHANNEL_COUNT 6
volatile int PPM_Input[PPM_CHANNEL_COUNT] = {-1,-1,-1,-1,-1,-1};
/*
 * Channel 1~6:
 * 1: Roll => Used
 * 2: Pitch => Used
 * 3: Throttle  => For Throttle
 * 4: Yaw => Don't use
 * 5: Dial1 => P term
 * 6: Dial2 => I term
 */
////////////////
bool AUTOLEVEL = true;

float Pid_Kp_Roll = 0;
float Pid_setpoint_Roll = 0;

int Pid_Output_Roll = 0;
int Pid_max_Roll = 400;

float Pid_Kp_Pitch = 0;
float Pid_setpoint_Pitch;

int Pid_Output_Pitch = 0;
int Pid_max_Pitch = 400;
//////////////////////////////////////////
float Acc_Pitch = 0;
float Acc_Roll = 0;
float Vehicle_Pitch = 0;
float Vehicle_Roll = 0;
float Acc_data_correction = 0.05;
float One_minus_Acc_data_correction = 1-Acc_data_correction;
//////////////////////////////////////////////////////////////
#define DEG_TO_RAD 0.017453292f
#define RAD_TO_DEG 57.295779f
//////////////////////////////////////////////////////////////
float Gyro_dps_per_digit = 0.0175;//0.0175 degree/s = 1 digit.
int Gyro_RAW[3];//X,Y,Z value straight from the sensor.
int Gyro_Cal[3];//calibration data
float Gyro_dps[3];//final result
int Gyro_Threshold[3];//threshold for Gyro_RAW
int Gyro_Cal_count = 0;
//
float Acc_g_per_digit = 0.0039;//0.0039 g force = 1 digit.(+-2g mode, or Full_Resolution Mode)
int Acc_RAW[3];//X,Y,Z value straight from the sensor.
float Acc_g[3];//final result
//
////////////////////////////////////////////////////////////////
#define L_SERVO_PORT        B00010000 // PD4
#define R_SERVO_PORT        B00100000 // PD5
#define MOTOR_PORT          B01000000

#define SERVO_SET_OUTPUT    DDRD |= (L_SERVO_PORT | R_SERVO_PORT | MOTOR_PORT);

#define LEFT_SERVO_OFF      PORTD &= ~L_SERVO_PORT;
#define RIGHT_SERVO_OFF     PORTD &= ~R_SERVO_PORT;
#define MOTOR_OFF           PORTD &= ~MOTOR_PORT;

#define ALL_SERVO_ON        PORTD |= (L_SERVO_PORT | R_SERVO_PORT | MOTOR_PORT);
#define ALL_SERVO_OFF       PORTD &= ~(L_SERVO_PORT | R_SERVO_PORT | MOTOR_PORT);

int motor_val = 0;
int L_Servo_val = 0;
int R_Servo_val = 0;
//////////////////////////////////////////////////////////////////
byte Calculation_step = 1;//250Hz. For ex.
byte Output_step = 5;
////////////////////////////////////////////////////////////////////////////////////////////////////
const uint8_t chipSelect = SS;
SdFat sd;
SdFile file;
const uint32_t BLOCK_COUNT = 1563;//250/8 = 31.25 block per second. => 50 second = 1562.5 block. 1563!
uint32_t bgnBlock, endBlock;
uint32_t curBlock = 0;
uint8_t* pCache;
/////////////////////////////////////////////////////////////////////
bool Sd_Init(){
if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {Serial.println("No begin"); return false;}
sd.remove("RawWrite.txt");
if (!file.createContiguous("RawWrite.txt", 512UL*BLOCK_COUNT)){Serial.println("No cC"); return false;}
if (!file.contiguousRange(&bgnBlock, &endBlock)){Serial.println("No cRange"); return false;}
if (!sd.card()->writeStart(bgnBlock, BLOCK_COUNT)){Serial.println("No wS"); return false;}
//*********************NOTE**************************************
// NO SdFile calls are allowed while cache is used for raw writes
//***************************************************************
pCache = (uint8_t*)sd.vol()->cacheClear();  
memset(pCache,' ',512);
pCache[507] = 'E';
pCache[508] = 'N';
pCache[509] = 'D';
pCache[510] = '\r';
pCache[511]='\n';//Last should be New Line.
return true;//Don't forget to return true when it is ok...
}

bool Sd_Write(){if(curBlock<BLOCK_COUNT){if (sd.card()->writeData(pCache)){curBlock++; return true;}; return false; } else{Sd_Stop();} }

void  Sd_Stop(){while(!sd.card()->writeStop()){delay(10);}; file.close();}

////////////////////////////////////////////////////////////////////////
void calculate_Vehicle_Attitude(){
  float Total_g_vec = sqrt(Acc_g[0]*Acc_g[0]+Acc_g[1]*Acc_g[1]+Acc_g[2]*Acc_g[2]);
  if(abs(Acc_g[1])<=Total_g_vec) Acc_Pitch = asin(Acc_g[1]/Total_g_vec)*RAD_TO_DEG;//calculate Acc angle 
  if(abs(Acc_g[0])<=Total_g_vec) Acc_Roll = -asin(Acc_g[0]/Total_g_vec)*RAD_TO_DEG;
  Vehicle_Pitch += Gyro_dps[0]*(0.004*Calculation_step);//add gyroscope angle
  Vehicle_Roll += Gyro_dps[1]*(0.004*Calculation_step);
  Vehicle_Pitch = Vehicle_Pitch*One_minus_Acc_data_correction+Acc_Pitch*Acc_data_correction;//Apply Acc angle correction
  Vehicle_Roll = Vehicle_Roll*One_minus_Acc_data_correction+Acc_Roll*Acc_data_correction;
}

#define GYRO_ADDRESS 0x69//address of gyro(L3G4200D,SD0pin(4) held HIGH)
#define ACC_ADDRESS 0x53//address of accelerometer(ADXL345,ALT ADDRESS(12) held LOW)

bool WriteByte(byte address,byte reg,byte data){
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(data);
  if(!Wire.endTransmission()) return true;//successfully written
  else  return false;
}

byte ReadByte(byte address,byte reg){//passes '1 Byte' of data from 'reg' from I2C device with 'address'
  Wire.beginTransmission(address);
  Wire.write(reg);
  if(Wire.endTransmission(false)) return 0;//keep connection(read mode)
  Wire.requestFrom(address,1);
  while(!Wire.available());//wati until '1 byte' is recieved.
  return Wire.read();
}

//1.GyroScope Code
#define GYRO_CALIBRATE_COUNT 1000 // DONT, PUT, SEMICOLON, AFTER MACRO. DONT DONT DONT! You wasted my 30 minute !! 1703061735
#define GYRO_THRESHOLD_MULTIPLE 4//Determines Threshold Value

bool Gyro_Init(){
  if(ReadByte(GYRO_ADDRESS,0x0F)!=0xD3)//'WHO_AM_I' register Check.
    return false;
  if(!WriteByte(GYRO_ADDRESS,0x20,B00101111))//<CTRL_REG1> 'DR to 400Hz(B11)' & 'set PD bit(3) to 1(normal/sleep mode)' & 'XYZ enable'
      return false;
  if(!WriteByte(GYRO_ADDRESS,0x23,B10010000))//<CTRL_REG4> set FS10 bit(5,4) = 0,1. AND BDU bit(7) =1<BLOCK DATA UPDATE>. (500 dps scale.)
      return false;
  return true;}

void Gyro_readData(){
  Wire.beginTransmission(GYRO_ADDRESS);
  Wire.write(0x28|1<<7);//Read from <OUT_X_L>. AND, MSB for the 'auto-increment' OR ELSE, auto_increment won't happen and Slave will keep giving the same data!
  Wire.endTransmission(false);//sending address&write_bit(0) completed. FALSE for keeping connection.
  Wire.requestFrom(GYRO_ADDRESS,6);//read 6 bytes, then give 'stop'
  while(Wire.available()<6);//wait until all data arrives
   for(int i=0;i<3;i++){
    Gyro_RAW[i] = Wire.read()|Wire.read()<<8;//RAW data reading...
    if(Gyro_Cal_count==GYRO_CALIBRATE_COUNT)//Only apply calibration value after calibration.
    Gyro_dps[i]=(abs(Gyro_RAW[i])>Gyro_Threshold[i])?((Gyro_RAW[i]-Gyro_Cal[i])*Gyro_dps_per_digit):(0);//if Inside Threshold, return 0.
    else//If, not calibrated
    Gyro_dps[i]=Gyro_RAW[i]*Gyro_dps_per_digit;
  }
}

void Gyro_getCalibrationData(){
  long SUM[3]={0,0,0};//place to store sum of all data
  unsigned long Squared[3]={0,0,0};//place to store all squared data, at GYRO_CALIBRATE_COUNT==1000, each squared shouldn't exceed: (2^32-1)/1000.
  
  for(Gyro_Cal_count=0;Gyro_Cal_count<GYRO_CALIBRATE_COUNT;Gyro_Cal_count++){
     Gyro_readData();
     for(int j=0;j<3;j++){
        SUM[j] += Gyro_RAW[j];
        Squared[j]+=Gyro_RAW[j]*Gyro_RAW[j];
        delayMicroseconds(3000);
     }
   }//Calibration Read/Save Process
   
   for(byte j=0;j<3;j++){
        Gyro_Cal[j]=SUM[j]/GYRO_CALIBRATE_COUNT;//int type
        Gyro_Threshold[j] = abs(sqrt((float)Squared[j]/GYRO_CALIBRATE_COUNT-Gyro_Cal[j]*Gyro_Cal[j])*GYRO_THRESHOLD_MULTIPLE);//(+) Positive Value
        // sqrt {sigma_squared/totalNum - Mean^2 } * CONSTANT
     }
}

bool Acc_Init(){
  if(ReadByte(ACC_ADDRESS,0x00)!=0xE5) return false;//DVID register check.
  if(!WriteByte(ACC_ADDRESS,0x2C,0x0C)) return false;//Output Data Rate 400Hz(B1100) & Normal_Operation Mode.
  if(!WriteByte(ACC_ADDRESS,0x2D,0x08)) return false;//'Measure_Mode' ON.
  if(!WriteByte(ACC_ADDRESS,0x31,0x09)) return false;//Full Resolution Mode ( 0.4 mg / LSB no matter what Range is selected.) & +-4g limit.
  return true;
}
void Acc_readData(){
  //while(!ReadByte(Acc_ADDRESS,0x27)&(1<<3));//wait until <STATUS_REG> 'ZYXDA(x,y,z-axis new data available' flag turns 0;(in the loop, if set to 0.<not ready>)
  Wire.beginTransmission(ACC_ADDRESS);
  Wire.write(0x32|1<<7);//Read from AND, MSB for the 'auto-increment' OR ELSE, auto_increment won't happen and Slave will keep giving the same data!
  Wire.endTransmission(false);//sending address&write_bit(0) completed. FALSE for keeping connection.
  Wire.requestFrom(ACC_ADDRESS,6);//read 6 bytes, then give 'stop'
  while(Wire.available()<6);//wait until all data arrives
   for(int i=0;i<3;i++){
    Acc_RAW[i] = Wire.read()|Wire.read()<<8;//RAW data reading...
    Acc_g[i]=Acc_RAW[i]*Acc_g_per_digit;
    }
}
/////////////////////////////////////
void update_PID_values(){
  Pid_setpoint_Roll = ((float)PPM_Input[0]-500)/15;//33.3 deg Max.
  Pid_setpoint_Pitch = ((float)PPM_Input[1]-500)/15;//!!!!!!!!!!!!!!!!!Make sure when STICKDOWN, Value UP!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  if(AUTOLEVEL){
  Pid_setpoint_Roll -= Vehicle_Roll;//if correct, max 66.6(=33.3-(-33.3)).
  Pid_setpoint_Pitch -= Vehicle_Pitch;
  }
  //I wish it can be like 250 deg/s or so. => Times 4. 66.6*4 = 266.4[deg/s] MAX.
  Pid_setpoint_Roll = constrain(Pid_setpoint_Roll*4,-270,270);//270 deg/s MAX -> If not AUTOLEVEL, 135deg/s.. pretty small..
  Pid_setpoint_Pitch = constrain(Pid_setpoint_Pitch*4,-270,270);
   
  Pid_Kp_Roll = (float)PPM_Input[4]/200;//0~1000 => (0~5.0)
  Pid_Kp_Pitch = (float)PPM_Input[5]/200;//0~1000 => (0~5.0)
}
////////////////////////
void Pid_calculate_Output(){
  //Pid_setpoint_Roll&Pitch must be set before this!
  
  //Roll CALCULATION
  static float Error_Temp = Pid_setpoint_Roll-Gyro_dps[1];//Roll = Y Axis.
  Pid_Output_Roll = 0;
  Pid_Output_Roll = (Error_Temp)*Pid_Kp_Roll;
  
  if(Pid_Output_Roll>Pid_max_Roll){ Pid_Output_Roll = Pid_max_Roll; }
  else if(Pid_Output_Roll<-1*Pid_max_Roll){ Pid_Output_Roll = -Pid_max_Roll; }
  
  //Pitch CALCULATION
  Error_Temp = Pid_setpoint_Pitch-Gyro_dps[0];//Pitch = X Axis.
  Pid_Output_Pitch = 0;
  Pid_Output_Pitch = (Error_Temp)*Pid_Kp_Pitch;
  
  if(Pid_Output_Pitch>Pid_max_Pitch){ Pid_Output_Pitch = Pid_max_Pitch; }
  else if(Pid_Output_Pitch<-1*Pid_max_Pitch){ Pid_Output_Pitch = -Pid_max_Pitch*-1; }
}
//////////////////////////////////////////////////////////////
unsigned long loop_timer = 0;
int loop_interval = 0;
byte loop_counter = 0;
///////////////////////////////////////////////////////////////////////
void setup() {
  
  if(DEBUGGING_MODE){DEBUGGING_PIN_SET_OUTPUT}
  SERVO_SET_OUTPUT
  
  Wire.begin();
  TWBR = 12; //400kHz I2c
  if(DEBUGGING_MODE){
    Serial.begin(19200);
    Serial.println("Hello Debugger! Have a nice Experiment!");
    Serial.println("Current Baud Rate is 19200");
  }
  
  attachInterrupt(digitalPinToInterrupt(PPM_PIN),PPM_Reader,FALLING);//Interrupt at 'falling'
  
  bool ERROR_OMG = false;
  
  if(!Gyro_Init()){if(DEBUGGING_MODE) Serial.println("Gyro Init Failed!"); ERROR_OMG = true;}
  if(!Acc_Init()){if(DEBUGGING_MODE) Serial.println("Acc Init Failed!"); ERROR_OMG = true;}
  if(!Sd_Init()){if(DEBUGGING_MODE) Serial.println("Sd Init Failed!"); ERROR_OMG = true;}
  
  if(ERROR_OMG){
    while(1){
      if(DEBUGGING_MODE) Serial.println("ERROR!");
      delay(1000);//1s HAHA
    }
  }
  
  Acc_Pitch = 0;
  Acc_Roll = 0;
  
  Vehicle_Pitch = 0;
  Vehicle_Roll = 0;

  //while(1){byte TEMP = 0; for(byte i=0;i<PPM_CHANNEL_COUNT;i++){if(PPM_Input[i]>=0&&PPM_Input[i]<=1000) TEMP++;} if(TEMP==PPM_CHANNEL_COUNT) break;}
  
  if(DEBUGGING_MODE) Serial.println("NO ERRORS, PRECEEDING TO THE LOOP!");
  
  loop_timer = micros();
}
//////////////////////////////////////////////////////
byte Sd_step = 2;
byte Sd_Stack = 0;//when gets 2, fflush!
unsigned char* newPointer = (unsigned char*)pCache;
///////////////////////////////////////////////////////
void loop() {
  while(micros()-loop_timer<4000);
  loop_interval = micros()-loop_timer;
  loop_timer = loop_interval+loop_timer;
  /////////////////////////////////////////////////////
  if(loop_counter%Calculation_step==0){//Sensor Reading&Calculation
    DEBUGGING_PIN_ON
    Gyro_readData();
    //DEBUGGING_PIN_OFF
    //DEBUGGING_PIN_ON
    Acc_readData();
    DEBUGGING_PIN_OFF
    DEBUGGING_PIN_ON
    calculate_Vehicle_Attitude();
    //DEBUGGING_PIN_OFF
    //DEBUGGING_PIN_ON
    update_PID_values();
    //DEBUGGING_PIN_OFF
    //DEBUGGING_PIN_ON
    Pid_calculate_Output();
    DEBUGGING_PIN_OFF
    //if(loop_counter%100==0) {Serial.print(micros()-L1);Serial.print('|');Serial.print(Vehicle_Pitch);Serial.print(',');Serial.println(Vehicle_Roll);}
  }
  if(loop_counter%Output_step==1){//Output Enable
    
    ALL_SERVO_ON//Turn on All servos!!!!!
    
    L_Servo_val = constrain((0.5)*(Pid_Output_Roll - Pid_Output_Pitch),-500,500);
    R_Servo_val = constrain((0.5)*(Pid_Output_Roll + Pid_Output_Pitch),-500,500);//Constrain These values...
    motor_val = constrain(PPM_Input[3],1000,2000);
    L_Servo_val += 1500;
    R_Servo_val += 1500;
      Serial.print("GP.");
      Serial.println(loop_counter);
    byte count = 0;
    unsigned long Servo_loop_timer = micros();//Because, the 'calculation phase' eats up alot of time... ne timer...
    while(count<3){
      count = 0;
      unsigned long Pulse_Loop_Time = micros();
      if(Pulse_Loop_Time-Servo_loop_timer>=L_Servo_val) {LEFT_SERVO_OFF; count++;}
      if(Pulse_Loop_Time-Servo_loop_timer>=R_Servo_val) {RIGHT_SERVO_OFF; count++;}
      if(Pulse_Loop_Time-Servo_loop_timer>=motor_val)   {MOTOR_OFF; count++;}
    }
  }//Output
  else if(Sd_Stack>=8 && false){
    if(Sd_Write()){//fflush~~~!!!!
      Sd_Stack = 0;//initialize stack count.
      newPointer = (unsigned char*)pCache;//initialize pointer
      if(curBlock++>=BLOCK_COUNT){
        Sd_Stop();
        while(true){Serial.println("Finished."); delay(1000);}
      }
    }
  }
  /////////////////////////pCache charger....///////////////////////////////////////////////
  /*
   * pCache will be stacked in following order:
   * (loop_timer),(Kp_Pitch),(Kp_Roll),(dps[0~2]),(acc_g[0~2]),(Vehicle_[Pitch, Roll]),(loop_interval),(loop_counter),(servoVal[L,R,motor]),(PPM_Input[0~1]),(Pid_setpoint_[Pitch,Roll])
   *      1<ul>  +    1<f>   +   1<f>  +  3<f>  +   3<f>   +         2<f>          +     1<i>      +      1<b>      +        2<i>        +       2<i>      +      2<i>
   *      Total : <byte(1)>*1 + <float(4)>*10 + <int(2)>*7 + <ul(4)>*1 = 59.[Just Safely Fill about 8 of them.]
   */
  if(Sd_Stack<8){
    *((unsigned long*)newPointer++)=loop_timer;
    *((float*)newPointer++) = Pid_Kp_Pitch;
    *((float*)newPointer++) = Pid_Kp_Roll;
    for(byte i=0;i<3;i++){*((float*)newPointer++) = Gyro_dps[i];}
    for(byte i=0;i<3;i++){*((float*)newPointer++) = Acc_g[i];}
    *((float*)newPointer++) = Vehicle_Pitch;
    *((float*)newPointer++) = Vehicle_Roll;
    *((int*)newPointer++) = loop_interval;
    *((byte*)newPointer++) = loop_counter;
    *((int*)newPointer++) = L_Servo_val;
    *((int*)newPointer++) = R_Servo_val;
    *((int*)newPointer++) = motor_val;
    for(byte i=0;i<2;i++){*((int*)newPointer++) = PPM_Input[i];}
    *((int*)newPointer++) = Pid_setpoint_Pitch;
    *((int*)newPointer++) = Pid_setpoint_Roll;
    *newPointer++ = '\n';//NL
    Sd_Stack++;
  }
  ///////////////////////////////////////////////////////////////////////////////////////////
  loop_counter++;//increase loop counter
  if(loop_counter>=250) loop_counter = 0;//reset loop counter
}

void PPM_Reader(){
  static byte curChannel;
  static unsigned long Last_Fall_Time;
  unsigned long Now_Time = micros();
  if(Now_Time-Last_Fall_Time>=20000||curChannel>=PPM_CHANNEL_COUNT)
    curChannel = 0;
  else{
    PPM_Input[curChannel++]=constrain((Now_Time-Last_Fall_Time-1000),0,1000);//0~1000
  }
  Last_Fall_Time = Now_Time;
}

