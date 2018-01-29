#define SerialPrint_Name_Value(name) \
Serial.println(F(#name)); \
Serial.println(name,HEX);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  SerialPrint_Name_Value(TCCR2A)
  SerialPrint_Name_Value(TCCR2B)
  
  SerialPrint_Name_Value(TCNT2)
  
  SerialPrint_Name_Value(OCR2A)
  SerialPrint_Name_Value(OCR2B)
  
  SerialPrint_Name_Value(TIMSK2)
  
  SerialPrint_Name_Value(TIFR2)
  
  SerialPrint_Name_Value(ASSR)
  SerialPrint_Name_Value(GTCCR)

  TCCR2A = 0x00;//WGM[1:0] all set to 0.
  TCCR2B = 0x01;//WGM[2] set to 0 So We are in Normal Mode Now. CS(ClockSelect) set to 1(NO Prescaling -> MAX Resolution).
}

const byte StopAt = 5;
byte curAt = 0;

void loop() {
  if(curAt < StopAt)
  {
  byte Start = TCNT2;
  delayMicroseconds(8);
  byte End = TCNT2;
  Serial.print("After 8us Delay, TCNT2 changed by : ");
  Serial.print((End-Start)%256);
  Serial.print(" . FROM ");
  Serial.print(Start);
  Serial.print(" TO ");
  Serial.println(End);
  ++curAt;
  }
  delay(1000);
}
