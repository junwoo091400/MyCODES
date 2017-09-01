/*
* PPM generator originally written by David Hasko
* on https://code.google.com/p/generate-ppm-signal/
*/

//////////////////////CONFIGURATION///////////////////////////////
#define CHANNEL_NUMBER 12  //set the number of chanels
#define CHANNEL_DEFAULT_VALUE 1000  //set the default servo value
#define FRAME_LENGTH 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PULSE_LENGTH 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin  6  //set PPM signal output pin on the arduino

#define sigPinON PORTD |= B01000000;
#define sigPinOFF PORTD &= B10111111;
//////////////////////////////////////////////////////////////////

/*this array holds the servo values for the ppm signal
change theese values in your code (usually servo values move between 1000 and 2000)*/
int PPM_Data[CHANNEL_NUMBER];

void PPM_init() {

	//initiallize default ppm values
	for (int i = 0; i<CHANNEL_NUMBER; i++) PPM_Data[i] = CHANNEL_DEFAULT_VALUE;

	pinMode(sigPin, OUTPUT);
	sigPinOFF //OFF!
	//digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)

	cli();
	TCCR1A = 0; // set entire TCCR1 register to 0
	TCCR1B = 0;

	OCR1A = 100;  // compare match register, change this
	TCCR1B |= (1 << WGM12);  // turn on CTC mode
	TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
	TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
	sei();
}

ISR(TIMER1_COMPA_vect) {  //leave this alone
	static boolean state = true;

	TCNT1 = 0;

	if (state) {  //start pulse
		sigPinON //ON!
		OCR1A = PULSE_LENGTH * 2;
		state = false;
	}
	else {  //end pulse and calculate when to start the next pulse
		static byte cur_chan_numb;
		static unsigned int calc_rest;

		sigPinOFF //OFF!
		state = true;

		if (cur_chan_numb >= CHANNEL_NUMBER) {
			cur_chan_numb = 0;
			calc_rest = calc_rest + PULSE_LENGTH;// 
			OCR1A = (FRAME_LENGTH - calc_rest) * 2;
			calc_rest = 0;
		}
		else {
			OCR1A = (PPM_Data[cur_chan_numb] - PULSE_LENGTH) * 2;
			calc_rest = calc_rest + PPM_Data[cur_chan_numb];
			cur_chan_numb++;
		}
	}
}
