#define JunwooHWANG_PPM_Enable
#include"JunwooHWANG_parent.h"

#define R_SERVO_PORT         B00001000 // PD3
#define L_SERVO_PORT         B00010000 // PD4
#define LEFT_SERVO_ON        PORTD |= L_SERVO_PORT;
#define RIGHT_SERVO_ON       PORTD |= R_SERVO_PORT;
#define LEFT_SERVO_OFF       PORTD &= ~L_SERVO_PORT;
#define RIGHT_SERVO_OFF      PORTD &= ~R_SERVO_PORT;

#define MOTOR_PORT           B00100000//PD 5
#define MOTOR_OFF            PORTD &= ~MOTOR_PORT;
#define ALL_SERVO_ON         PORTD |= (L_SERVO_PORT | R_SERVO_PORT | MOTOR_PORT);

int motor_val = 1000;
int L_Servo_val = 1500;
int R_Servo_val = 1500;

boolean getInput = true;

void setup() {
  Serial.begin(9600);
  motor_val = constrain(motor_val,1000,2000);
  L_Servo_val = constrain(L_Servo_val,1000,2000);
  R_Servo_val = constrain(R_Servo_val,1000,2000);
  DDRD |= (L_SERVO_PORT | R_SERVO_PORT | MOTOR_PORT);
  PPM_init();
  loop_timer = micros();
}
void loop() {
  /*
  if(Serial.available()){
    int b=0;
    while(Serial.available()){
      char c = Serial.read();
      if(c>='0'&&c<='9'){b = b*10 + (c-'0');}
      if(b>2000) break;
    }
    if(getInput){L_Servo_val = b;R_Servo_val = b;Serial.print("SET : ");}
    Serial.println(b);
  }
  */
  L_Servo_val = 1500;//constrain(PPM_Input[0],1000,2000);
  R_Servo_val = 1500;//constrain(PPM_Input[1],1000,2000);
  Serial.print(L_Servo_val);
  Serial.write(',');
  Serial.println(R_Servo_val);
  while(micros()-loop_timer<20000);//20ms[50Hz]
  loop_timer = micros();
  ALL_SERVO_ON
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
