/*

*/

#define DATA_SIZE_PER_TIMESTAMP (1) // Unfortunate...

#define _4SECONDS_TOTAL_BYTE_TO_SEND (DATA_SIZE_PER_TIMESTAMP * 40) // 40 'timeStamp'.

#define NUM_OF_FIELDS (6)

typedef struct{
  char field1[_4SECONDS_TOTAL_BYTE_TO_SEND];
  char field2[_4SECONDS_TOTAL_BYTE_TO_SEND];
  char field3[_4SECONDS_TOTAL_BYTE_TO_SEND];
  char field4[_4SECONDS_TOTAL_BYTE_TO_SEND];
  char field5[_4SECONDS_TOTAL_BYTE_TO_SEND];
  char field6[3];//simple 'Sample' ex) '0~5'
  //char field7;//simple 'Sample' ex) '0~5'
  //char field8;//simple 'Sample' ex) '0~5'
} Data_t;

#define Data_t_SIZE sizeof(Data_t)

//#define Field6_OFFSET _4SECONDS_TOTAL_BYTE_TO_SEND * 5 // where field6 starts.

const char MESSAGE_HEADER[] = "GET /update?key=YourSpecificKEYs";//16-letter 'KEY'. Should be modified for custom use!
#define MESSAGE_HEADER_LEN (sizeof(MESSAGE_HEADER) - 1) //don't include 'NULL'

const char AndField_common[] = "&field1=";
#define AndField_common_LEN (sizeof(AndField_common) - 1) //don't include 'NULL'
const int AndField_IndexPos = AndField_common_LEN - 2;//2nd from the back.

const char HTTP_TRAILING_STUFF[] = " HTTP/1.1\r\nHost: api.thingspeak.com\r\nConnection: close\r\n\r\n";
#define HTTP_TRAILING_START_POS (MESSAGE_HEADER_LEN + AndField_common_LEN * NUM_OF_FIELDS + Data_t_SIZE)
#define HTTP_TRAILING_STUFF_LEN (sizeof(HTTP_TRAILING_STUFF) - 1) //don't include 'NULL'

#define TOTAL_MESSAGE_LENGTH (MESSAGE_HEADER_LEN + AndField_common_LEN * NUM_OF_FIELDS + Data_t_SIZE + HTTP_TRAILING_STUFF_LEN + 1) // struct can have 'padding'...

char MsgBuffer[TOTAL_MESSAGE_LENGTH]; // Declaration of Message Buffer!

Data_t data;// Declaration of DATA

//bool msgBuffer_filled = false;

extern WiFiClient client;//extern!!!

void bufferInit(void)
{
	memset(&data,0,sizeof(Data_t));
	memset(MsgBuffer,0,sizeof(MsgBuffer));
	memcpy(MsgBuffer,MESSAGE_HEADER,MESSAGE_HEADER_LEN);//put it there!
	memcpy(&MsgBuffer[HTTP_TRAILING_START_POS] , HTTP_TRAILING_STUFF , HTTP_TRAILING_STUFF_LEN);//following lines...
}

//#define WRITE_FIELD_DATA_RAW_SHOW
void writeFieldData(int fieldNum,int index,unsigned int package)//total index upto 39.
{
	#ifdef WRITE_FIELD_DATA_RAW_SHOW
	Serial.print("wFD");
	Serial.print(fieldNum);
	Serial.print(',');
	Serial.print(index);
	Serial.print(',');
	Serial.print(package);
	Serial.println(" CALLED");
  #endif
	char* target = NULL;
	if(fieldNum > NUM_OF_FIELDS || fieldNum < 0) // fallthrough all the bad cases.
		return;
	switch(fieldNum){
		case 1:
		target = &data.field1[index*DATA_SIZE_PER_TIMESTAMP]; break;
		case 2:
		target = &data.field2[index*DATA_SIZE_PER_TIMESTAMP]; break;
		case 3:
		target = &data.field3[index*DATA_SIZE_PER_TIMESTAMP]; break;
		case 4:
		target = &data.field4[index*DATA_SIZE_PER_TIMESTAMP]; break;
		case 5:
		target = &data.field5[index*DATA_SIZE_PER_TIMESTAMP]; break;
		case 6:
		data.field6[index] = package; break; // Special field to write!
		/*
		case 7:
		data.field7 = package; break;
		case 8:
		data.field8 = package; break;
		*/
	}
	if(target != NULL && fieldNum < 6) // only 1~5 will do Compression.
		CompressData(target,package);
}

//#define SERIALIZE_PARTIAL_ENALBE
void serializePartial(char* buf,int len){
  #ifdef SERIALIZE_PARTIAL_ENALBE
  Serial.print('>');
  for(int i=0;i<len;i++){
    Serial.print(buf[i]);
  }
  Serial.println('<');
  #endif
  return;
}

