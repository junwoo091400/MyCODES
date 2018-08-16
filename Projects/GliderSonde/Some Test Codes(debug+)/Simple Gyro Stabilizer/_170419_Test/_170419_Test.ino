#include<Wire.h>
#include"JunwooHWANG.h"
////////////////////////////////////////////////////
bool DEBUG_MODE = true;
#define DEBUG_PIN_PORT B00000100//DEBUG PIN at PD2!
#define DEBUG_PIN_ON PORTD |= DEBUG_PIN_PORT;
#define DEBUG_PIN_OFF PORTD &= ~DEBUG_PIN_PORT;
///////////////////////////////////////////////////
#define PPM_PIN 9
#define PPM_CHANNEL_COUNT 6
volatile int PPM_Input[PPM_CHANNEL_COUNT] = {};
/*
 * Channel 1~6:
 * 1: Roll => Used
 * 2: Pitch => Used
 * 3: Throttle  => For Throttle
 * 4: Yaw => Don't use
 * 5: Dial1 => P term
 * 6: Dial2 => I term
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

void update_PID_values(){
  Pid_setpoint_Roll = ((float)PPM_Input[0]-500)/15;//33.3 deg Max.
  Pid_setpoint_Pitch = ((float)PPM_Input[1]-500)/15;//!!!!!!!!!!!!!!!!!Make sure when STICKDOWN, Value UP!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
  if(AUTOLEVEL){
  Pid_setpoint_Roll -= Vehicle_Roll;//if correct, max 66.6(=33.3-(-33.3)).
  Pid_setpoint_Pitch -= Vehicle_Pitch;
  } //I wish it can be like 250 deg/s or so. => Times 4. 66.6*4 = 266.4[deg/s] MAX.
  
  Pid_setpoint_Roll = constrain(Pid_setpoint_Roll*4,-270,270);//270 deg/s MAX
  Pid_setpoint_Pitch = constrain(Pid_setpoint_Pitch*4,-270,270);
  //if(ROLL_TUNING){
    Pid_Kp_Roll = (float)PPM_Input[4]/200;//0~1000 => (0~5.0)
    //Pid_Ki_Roll = (float)PPM_Input[5]/1000;// => (0~1.0)
  //}
  //else{
    Pid_Kp_Pitch = (float)PPM_Input[5]/200;//0~1000 => (0~5.0)
    //Pid_Ki_Pitch = (float)PPM_Input[5]/1000;// => (0~1.0)
  //}
}
//////////////////////////////////////////////////////////////////////////////
byte Calculation_step = 1;//250Hz
byte Output_step = 5;//50Hz
////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  DDRD |= (DEBUG_PIN_PORT);
  DDRD |= (L_SERVO_PORT | R_SERVO_PORT | MOTOR_PORT);
  Wire.begin();
  TWBR = 12; //400kHz I2c
  if(DEBUG_MODE){
    Serial.begin(19200);
    Serial.println("Hello Debugger! Have a nice Experiment!");
    Serial.println("Current Baud Rate is 19200");
  }
  bool ERROR_OMG = false; 
  if(!Gyro_Init()){if(DEBUG_MODE) Serial.println("Gyro Init Failed!"); ERROR_OMG = true;}
  if(!Acc_Init()){if(DEBUG_MODE) Serial.println("Acc Init Failed!"); ERROR_OMG = true;}
  if(ERROR_OMG){ while(1){ if(DEBUG_MODE) Serial.println("ERROR!"); delay(1000); } }
  
  Acc_Pitch = 0;
  Acc_Roll = 0;
  Vehicle_Pitch = 0;
  Vehicle_Roll = 0;
  
  attachInterrupt(digitalPinToInterrupt(PPM_PIN),PPM_Reader,FALLING);//Interrupt at 'falling'
  if(DEBUG_MODE) Serial.println("NO ERRORS, PRECEEDING TO THE LOOP!");
  loop_timer = micros();
}

void loop() {
  while(micros()-loop_timer<4000);
  loop_interval = micros()-loop_timer;
  loop_timer = loop_interval+loop_timer;
  /////////////////////////////////////////////////////
  if(loop_counter%Calculation_step==0){//Sensor Reading&Calculation
    DEBUG_PIN_ON
    Gyro_readData();
    //DEBUG_PIN_OFF
    //DEBUG_PIN_ON
    Acc_readData();
    DEBUG_PIN_OFF
    DEBUG_PIN_ON
    calculate_Vehicle_Attitude();
    //DEBUG_PIN_OFF
    //DEBUG_PIN_ON
    update_PID_values();
    //DEBUG_PIN_OFF
    //DEBUG_PIN_ON
    Pid_calculate_Output();
    DEBUG_PIN_OFF
  }
  if(loop_counter%Output_step==1){//Output pulse.
    ALL_SERVO_ON
    L_Servo_val = constrain((0.5)*(Pid_Output_Roll - Pid_Output_Pitch),-500,500);
    R_Servo_val = constrain((0.5)*(Pid_Output_Roll + Pid_Output_Pitch),-500,500);//Constrain These values...
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

