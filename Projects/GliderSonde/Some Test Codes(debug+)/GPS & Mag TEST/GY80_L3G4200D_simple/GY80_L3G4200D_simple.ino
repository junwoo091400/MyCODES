#include<Wire.h>

#define GYRO_ADDRESS 0x69//address of gyro(L3G4200D,SD0pin(4) held HIGH)

#define GYRO_CALIBRATE_COUNT 1000 // DONT, PUT, SEMICOLON, AFTER MACRO. DONT DONT DONT! You wasted my 30 minute !! 1703061735
#define GYRO_THRESHOLD_MULTIPLE 4//Determines Threshold Value

float Gyro_dps_per_digit = 0.0175;//0.0175 degree/s = 1 digit.
int Gyro_RAW[3];//X,Y,Z value straight from the sensor.
int Gyro_Cal[3];//calibration data
float Gyro_dps[3];//final result
int Gyro_Threshold[3];//threshold for Gyro_RAW
int Gyro_Cal_count = 0;

void setup() {
  pinMode(3,OUTPUT);
Wire.begin();
Serial.begin(9600);

if(!Sensors_Init()){
    Serial.println("Sensor Init. failed");
    while(1) delay(100);//Infinite Loop
  }
  delay(100);//give some time before going into the loop!
}
unsigned long loop_timer = 0;
//int interval;//stores maximum 65.536ms and that is too cruel! So...
unsigned long interval;//now!
unsigned long now_time=0;

float Dir[3]={0,0,0};

void loop() {
Gyro_ReadData();
now_time = micros();
interval = now_time-loop_timer;
loop_timer = now_time;
for(int i=0;i<3;i++){
    Serial.print(Gyro_dps[i]);
    Serial.print(",");
  }
  for(int i=0;i<3;i++){
    Dir[i]+=Gyro_dps[i]*((float)interval/1000000);
    Serial.print(Dir[i]);
    Serial.print(",");
  }
  Serial.println(interval);
delay(100);
}

boolean Sensors_Init(){
    //1.see if sensors are present.
    if(!Sensor_Present_Check(GYRO_ADDRESS))
      return false;
    
    //2. Set correct Values for each register
    if(!WriteByte(GYRO_ADDRESS,0x23,B10010000))//<CTRL_REG4> set FS10 bit(5,4) = 0,1. AND BDU bit(7) =1<BLOCK DATA UPDATE>. (500 dps scale.)
      return false;
   if(!WriteByte(GYRO_ADDRESS,0x20,B00001111))//<CTRL_REG1> set PD bit(3) to 1.(normal/sleep mode)
      return false;

   //3. Calibrate Sensor.
   Gyro_Calibrate();
   return true;
}
void Gyro_ReadData(){
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

void Gyro_Calibrate(){
  long SUM[3]={};//place to store sum of all data
  unsigned long Squared[3]={};//place to store all squared data, at GYRO_CALIBRATE_COUNT==1000, each squared shouldn't exceed: (2^32-1)/1000.
  //Which means that each number shouldn't exceed 2072.43. and that is, 36.267 deg/s. So I consider this safe for now.
 //!!!!!!!!!!!!!!!! Doing ={} Ensures that 'SUM' and 'Squared' all set to 0. But to even ensure, I am going to reset them AGAIN.
 for(int i=0;i<3;i++){SUM[i] = 0; Squared[i] = 0;}
 //Because this gave me so much pain in the ass. Almost another HALF a hour,, 1703061754...
  for(Gyro_Cal_count=0;Gyro_Cal_count<GYRO_CALIBRATE_COUNT;Gyro_Cal_count++){
     Gyro_ReadData();
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
boolean Sensor_Present_Check(byte address){ Wire.beginTransmission(address);return Wire.endTransmission()==0?true:false;}

boolean WriteByte(byte address,byte reg,byte data){
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
  Wire.requestFrom(address,1);//OMG!I request to 'address'!!! Not the 'reg'!!! Wasted 0.5 Hour :) 170305 // actually I also didn't set MSB that was a problem at x,y,z
  while(!Wire.available());//wati until '1 byte' is recieved.
  return Wire.read();
}

