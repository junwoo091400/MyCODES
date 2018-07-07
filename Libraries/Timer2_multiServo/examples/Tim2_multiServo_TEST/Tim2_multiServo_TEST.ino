#include "Tim2_multiServo.h"

Tim2_Servo myServo[10];
int servoPins[] = {2,3,4,5,6,7,8,9,10,11};

void setup()
{
	Serial.begin(115200);
	for(int i = 0; i< 10; i++)
		myServo[i].begin(servoPins[i], 1500);
}

unsigned long last_set = micros();

int getSerialInput(){
	int value = 0;
	while(Serial.available()) {
		char readChar = Serial.read();
		if(readChar >= '0' && readChar <= '9') {
			value = value*10 + (readChar - '0');
		}
	}
	return value;
}

void loop()
{
	if(micros() - last_set > 10000) {//100 ms. Wait for serial buffer to fill up.
		//Serial.println(debug_val);
		int idx = getSerialInput();
		int input = 0;
		if(idx >=0 || idx <= 10) {
			Serial.println("Valid Servo Idx. Value?");
			while(input == 0) {
				input = getSerialInput();//pulse width.
				if(input >= 1000 && input <= 2000) {
					Serial.println("Valid Pulse Width!");
				}
			}
			//input = constrain(input, 0, 255);
			Serial.print("Got Input ");
			Serial.println(input);
			//analogWrite(5, input);
			//OCR0B = input;
			//val = input;
			myServo[idx].writeUS(input);
			//Servo_tick_set = map(input,0,255,2000,4000);//Up to 2000 us.
			//Serial.println(Servo_tick_set);
		}
		last_set = micros();
	}//after 10ms.
}

