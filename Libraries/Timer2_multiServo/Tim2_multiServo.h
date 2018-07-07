/*
 * Tim2_multiServo.h
 */

#define MAX_SERVO_COUNT 10
#define DEFAULT_SERVO_TICK 3000 // 1500 us.

typedef boolean bool;

//#define MIN_SERVO_TICK 256 // if less, 

typedef struct
{
	bool isUsed;
	uint8_t *port;
	uint8_t bitMask;
	int tick_duration;
} Tim2_Servo_t;

class Tim2_Servo
{
private:
	uint8_t idx;
	bool begin(uint8_t *port, uint8_t bitMask, int durationUS);
public:
	bool begin(int pin, int durationUS);
	void writeUS(int durationUS);
};
//volatile int Servo_tick_set = 1000 * 2;//1000 microseconds pulse.
