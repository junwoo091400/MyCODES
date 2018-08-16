void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int A;
  int example;
  if(Serial.available()){
    A=0;
    while(Serial.available()){
      example = Serial.read();
      if(example>='0'&&example<='9'){
      A = A*10 + (example-'0');}
    }
    Serial.print("YOU SAID:");
  Serial.print(A);
  Serial.print("(DEC), (HEX) : ");
  Serial.println(A,HEX);
  for(int i=0;i<=16;i+=4){
    long STH = 1;
    unsigned long K=STH*A<<i;
    
    Serial.print(i);
    Serial.print(" Times Shift : ");
    Serial.println(K,HEX);
  }
  short A = 0xD1F6;
  short B = 0x1750;
  short C = 0x0A8F;
  Serial.println((unsigned long)A<<11,HEX);
  Serial.println(B,HEX);
  Serial.println(C,HEX);
  long X2 = ((long)A<<11)/(B+C);
  Serial.println(X2,HEX);
  }
  delay(100);
}
