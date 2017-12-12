#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include "RTClib.h"

WiFiClient client;
RTC_DS1307 rtc;

#define DEBUG

#include "JW_Compression.h"
#include "JW_ThingSpeak.h"
#include "JW_Scheduler.h"

uint8_t lastWeMosState = IDLE; // SchedulerStatus_e
uint8_t WeMosState = IDLE; // State of the Mahcine...
extern Data_t data;

char ssid[20];
char password[20];

#define SENSOR_PIN_1 D0
#define SENSOR_PIN_2 D5
#define SENSOR_PIN_3 D6
#define SENSOR_PIN_4 D7
#define SENSOR_PIN_5_A A0

DateTime MachineTime;
uint8_t MachineDayOfWeek = 8;

int timeStamp_count = 0;
uint8_t lastVal = 0;
uint8_t Val_sum_mask = 0b00011111;

bool isWiFiHealthy = false;
bool isClientConnected = false;
bool isRtcTimeHealthy = false;

unsigned long lastWiFiUpdate = 0;
unsigned long lastFootPadUpdate = 0;
unsigned long lastRtcUpdate = 0;
unsigned long lastSerialUpdate = 0;

uint16_t Sum_highest_MSB = 0;
uint16_t avg_highest_MSB_centi = 0;

bool forced_Val_print = false; // CLI updated variable.
unsigned long lastForcedValprint = 0;

uint8_t getHighestMSB(uint8_t tempData)
{
  if(tempData >= 1<<4) return 5;
  else if(tempData >= 1<<3) return 4;
  else if(tempData >= 1<<2) return 3;
  else if(tempData >= 1<<1) return 2;
  else if(tempData >= 1) return 1;
  else return 0;
}

void rtc_loop()
{
  unsigned long now = millis();
  if((now - lastRtcUpdate) < 10000 && isRtcTimeHealthy) // every 10s. Update.
    return;
  lastRtcUpdate = now;
  if(!rtc.isrunning())
  {
    isRtcTimeHealthy = false;
    return;
  }
  else
  {
    isRtcTimeHealthy = true;
    MachineTime = rtc.now();
    MachineDayOfWeek = MachineTime.dayOfTheWeek();//update DOW [SUNDAY = 0 ~ SATURDAY = 6]
  }
}

void Scheduler_loop()
{
  if(isRtcTimeHealthy)
    WeMosState = getSchedulerStatus(MachineTime,MachineDayOfWeek);
  else
    WeMosState = LOGGING;//if time is not healthy,,, keep logging.
  if(lastWeMosState == IDLE && (lastWeMosState != WeMosState))//From IDLE to working!!
  {
    timeStamp_count = 0;
    lastFootPadUpdate = 0;
    lastWiFiUpdate = 0;
    lastSerialUpdate = 0;
    bufferInit(); // init buffer, to start warmup.
    isWiFiHealthy = false;
    isClientConnected = false;
  }
  else if(WeMosState == IDLE && (lastWeMosState != WeMosState))//From working to IDLE!
  {
    timeStamp_count = 0;//reset count.
    isClientConnected = false;
  }
  lastWeMosState = WeMosState;//log the 'last' state.
}

void WiFi_loop()
{
  unsigned long now = millis();
  if((now-lastWiFiUpdate)<10000 && isWiFiHealthy)//Check every '10[s]'
    return;
  lastWiFiUpdate = now;
  if(WiFi.status() == WL_CONNECTED)
  {
    isWiFiHealthy = true;
  }
  else
  {
    isWiFiHealthy = false;
    isClientConnected = false;//automatically.
    return;
  }

}

void client_loop()
{
  if(timeStamp_count == 200 && isWiFiHealthy)
  {
    if(clientConnect())
    {
      isClientConnected = true;
    }
    else
      isClientConnected = false;
  }
  if(!isWiFiHealthy)
    isClientConnected = false;//Hierarchially, client should decide it's state by WiFi Status.
}

void ThingSpeak_loop()
{
  if(WeMosState != LOGGING)
    return;//if not logging, return.
  if(timeStamp_count == 200)
  {
    avg_highest_MSB_centi = Sum_highest_MSB / (200 / 100); 
    uint16_t singleDigit = avg_highest_MSB_centi; // can go over 500.
    writeFieldData(6,2,singleDigit % 10 + '0');
    singleDigit /= 10;
    writeFieldData(6,1,singleDigit % 10 + '0');
    singleDigit /= 10;
    writeFieldData(6,0,singleDigit % 10 + '0');
    //writeFieldData(7,0,'0');
    //writeFieldData(8,0,'0');
    timeStamp_count = 0;
    Sum_highest_MSB = 0;//init.
    if(isClientConnected)
      bool temporary = sendRequest_to_ThingSpeak();
  }
}

