//////////////////
#define PPM_PIN 2 //only 2 or 3??
#define PPM_CHANNEL_COUNT 8
#define PPM_ENDING_LENGTH 3000
volatile int PPM_Input[PPM_CHANNEL_COUNT] = {};
////////////////
void setup() {
  // put your setup code here, to run once:
  attachInterrupt(digitalPinToInterrupt(PPM_PIN),PPM_Reader,FALLING);//Interrupt at 'falling'
  Serial.begin(115200);
  Serial.println("Reads in PPM Signal CODE.");
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0;i<PPM_CHANNEL_COUNT;i++){
    //Serial.print("Channel_");
    //Serial.print(i);
    Serial.print(PPM_Input[i]);
    Serial.print(',');
  }
  Serial.println();
  delay(500);
}

void PPM_Reader(){
  static byte curChannel;
  static unsigned long Last_Fall_Time;
  unsigned long Now_Time = micros();
  
  if(Now_Time-Last_Fall_Time>=PPM_ENDING_LENGTH||curChannel>=PPM_CHANNEL_COUNT) curChannel = 0;
  else PPM_Input[curChannel++]=(Now_Time-Last_Fall_Time);
  
  Last_Fall_Time = Now_Time;
}
