#include<Wire.h>
#include"JunwooHWANG_parent.h"
#include"SdFat.h"
////////////////////////////////////////////////////
bool DEBUG_ENABLE = true;
#define DEBUG_PIN_PORT B10000000//DEBUG PIN at PD7!
#define DEBUG_PIN_ON PORTD |= DEBUG_PIN_PORT;
#define DEBUG_PIN_OFF PORTD &= ~DEBUG_PIN_PORT;
///////////////////////////////////////////////////
/*
 * Channel 1~6:
 * 1: Roll => Used
 * 2: Pitch => Used
 * 3: Throttle  => For Throttle
 * 4: Yaw => Don't use
 * 5: Dial1 => Roll Kp
 * 6: Dial2 => Pitch Kp
 * 7: ...
 * 8: ...
 */
////////////////////////////////////////////////////
bool AUTOLEVEL = true;
///////////////////////////////////
#define DEG_TO_RAD 0.017453292f
#define RAD_TO_DEG 57.295779f
///////////////////////////////////
#define L_SERVO_PORT         B00001000 // PD3
#define R_SERVO_PORT         B00010000 // PD4
#define LEFT_SERVO_ON        PORTD |= L_SERVO_PORT;
#define RIGHT_SERVO_ON       PORTD |= R_SERVO_PORT;
#define LEFT_SERVO_OFF       PORTD &= ~L_SERVO_PORT;
#define RIGHT_SERVO_OFF      PORTD &= ~R_SERVO_PORT;

#define MOTOR_PORT           B00100000//PD 5
#define MOTOR_OFF            PORTD &= ~MOTOR_PORT;
#define ALL_SERVO_ON         PORTD |= (L_SERVO_PORT | R_SERVO_PORT | MOTOR_PORT);

int motor_val = 0;
int L_Servo_val = 0;
int R_Servo_val = 0;
/////////////////////////////////////////////////////////////
const uint8_t chipSelect = SS;
SdFat sd;
SdFile file;
const uint32_t BLOCK_COUNT = 1563;//250/8 = 31.25 block per second. => 50 second = 1562.5 block. 1563!
//const uint32_t BLOCK_COUNT
uint32_t bgnBlock, endBlock;
uint32_t curBlock = 0;
uint8_t* pCache;
//////////////////////////////////////////////////////////////
boolean Sd_init(){
  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {return false;}
  return true;
}
boolean Sd_makenewFile(){
  if(DEBUG_ENABLE) Serial.print(F("Checking file-able-name... :"));
  unsigned long TEMPstart = micros();
  char fileName[7] = "DATA00";
  while (sd.exists(fileName)) {
    if (fileName[5] != '9') {fileName[5]++;}
    else if (fileName[4] != '9') {fileName[5] = '0';fileName[4]++;} 
    else {return false;}
  }
  unsigned long TEMPmiddle = micros();
  if(DEBUG_ENABLE) Serial.println(TEMPmiddle-TEMPstart);
  if (!file.createContiguous(fileName, 512UL*BLOCK_COUNT)){if(DEBUG_ENABLE)Serial.println(F("No cC")); return false;}//file~~
  if (!file.contiguousRange(&bgnBlock, &endBlock)){if(DEBUG_ENABLE)Serial.println(F("No cRange")); return false;}
  if (!sd.card()->writeStart(bgnBlock, BLOCK_COUNT)){if(DEBUG_ENABLE)Serial.println(F("No wS")); return false;}
  //*********************NOTE**************************************
  // NO SdFile calls are allowed while cache is used for raw writes
  //***************************************************************
  pCache = (uint8_t*)sd.vol()->cacheClear();  //'cache' is used!
  memset(pCache,' ',512);
  pCache[510] = '\r';
  pCache[511]='\n';//Last should be New Line.
  if(DEBUG_ENABLE) Serial.println(micros()-TEMPmiddle);
  return true;//Don't forget to return true when it is ok...
}

boolean Sd_Write(){
  if(curBlock==BLOCK_COUNT){pCache[505] = '$';pCache[507] = 'E';pCache[508] = 'N';pCache[509] = 'D';pCache[510] = '\r';pCache[511]='\n';}
  if (sd.card()->writeData(pCache)){curBlock++; return true;};
  return false;
  }

boolean Sd_Stop(){unsigned long Start = micros();while(!sd.card()->writeStop()){if((micros()-Start)>=1000000)return false;}; file.close();return true;}//Max 1s TRY.

/////////////////////////////////////////////////////////////

