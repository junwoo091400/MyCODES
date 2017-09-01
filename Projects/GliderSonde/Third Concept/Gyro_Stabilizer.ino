#include<Wire.h>
#define JunwooHWANG_Wire_Enable
#define JunwooHWANG_Gyro_Enable
#define JunwooHWANG_Acc_Enable
#define JunwooHWANG_Attitude_Enable
#define JunwooHWANG_PID_Enable
#define JunwooHWANG_PPM_Enable

#include"JunwooHWANG_parent.h"
////////////////////////////////////////////////////
#define HEARTBEAT_PIN_PORT B01000000//PD 6
#define HEARTBEAT_PIN_ON PORTD |= HEARTBEAT_PIN_PORT;
#define HEARTBEAT_PIN_OFF PORTD &= ~HEARTBEAT_PIN_PORT;
boolean HEARTBEAT_ON = false;
////////////////////////////////////////////////////
bool DEBUG_ENABLE = false;
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
 * 5: Dial1 => P term
 * 6: Dial2 => I term
 * 7: ...
 * 8: ...
 */
////////////////////////////////////////////////////
bool AUTOLEVEL = true;
///////////////////////////////////
#define DEG_TO_RAD 0.017453292f
#define RAD_TO_DEG 57.295779f
///////////////////////////////////
#define R_SERVO_PORT         B00001000 // PD3
#define L_SERVO_PORT         B00010000 // PD4
#define LEFT_SERVO_ON        PORTD |= L_SERVO_PORT;
#define RIGHT_SERVO_ON       PORTD |= R_SERVO_PORT;
#define LEFT_SERVO_OFF       PORTD &= ~L_SERVO_PORT;
#define RIGHT_SERVO_OFF      PORTD &= ~R_SERVO_PORT;

#define L_SERVO_MIN -200
#define L_SERVO_MAX 200
#define R_SERVO_MIN -200
#define R_SERVO_MAX 200

#define MOTOR_PORT           B00100000//PD 5
#define MOTOR_OFF            PORTD &= ~MOTOR_PORT;
#define ALL_SERVO_ON         PORTD |= (L_SERVO_PORT | R_SERVO_PORT | MOTOR_PORT);

int motor_val = 0;
int L_Servo_val = 0;
int R_Servo_val = 0;
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

  L_Servo_val = constrain( (0.5)*(Pid_setpoint_Roll - Pid_setpoint_Pitch) ,L_SERVO_MIN,L_SERVO_MAX );
  R_Servo_val = constrain( (0.5)*(Pid_setpoint_Roll + Pid_setpoint_Pitch) ,R_SERVO_MIN,R_SERVO_MAX );
  
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
byte Calculation_step = 1;//250Hz
byte Output_step = 5;//50Hz
boolean EMERGENCY_MANUAL_MODE = false;
byte Emergency_manual_mode_counter = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  DDRD |= (DEBUG_PIN_PORT);
  DDRD |= (L_SERVO_PORT | R_SERVO_PORT | MOTOR_PORT | HEARTBEAT_PIN_PORT);
  pinMode(11,OUTPUT);
  Wire.begin();
  TWBR = 12; //400kHz I2c
  if(DEBUG_ENABLE){
    Serial.begin(19200);
    Serial.println(F("Hello Debugger! Have a nice Experiment!"));
    Serial.println(F("Current Baud Rate is 19200"));
  }
  bool ERROR_OMG = false; 
  if(!Gyro_init()){if(DEBUG_ENABLE) Serial.println(F("Gyro Init Failed!")); ERROR_OMG = true;}
  if(!Acc_init()){if(DEBUG_ENABLE) Serial.println(F("Acc Init Failed!")); ERROR_OMG = true;}
  if(ERROR_OMG){ while(1){ if(DEBUG_ENABLE) Serial.println(F("ERROR!")); delay(1000); } }

  PPM_init();
  
  Acc_Pitch = 0;
  Acc_Roll = 0;
  Vehicle_Pitch = 0;
  Vehicle_Roll = 0;
  
  if(DEBUG_ENABLE) Serial.println(F("NO ERRORS, PRECEEDING TO THE LOOP!"));
  loop_timer = micros();
}

void loop() {
  while(micros()-loop_timer<4000);
  loop_interval = micros()-loop_timer;
  loop_timer = loop_interval+loop_timer;
  /////////////////////////////////////////////////////
  if(!EMERGENCY_MANUAL_MODE){//Sensor Reading&Calculation
    DEBUG_PIN_ON
    if(!Gyro_readData()||!Acc_readData()){ EMERGENCY_MANUAL_MODE = true;}
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
  }
  if(loop_counter%Output_step==0){//Output pulse.
    ALL_SERVO_ON
    if(EMERGENCY_MANUAL_MODE){L_Servo_val = 0; R_Servo_val = 0; Pid_Output_Roll = (PPM_Input[0]-1500); Pid_Output_Pitch = (PPM_Input[1]-1500);}
    
    L_Servo_val = constrain(L_Servo_val + (0.5)*(Pid_Output_Roll - Pid_Output_Pitch),L_SERVO_MIN,L_SERVO_MAX);//Already have set values... so Plus PID output...
    R_Servo_val = constrain(R_Servo_val + (0.5)*(Pid_Output_Roll + Pid_Output_Pitch),R_SERVO_MIN,L_SERVO_MAX);//Constrain These values...
    
    if(!EMERGENCY_MANUAL_MODE) motor_val = constrain(PPM_Input[2],1000,1700);
    else motor_val = constrain(PPM_Input[2],1000,1200);//limit at emergency mode...
    
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
  if(loop_counter>=250){if(DEBUG_ENABLE){Serial.print(L_Servo_val);Serial.write(',');Serial.println(R_Servo_val);
                                     Serial.print(Pid_Kp_Roll);Serial.write(',');Serial.println(Pid_Kp_Pitch);
                                     Serial.print(Pid_Output_Roll);Serial.write(',');Serial.println(Pid_Output_Pitch);}
                                     loop_counter = 0;}//reset loop counter
  if(EMERGENCY_MANUAL_MODE){
    Emergency_manual_mode_counter++;
    if(Emergency_manual_mode_counter%10==0){
      HEARTBEAT_ON = !HEARTBEAT_ON;
      if(HEARTBEAT_ON) HEARTBEAT_PIN_ON
      else HEARTBEAT_PIN_OFF
      Emergency_manual_mode_counter = 0;
    }
  }
  else{
    HEARTBEAT_ON = !HEARTBEAT_ON;
    if(HEARTBEAT_ON){ HEARTBEAT_PIN_ON}
    else {HEARTBEAT_PIN_OFF}
  }
}

