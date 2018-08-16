#include<Wire.h>
void setup() {
  // put your setup code here, to run once:
Wire.begin();
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
byte Result;
for(byte A=1;A<128;A++){
  Wire.beginTransmission(A);
  Result=Wire.endTransmission();
  if(Result==0){
  Serial.print("Found at : ");
  Serial.println(A,HEX);
  }
  else if(Result==4){
  Serial.print("Wow, unknown error! at : ");
  Serial.println(A,HEX);
  }
}
delay(1000);
Serial.println("Try Again..");
}