void update_PID_values(){
  Pid_setpoint_Roll = (float)(PPM_Input[0]-1500)/15;//33.3 deg Max.
  Pid_setpoint_Pitch = (float)(PPM_Input[1]-1500)/15;
  
  if(AUTOLEVEL){
  Pid_setpoint_Roll -= Vehicle_Roll;//if correct, max 66.6(=33.3-(-33.3)).
  Pid_setpoint_Pitch -= Vehicle_Pitch;
  } //I wish it can be like 250 deg/s or so. => Times 4. 66.6*4 = 266.4[deg/s] MAX.
  
  Pid_setpoint_Roll = constrain(Pid_setpoint_Roll*4,-270,270);//270 deg/s MAX
  Pid_setpoint_Pitch = constrain(Pid_setpoint_Pitch*4,-270,270);

  L_Servo_val = (0.5)*(Pid_setpoint_Roll - Pid_setpoint_Pitch);
  R_Servo_val = (0.5)*(Pid_setpoint_Roll + Pid_setpoint_Pitch);
  
  //if(ROLL_TUNING){
    Pid_Kp_Roll = (float)constrain( (PPM_Input[4]-1000) , 0, 1000) / 1000;//0~1000 => (0~1.0)
    //Pid_Ki_Roll = (float)PPM_Input[5]/1000;// => (0~1.0)
  //}
  //else{
    Pid_Kp_Pitch = (float)constrain( (PPM_Input[5]-1000) , 0, 1000) / 1000;//0~1000 => (0~1.0)
    //Pid_Ki_Pitch = (float)PPM_Input[5]/1000;// => (0~1.0)
  //}
}
//////////////////////////////////////////////////////////////////////////////
//byte Calculation_step = 1;//250Hz
byte Output_step = 5;//50Hz
////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  DDRD |= (DEBUG_PIN_PORT);
  DDRD |= (L_SERVO_PORT | R_SERVO_PORT | MOTOR_PORT);
  Wire.begin();
  TWBR = 12; //400kHz I2c
  if(DEBUG_ENABLE){
    Serial.begin(19200);
    Serial.println(F("Hello Debugger! Have a nice Experiment!"));
    Serial.println(F("Current Baud Rate is 19200"));
  }
  bool ERROR_OMG = false; 
  if(!Gyro_init()){if(DEBUG_ENABLE) Serial.println(F("Gyro init Failed!")); ERROR_OMG = true;}
  if(!Acc_init()){if(DEBUG_ENABLE) Serial.println(F("Acc init Failed!")); ERROR_OMG = true;}
  if(!Sd_init()){if(DEBUG_ENABLE) Serial.println(F("Sd init Failed!")); ERROR_OMG = true;}
  if(!Sd_makenewFile()){if(DEBUG_ENABLE) Serial.println(F("Sd makenewFile Failed!")); ERROR_OMG = true;}
  if(ERROR_OMG){ while(1){ if(DEBUG_ENABLE) Serial.println(F("ERROR!")); delay(1000); } }

  PPM_init();
  
  Acc_Pitch = 0;
  Acc_Roll = 0;
  Vehicle_Pitch = 0;
  Vehicle_Roll = 0;
  
  if(DEBUG_ENABLE) Serial.println(F("NO ERRORS, PRECEEDING TO THE LOOP!"));
  loop_timer = micros();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
