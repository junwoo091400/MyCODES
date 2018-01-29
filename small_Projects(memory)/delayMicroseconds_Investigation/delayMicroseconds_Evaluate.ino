
byte evaluateDelayMicroseconds(byte microseconds){
  
  if(microseconds >= 16)
    return;//Can't do more than 15 microseconds. TCNT2 overflows, and will give value of 0 or little over that. Which is WRONG(should be 256 -> Impossible).
  
  delay(1);//Take some breath.
  
  byte Start = TCNT2;
  delayMicroseconds(microseconds);
  byte Stop = TCNT2;
  
  return (Stop - Start + 256);
}

byte evaluateNOTHING(){
  //Shows how many Clock-cycles Needed for Just NO operation between Reading TCNT2 Register.
  byte Start = TCNT2;
  byte Stop = TCNT2;
  return (Stop - Start + 256);
}

float getAverage(byte* list, int count){
  unsigned int Sum = 0;
  for(int i = 0 ; i < count ; i++)
    Sum += list[i];
  return ((float)Sum / count);
}

float getStdDev(byte* list, int count){
  unsigned long SquaredSum = 0;
  for(int i = 0 ; i < count ; i++)
    SquaredSum += pow(list[i],2);
    
  float SqSumAvg = (float)SquaredSum / count; // Get Averaged Square Sum.
  
  float Average = getAverage(list,count);
  float AvgSquared = Average * Average;

  return sqrt(SqSumAvg - AvgSquared);
}

void PrintStatistics(byte* arr, int count){
  for(int i = 0 ; i < count ; i++){
    Serial.print(arr[i]);
    Serial.print(", ");
  }
  Serial.print("Average : ");
  Serial.print(getAverage(arr,count));
  Serial.print(" Stddev : ");
  Serial.println(getStdDev(arr,count));
}
///////////////////////////////////////////////////////////

#define TEST_COUNT 5
byte DataList[TEST_COUNT] = {};//array of 0

byte AvgMemory[15] = {};//Store 'Avg' value for each microseconds experiment.

void setup() {
  Serial.begin(115200);

  Serial.println(F("<<delayMicroseconds() Experiment>>\n"));
  Serial.println(F("First, set Timer 2 accordingly."));
  TCCR2A = 0x00;//WGM[1:0] all set to 0.
  TCCR2B = 0x01;//WGM[2] set to 0 So We are in Normal Mode Now. CS(ClockSelect) set to 1(NO Prescaling -> MAX Resolution).
  Serial.println(F("NOW, Timer2 runs on 16[MHz] = FULL resolution. And is on Normal Operation Mode(Clock value only rises and overflows)\n"));

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Serial.println(F("OK, for basis, let's see how much Clock cycles are wasted without any line between Reading TCNT2 Register."));

  for(byte i = 0 ; i < TEST_COUNT ; i++)
    DataList[i] = evaluateNOTHING();
  PrintStatistics(DataList, TEST_COUNT);

  byte tick_when_nothing = (byte)getAverage(DataList, TEST_COUNT);

  Serial.print(F("\nTick when Nothing Executed = "));
  Serial.println(tick_when_nothing);

  ///////////////////////////////////////////////////////////

  Serial.println(F("\nNOW, testing delayMicrsoeconds()\n"));

  for(byte micro = 0 ; micro < 15 ; micro++){
    
    Serial.print(micro);
    Serial.print(": ");
    
    for(byte i = 0 ; i < TEST_COUNT ; i++)
      DataList[i] = evaluateDelayMicroseconds(micro);

    PrintStatistics(DataList, TEST_COUNT);
    
    AvgMemory[micro] = (byte)getAverage(DataList, TEST_COUNT) - tick_when_nothing;//Save.
  }
  
  Serial.println("\nFor CSV graphing VV\n");
  Serial.println("delayMicro, Clock Cycle, Expected, Error,RawMicroSeconds,ExpectedRawMicroSeconds");
  for(byte micro = 0 ; micro < 15 ; micro++){
    Serial.print(micro);
    Serial.print(",");
    Serial.print(AvgMemory[micro]);
    Serial.print(",");
    Serial.print(micro * 16);//Expected 'Tick' value.
    Serial.print(",");
    Serial.print(AvgMemory[micro] - 16 * micro);
    
    Serial.print(",");
    Serial.print((float)AvgMemory[micro] / 16);
    Serial.print(",");
    Serial.print(micro);
    
    Serial.println("");
  }
}

void loop() {
  delay(1000);
}