void fill_MsgBuffer(void)
{
	char* ptr = &MsgBuffer[MESSAGE_HEADER_LEN];
  serializePartial(MsgBuffer,MESSAGE_HEADER_LEN);
	//field 1
	memcpy(ptr,AndField_common,AndField_common_LEN);
	ptr[AndField_IndexPos] = '1';
	ptr += AndField_common_LEN;
  serializePartial(MsgBuffer,(ptr-MsgBuffer));

	memcpy(ptr,&data.field1,_4SECONDS_TOTAL_BYTE_TO_SEND);
	ptr += _4SECONDS_TOTAL_BYTE_TO_SEND;
  serializePartial(MsgBuffer,(ptr-MsgBuffer));
  
	//field 2
	memcpy(ptr,AndField_common,AndField_common_LEN);
	ptr[AndField_IndexPos] = '2';
	ptr += AndField_common_LEN;
  serializePartial(MsgBuffer,(ptr-MsgBuffer));
  
	memcpy(ptr,&data.field2,_4SECONDS_TOTAL_BYTE_TO_SEND);
	ptr += _4SECONDS_TOTAL_BYTE_TO_SEND;
  serializePartial(MsgBuffer,(ptr-MsgBuffer));
  
	//field 3
	memcpy(ptr,AndField_common,AndField_common_LEN);
	ptr[AndField_IndexPos] = '3';
	ptr += AndField_common_LEN;
  serializePartial(MsgBuffer,(ptr-MsgBuffer));
  
	memcpy(ptr,&data.field3,_4SECONDS_TOTAL_BYTE_TO_SEND);
	ptr += _4SECONDS_TOTAL_BYTE_TO_SEND;
  serializePartial(MsgBuffer,(ptr-MsgBuffer));
  
	//field 4
	memcpy(ptr,AndField_common,AndField_common_LEN);
	ptr[AndField_IndexPos] = '4';
	ptr += AndField_common_LEN;
  serializePartial(MsgBuffer,(ptr-MsgBuffer));
  
	memcpy(ptr,&data.field4,_4SECONDS_TOTAL_BYTE_TO_SEND);
	ptr += _4SECONDS_TOTAL_BYTE_TO_SEND;
  serializePartial(MsgBuffer,(ptr-MsgBuffer));
  
	//field 5
	memcpy(ptr,AndField_common,AndField_common_LEN);
	ptr[AndField_IndexPos] = '5';
	ptr += AndField_common_LEN;
  serializePartial(MsgBuffer,(ptr-MsgBuffer));
  
	memcpy(ptr,&data.field5,_4SECONDS_TOTAL_BYTE_TO_SEND);
	ptr += _4SECONDS_TOTAL_BYTE_TO_SEND;
  serializePartial(MsgBuffer,(ptr-MsgBuffer));
  
	//field 6
  memcpy(ptr,AndField_common,AndField_common_LEN);
  ptr[AndField_IndexPos] = '6';
  ptr += AndField_common_LEN;
	memcpy(ptr,data.field6,3);
	ptr += 3;//field 6.
/*
  //field 7
  memcpy(ptr,AndField_common,AndField_common_LEN);
  ptr[AndField_IndexPos] = '7';
  ptr += AndField_common_LEN;
	*ptr++ = data.field7;

  //field 8
  memcpy(ptr,AndField_common,AndField_common_LEN);
  ptr[AndField_IndexPos] = '8';
  ptr += AndField_common_LEN;
	*ptr++ = data.field8;
 */
 
  serializePartial(MsgBuffer,(ptr-MsgBuffer));
  
  Serial.println("End of fill_MsgBuffer()");
}

bool clientConnect(void){ // takes a long~ time...
  return (client.connect("api.thingspeak.com", 80) == 1);// 1 : SUCCESS!
}

void handle_Client_Timeout(void){
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
}

extern bool isClientConnected;

bool sendRequest_to_ThingSpeak(void)
{
  if(!isClientConnected)
    return false;

  fill_MsgBuffer();
  client.print(MsgBuffer); 
  
  #ifdef DEBUG
  //for(int i=0;i<TOTAL_MESSAGE_LENGTH;i++){Serial.print(MsgBuffer[i]);}
  //Serial.println("\nTHISISENOGUH??????");
  #endif
  
  handle_Client_Timeout();
  
  while(client.available()){Serial.print(client.readStringUntil('\r'));}
  
  return true;
}

#ifdef DEBUG
	void ShowMacros(void){
		Serial.print("MESSAGE_HEADER>");
		Serial.println(MESSAGE_HEADER);
		Serial.print("MESSAGE_HEADER_LEN");
		Serial.println(MESSAGE_HEADER_LEN);
		Serial.print("AndField_common>");
		Serial.println(AndField_common);
		Serial.print("AndField_common_LEN>");
		Serial.println(AndField_common_LEN);
		Serial.print("AndField_common_LEN * NUM_OF_FIELDS>");
		Serial.println(AndField_common_LEN * NUM_OF_FIELDS);
		Serial.print("HTTP_TRAILING_STUFF>");
		Serial.println(HTTP_TRAILING_STUFF);
		Serial.print("HTTP_TRAILING_STUFF_LEN>");
		Serial.println(HTTP_TRAILING_STUFF_LEN);
		Serial.print("HTTP_TRAILING_START_POS>");
		Serial.println(HTTP_TRAILING_START_POS);
		Serial.print("TOTAL_MESSAGE_LENGTH>");
		Serial.println(TOTAL_MESSAGE_LENGTH);
		Serial.print("Data_t_SIZE>");
		Serial.println(Data_t_SIZE);
    Serial.print("sizeof(Data_t)>");
    Serial.println(sizeof(Data_t));
	}
#endif