byte Sd_Stack = 0;
unsigned char* newPointer = (unsigned char*)pCache;
boolean Sd_whileWriting = false;
boolean Sd_Error = false;
/////////////////////////////////////////////////////////////////////
void loop() {
  while(micros()-loop_timer<4000);
  loop_interval = micros()-loop_timer;
  loop_timer = loop_interval+loop_timer;
  /////////////////////////////////////////////////////
    DEBUG_PIN_ON
    Gyro_readData();
    //DEBUG_PIN_OFF
    //DEBUG_PIN_ON
    Acc_readData();
    DEBUG_PIN_OFF
    DEBUG_PIN_ON
    Attitude_calculate();
    //DEBUG_PIN_OFF
    //DEBUG_PIN_ON
    update_PID_values();
    //DEBUG_PIN_OFF
    //DEBUG_PIN_ON
    Pid_calculate_Output();
    DEBUG_PIN_OFF
    
  if(loop_counter%Output_step==0){//Output pulse.
    ALL_SERVO_ON
    L_Servo_val = constrain(L_Servo_val + (0.5)*(Pid_Output_Roll - Pid_Output_Pitch),-500,500);//Already have set values... so Plus PID output...
    R_Servo_val = constrain(R_Servo_val + (0.5)*(Pid_Output_Roll + Pid_Output_Pitch),-500,500);//Constrain These values...
    motor_val = constrain(PPM_Input[3],1000,2000);
    L_Servo_val += 1500;
    R_Servo_val += 1500;
    byte count = 0;
    unsigned long Servo_loop_timer = micros();//Because, the 'calculation phase' eats up alot of time... ne timer...
    while(count<3){
      count = 0;
      unsigned long Pulse_Loop_Time = micros();
      if(Pulse_Loop_Time-Servo_loop_timer>=L_Servo_val) {LEFT_SERVO_OFF; count++;}
      if(Pulse_Loop_Time-Servo_loop_timer>=R_Servo_val) {RIGHT_SERVO_OFF; count++;}
      if(Pulse_Loop_Time-Servo_loop_timer>=motor_val)   {MOTOR_OFF; count++;}
    }
  }
  else if(!Sd_Error){
    
    if(Sd_whileWriting && Sd_Stack>=8){
      if(Sd_Write()){
        Sd_Stack = 0; newPointer = (unsigned char*)pCache;//initialize pointer
        if(++curBlock>=BLOCK_COUNT){if(DEBUG_ENABLE) Serial.println(F("curBlock reacehd BLOCK_COUNT... Sd_Stop()!")); if(!Sd_Stop()) Sd_Error = true; Sd_whileWriting = false;}
      }
      else{//Sd_Write failed
        if(DEBUG_ENABLE) Serial.println(F("Sd Write Failed.. stop recording")); 
        if(!Sd_Stop()){Sd_Error = true; if(DEBUG_ENABLE) Serial.println(F("Stoppint File write Failed..."));}
      }
    }
    
    else if(Sd_whileWriting && PPM_Input[2] < 1050 && PPM_Input[3] < 1050){//Does User want to STOP recording?
      Sd_Stack=0; if(DEBUG_ENABLE) Serial.println(F("User wants to Stop Recroding..."));
      if(!Sd_Stop()){Sd_Error = true; if(DEBUG_ENABLE) Serial.println(F("Stoppint File write Failed..."));}
      Sd_whileWriting = false;
    }
    
    else if(!Sd_whileWriting && PPM_Input[2] > 1950 && PPM_Input[3] > 1950){//Deos User want to START recording?
      Sd_Stack = 0; newPointer = (unsigned char*)pCache; curBlock = 0; if(DEBUG_ENABLE) Serial.println(F("User wants to Start Recroding..."));
      if(!Sd_makenewFile()){ Sd_Error = true; if(DEBUG_ENABLE) Serial.println(F("Making new File Failed...")); }
    }
    
 }
  ///////////////////////////pCache charger....///////////////////////////////////////////////
  if(Sd_Stack < 8 && Sd_whileWriting){
    *newPointer++ = '@';//Start Header           (1)--------
    *((unsigned long*)newPointer++)=loop_timer;//(4)        |
    *((int*)newPointer++) = loop_interval;     //(2)        | 8 byte
    *((byte*)newPointer++) = loop_counter;     //(1)--------- 
    
    *((float*)newPointer++) = Pid_Kp_Pitch;    //(4)--------
    *((float*)newPointer++) = Pid_Kp_Roll;     //(4)--------| 8 byte
    
    for(byte i=0;i<3;i++){*((float*)newPointer++) = Gyro_dps[i];}   //(12)-----
    for(byte i=0;i<3;i++){*((float*)newPointer++) = Acc_g[i];}      //(12)    |
    *((float*)newPointer++) = Vehicle_Pitch;   //(4)                          | 32 byte
    *((float*)newPointer++) = Vehicle_Roll;    //(4)                      -----
    
    *((int*)newPointer++) = L_Servo_val;       //(2)------
    *((int*)newPointer++) = R_Servo_val;       //(2)      | 6 byte
    *((int*)newPointer++) = motor_val;         //(2)------
    
    for(byte i=0;i<2;i++){*((int*)newPointer++) = PPM_Input[i];}    //(4) | 4 byte
    
    *((int*)newPointer++) = Pid_Output_Pitch;  //(2)-------
    *((int*)newPointer++) = Pid_Output_Roll;   //(2)       |
    *newPointer++ = '\r';                      //(1)       | 6 byte
    *newPointer++ = '\n';                      //(1)-------

    //TOTAL : 64 byte. => 8 times : 512 byte.
    Sd_Stack++;
   }
  loop_counter++;//increase loop counter
  if(loop_counter>=250){if(DEBUG_ENABLE){Serial.print(L_Servo_val);Serial.write(',');Serial.println(R_Servo_val);
                                     Serial.print(Pid_Kp_Roll);Serial.write(',');Serial.println(Pid_Kp_Pitch);
                                     Serial.print(Pid_Output_Roll);Serial.write(',');Serial.println(Pid_Output_Pitch);}
                                     loop_counter = 0;}//reset loop counter
}

