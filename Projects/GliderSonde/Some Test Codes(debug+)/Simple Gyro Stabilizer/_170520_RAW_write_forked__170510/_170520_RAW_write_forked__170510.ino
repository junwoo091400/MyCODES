#include <SPI.h>
#include "SdFat.h"
#include "FreeStack.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
const uint8_t chipSelect = SS;
SdFat sd;
SdFile file;
const uint32_t BLOCK_COUNT = 1800;
uint32_t bgnBlock, endBlock;
uint32_t curBlock = 0;
uint8_t* pCache;
/////////////////////////////////////////////////////////////////////
bool Sd_Init(){
if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {return false;}
sd.remove("RawWrite.txt");
if (!file.createContiguous("RawWrite.txt", 512UL*BLOCK_COUNT)){return false;}
if (!file.contiguousRange(&bgnBlock, &endBlock)){return false;}
if (!sd.card()->writeStart(bgnBlock, BLOCK_COUNT)){return false;}
//*********************NOTE**************************************
// NO SdFile calls are allowed while cache is used for raw writes
//***************************************************************
pCache = (uint8_t*)sd.vol()->cacheClear();  
memset(pCache,' ',512);
pCache[510] = '\r';
pCache[511]='\n';//Last should be New Line.
}
//
bool Sd_Write(){if(curBlock<BLOCK_COUNT){if (sd.card()->writeData(pCache)){curBlock++; return true;}; return false; } else{Sd_Stop();} }

void  Sd_Stop(){while(!sd.card()->writeStop()){delay(10);}; file.close();}
///
void Infinite_Loop(){
  int i=0;
  while(1){
    Serial.print("InfiniteLoopCount");
    Serial.println(++i);
    delay(1000);
  }
}
////////////////////////////////////////////////////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/////////////////////
int motor_val = 0;
int L_Servo_val = 0;
int R_Servo_val = 0;
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
float Acc_Pitch = 0;
float Acc_Roll = 0;
float Vehicle_Pitch = 0;
float Vehicle_Roll = 0;
float Acc_data_correction = 0.05;
float One_minus_Acc_data_correction = 1-Acc_data_correction;
////////////////////////////////////////////////////////////
bool AUTOLEVEL = true;

float Pid_Kp_Roll = 0;
float Pid_setpoint_Roll = 0;

int Pid_Output_Roll = 0;
int Pid_max_Roll = 400;

float Pid_Kp_Pitch = 0;
float Pid_setpoint_Pitch;

int Pid_Output_Pitch = 0;
int Pid_max_Pitch = 400;
//////////////////////////////////////////////////////////////
volatile int PPM_Input[PPM_CHANNEL_COUNT] = {0,0,0,0,0,0};
///////////////////////////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~///////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
void setup(){
  if(!Sd_Init()){Infinite_Loop();}
}
//////////////////////////////////////////////////////
byte Sd_step = 2;
byte Sd_Stack = 0;//when gets 7, fflush!
unsigned char* newPointer = (unsigned char*)pCache;
///////////////////////////////////////////////////////
void loop(){
  if(Sd_Stack>=7){
    if(Sd_Write()){//fflush~~~!!!!
      Sd_Stack = 0;//initialize stack count.
      newPointer = (unsigned char*)pCache;//initialize pointer
    }
  }
  if(Sd_Stack<7){
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