uint8_t readVal_raw()
{
  uint8_t val = 0;
  val |= (analogRead(SENSOR_PIN_5_A)<400?1:0)<<4;
  val |= (digitalRead(SENSOR_PIN_4)==LOW?1:0)<<3;
  val |= (digitalRead(SENSOR_PIN_3)==LOW?1:0)<<2;
  val |= (digitalRead(SENSOR_PIN_2)==LOW?1:0)<<1;
  val |= (digitalRead(SENSOR_PIN_1)==LOW?1:0)<<0;
  return val;
}

void FootPad_loop()
{
  unsigned long now = millis();
  while((now - lastFootPadUpdate) < 100){now = millis();}
  lastFootPadUpdate = now;

  uint8_t val = readVal_raw();//Read Val.
  
  lastVal = val;
  Sum_highest_MSB += getHighestMSB(val&Val_sum_mask);
  
  int fieldNum = timeStamp_count / (_4SECONDS_TOTAL_BYTE_TO_SEND/DATA_SIZE_PER_TIMESTAMP) + 1; // counter/(howmanyTimeStamp_per_4second) .
  int fieldIndex = timeStamp_count % (_4SECONDS_TOTAL_BYTE_TO_SEND/DATA_SIZE_PER_TIMESTAMP); // counter % (howmanyTimeStamp_per_4second) . 
  
  writeFieldData(fieldNum,fieldIndex,val);
  
  timeStamp_count++;//Max 199.
}

void setup()
{
  pinMode(SENSOR_PIN_1, INPUT);
  pinMode(SENSOR_PIN_2, INPUT);
  pinMode(SENSOR_PIN_3, INPUT);
  pinMode(SENSOR_PIN_4, INPUT);
  //pinMode(SENSOR_PIN_5, INPUT);

  ESP.wdtDisable();//What can possibly go wrong :)

  Serial.begin(115200);
  memcpy(ssid,"YOUR_WIFI_NAME",12);
  memcpy(password,"YOUR_WIFI_PASSWORD",9);
  Serial.print("\n1.WIFI : ");
  Serial.print(ssid);
  Serial.print(',');
  Serial.println(password);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.println('.');
    ESP.wdtFeed();//What can possbily go wrong :)
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("2.Client : ");
  if (clientConnect())
    Serial.println("OK");
  else
    Serial.println("FAIL");

  Serial.print("3.RTC run : ");
  rtc.begin();
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Wire.begin(D2,D1);//SDA, SCL
  if(rtc.isrunning())
    Serial.println("OK");
  else
    Serial.print("FAIL");

 bufferInit(); // ThingSpeak.h
 Serial.println("BufferInitialized");//debug.

#ifdef DEBUG
  //ShowMacros();
#endif

}

void Serial_loop()
{
  unsigned long now = millis();
  if((timeStamp_count +1) % 5 == 0)//Every 0.5s.
  {
    Serial.print("State:");
    Serial.println(lastVal,BIN);
  }
  else if(forced_Val_print && WeMosState != LOGGING && ((now - lastForcedValprint) > 500) )//forced print works only when WeMos is not logging.
  {
    lastForcedValprint = now;//time update.
    uint8_t val = readVal_raw();
    Serial.print("Val : ");
    Serial.println(val,BIN);
  }
  if((now - lastSerialUpdate) < 10000)//every 10s.
    return;
  lastSerialUpdate = now;

  Serial.print("\nTS_cnt:");
  Serial.println(timeStamp_count);

  Serial.print("hp: (WF,RTC) : ");
  Serial.print(isWiFiHealthy?'Y':'N');
  Serial.print(',');
  Serial.println(isRtcTimeHealthy?'Y':'N');

  Serial.print("client: ");
  Serial.println(isClientConnected?'Y':'N');

  Serial.print("WF~:");
  Serial.println(now - lastWiFiUpdate);
  Serial.print("FP~:");
  Serial.println(now - lastFootPadUpdate);
  Serial.print("RTC~");
  Serial.println(now - lastRtcUpdate);

  Serial.print("WeMo:");
  Serial.println(WeMosState);
  
  Serial.print("DoW:");
  Serial.println(MachineDayOfWeek);
  
  char buf[15];
  memset(buf,0,15);
  sprintf(buf,"%02d%02d%02d_%02d%02d%02d",MachineTime.year()%100,MachineTime.month(),
  MachineTime.day(),MachineTime.hour(),MachineTime.minute(),MachineTime.second());
  Serial.println(buf);
}

bool wait_Serial_with_delay(uint8_t howmany,uint8_t millis_delay)
{
  unsigned long Start = millis();
  while(Serial.available() < howmany)
  {
    if(millis()-Start > millis_delay)
      return false;
  }
  return true;
}

