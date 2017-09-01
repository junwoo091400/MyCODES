#ifndef __JunwooHWANG_PPM_H__
#define __JunwooHWANG_PPM_H__

#define PPM_PIN 2 //only pin 2 or 3
#define PPM_CHANNEL_COUNT 8
#define PPM_ENDING_LENGTH 5000

volatile int PPM_Input[PPM_CHANNEL_COUNT] = {};

//1
void PPM_Reader() {
	static byte curChannel;
	static unsigned long Last_Fall_Time;
	unsigned long Now_Time = micros();

	if (Now_Time - Last_Fall_Time >= PPM_ENDING_LENGTH || curChannel >= PPM_CHANNEL_COUNT) curChannel = 0;
	else PPM_Input[curChannel++] = (Now_Time - Last_Fall_Time);

	Last_Fall_Time = Now_Time;
}

//2
void PPM_init() { attachInterrupt(digitalPinToInterrupt(PPM_PIN), PPM_Reader, FALLING); }

#endif