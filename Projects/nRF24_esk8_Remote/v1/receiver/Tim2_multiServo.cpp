#include <avr/interrupt.h>
#include <Arduino.h>

#include "Tim2_multiServo.h"

static Tim2_Servo_t servoConfig[MAX_SERVO_COUNT];
static uint8_t SERVO_COUNT = 0;//only one.
//
static volatile uint8_t servoIdx = 0;
static volatile uint16_t tickLeft = 0;
//
static bool initialized = false;
//

#define getServoTickDuration(servoIdx) (servoConfig[servoIdx].isUsed) ? (servoConfig[servoIdx].tick_duration) : (DEFAULT_SERVO_TICK)
#define usToTick(us) us*2

static inline void servo_ON(uint8_t idx) {
	if(servoConfig[idx].isUsed) {
		*servoConfig[idx].port |= servoConfig[idx].bitMask;
	}
}
static inline void servo_OFF(uint8_t idx) {
	if(servoConfig[idx].isUsed) {
		*servoConfig[idx].port &= ~servoConfig[idx].bitMask;
	}
}

static bool initSTARTUP()//call FIRST.
{
	if(SERVO_COUNT == 0)
		return false;
	
	TIMSK2 &= ~(bit(TOIE2) | bit(OCIE2B) | bit(OCIE2A));//DISABLE any interrupt from happening.

	//startup procedure.
	servoIdx = 0;
	tickLeft = servoConfig[servoIdx].tick_duration;
	OCR2B = (uint8_t)(tickLeft & 0xFF);
	//

	TCCR2A = bit (WGM20) | bit (WGM21); // fast PWM.
	TCCR2B = bit (CS21);//prescale 8. => 2MHz !!!
	TCNT2 = 0;
	TIMSK2 = bit(TOIE2);//Overflow Interrupt...
}


static inline void tim2_OVF_isr();
static inline void tim2_COMPB_isr();

ISR(TIMER2_COMPB_vect)
{
	tim2_COMPB_isr();
}

static inline void tim2_COMPB_isr()
{
	//PORTD |= bit(2);
	uint8_t servoIdx_copy = servoIdx;
	if(servoIdx_copy == 0) {
		servo_OFF(servoIdx_copy);
	}
	TIMSK2 &= ~bit(OCIE2B);//disable B compare interrupt(Used for terminating pulses).
	servoIdx_copy = ((servoIdx_copy + 1) >= MAX_SERVO_COUNT) ? (0) : (servoIdx_copy + 1);//update idx.
	tickLeft = getServoTickDuration(servoIdx_copy);
	OCR2B = (uint8_t)(tickLeft & 0xFF);//PRE-LOAD tick to go... Because will update at the bottom after one more Overflow..
	servoIdx = servoIdx_copy;
	//PORTD &= ~bit(2);
}

ISR(TIMER2_OVF_vect)//2MHz timer.
{
	tim2_OVF_isr();
}

static inline void tim2_OVF_isr()
{
	static volatile bool new_Servo = false;
	//PORTD |= bit(3);
	if(new_Servo) {//new servo Start.
		uint8_t Idx_copy = servoIdx;
		servo_ON(Idx_copy);
		new_Servo = false;//reset.
	}
	if(tickLeft & 0xFF00){//More than 255.
		tickLeft -= 256;
	}
	else{//ready for the final pulse.
		TIFR2 |= bit(OCF2B);//CLEAR the interrupt flag. Otherwise, the previous match flag will triger the interrupt immediately...
		TIMSK2 |= bit(OCIE2B);//enable B Match Interrupt.
		//debug_val = tickLeft;
		tickLeft = 0;//finalize.
		new_Servo = true;
	}
	//PORTD &= ~bit(3);
}

uint8_t getFreedSlot()
{
	for(uint8_t i=0;i<MAX_SERVO_COUNT;i++)
		if(servoConfig[i].isUsed == false)
			return i;
	return -1;
}

bool Tim2_Servo::begin(uint8_t *port, uint8_t bitMask, int durationUS)
{
	if(SERVO_COUNT >= MAX_SERVO_COUNT) {
		return false;
	}
	if(durationUS <= 0) {
		return false;
	}

	//VALID port, bitMask...
	SERVO_COUNT += 1;
	uint8_t newIdx = getFreedSlot();

	this -> idx = newIdx;
	servoConfig[newIdx].isUsed = true;
	servoConfig[newIdx].tick_duration = usToTick(durationUS);
	servoConfig[newIdx].port = port;
	servoConfig[newIdx].bitMask = bitMask;

	if(!initialized && initSTARTUP()) {//configure timer, fire up the servos!
		initialized = true;
	}

	return true;
}

bool Tim2_Servo::begin(int pin, int durationUS)//More USER friendly.
{
	uint8_t* port = portOutputRegister(digitalPinToPort(pin));
	uint8_t bitMask = digitalPinToBitMask(pin);
	pinMode(pin, OUTPUT);
	return this -> begin(port, bitMask, durationUS);
}

void Tim2_Servo::writeUS(int durationUS)
{
	servoConfig[this -> idx].tick_duration = usToTick(durationUS);
}