void CLI_loop()
{
  uint8_t buffer[20];
  if(Serial.available() < 2)//Header + command 'at least' required.
    return;
  char c = Serial.read();
  if(c == '$')
  {//Header Received.
  char cmd = Serial.read();
  if(cmd =='r'){//RTC time reset. SMALL
  
  if(!wait_Serial_with_delay(6,10))
    return;
  
  for(uint8_t i=0;i<6;i++){buffer[i] = (Serial.read()-'0');}//get ex, 122345 (12PM 23min 45s.)
  uint8_t hour = (buffer[0])*10 + buffer[1];
  uint8_t minute = (buffer[2])*10 + buffer[3];
  uint8_t seconds = (buffer[4])*10 + buffer[5];
  if(hour >= 24 || minute >= 60 || seconds >= 60)
    return;
  rtc.adjust(DateTime(MachineTime.year(),MachineTime.month(),MachineTime.day(),hour,minute,seconds));//adjust 'em'!
  }
  else if(cmd == 'R'){////RTC time reset. BIG

  if(!wait_Serial_with_delay(12,10))//12 digits.
    return;
  
  for(uint8_t i=0;i<12;i++){buffer[i] = (Serial.read() - '0');}
  uint8_t year = (buffer[0])*10 + buffer[1];
  uint8_t month = (buffer[2])*10 + buffer[3];
  uint8_t day = (buffer[4])*10 + buffer[5];
  uint8_t hour = (buffer[6])*10 + buffer[7];
  uint8_t minute = (buffer[8])*10 + buffer[9];
  uint8_t seconds = (buffer[10])*10 + buffer[11];
  if(month > 12 || day > 31 || hour >= 24 || minute >= 60 || seconds >= 60)
    return;
  rtc.adjust(DateTime(year,month,day,hour,minute,seconds));//adjust 'em'!
  }
  else if(cmd == 'N'){//Network setting.
    char temp;
    uint8_t len = 0;
    delay(10);//don't know length of packet. Just delay 10ms... give some time for serial buffer to fill up.
    if(Serial.available() < (1+1+1+1))//ssid,/,paswd,/ AT LEAST.
      return;

    while(len < 19)//len should be atmost 18.
    {
      temp = Serial.read();
      if(temp == '/')
        break;
      buffer[len++] = temp;
    }
    if(temp != '/')//length overload.
    {
      Serial.println("ssid name over 19 or Invalid!");
      return;
    }
    memset(ssid,0,20);
    memcpy(ssid,buffer,len);

    len = 0;
    while(len < 19)//len should be atmost 18.
    {
      temp = Serial.read();
      if(temp == '/')
        break;
      buffer[len++] = temp;
    }
    if(temp != '/')//length overload.
    {
      Serial.println("paswd over 19 or Invalid!");
      return;
    }
    memset(password,0,20);
    memcpy(password,buffer,len);

    Serial.print('>');
    Serial.print(ssid);
    Serial.print(',');
    Serial.println(password);
    WiFi.begin(ssid,password);//init connection....
  }
  else if(cmd == 'M'){//Mask

    if(!wait_Serial_with_delay(1,5))//for 'detail'
      return;

    char detail = Serial.read();
    if(detail == '+')
    {
      if(!wait_Serial_with_delay(1,5))//for 'Num'
        return;
      char Num = Serial.read();
      if(Num > '5' || Num < '1')//Only '1' ~ '5' Allowed.
        return;
      Val_sum_mask |= (1<<(Num-'1'));
    }
    else if(detail == '-')
    {
      if(!wait_Serial_with_delay(1,5))//for 'Num'
        return;
      char Num = Serial.read();
      if(Num > '5' || Num < '1')//Only '1' ~ '5' Allowed.
        return;
      Val_sum_mask &= ~(1<<(Num-'1'));
    }
    else
    {
      if(detail != '?')
        return;
    }
    Serial.print("Val_sum_mask : ");
    Serial.println(Val_sum_mask,BIN);
  }
  else if(cmd == 'V'){//forced Val update?
    forced_Val_print = true;
    lastForcedValprint = 0;
  }
  else if(cmd == 'v'){
    forced_Val_print = false;
  }
  else
  {
    return;//Command not accpeted!
  }
  
  Serial.println("OK");//just an CLI thing... :)
  }//header recv.
}

void loop()
{
  rtc_loop();
  Scheduler_loop();
  //WeMosState = IDLE;//FIXME: Remove me.
  if(WeMosState != IDLE)
  {
  FootPad_loop();
  WiFi_loop();
  client_loop();
  ThingSpeak_loop();
  if(timeStamp_count == 200)
    timeStamp_count = 0;//reset timestamp count
  }
  Serial_loop();
  CLI_loop();
  ESP.wdtFeed();//What can possbily go wrong :)
}
