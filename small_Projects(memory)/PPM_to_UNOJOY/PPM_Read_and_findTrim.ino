
#define RC_CHANS 8
volatile uint16_t rcValue[RC_CHANS] = {1502, 1502, 1502, 1502, 1502, 1502, 1502, 1502};

enum {
  ROLL,
  PITCH,
  THROTTLE,
  YAW,
  AUX1,
  AUX2,
  AUX3,
  AUX4
};

void setup() {
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(2), rxInt, RISING);
}

void loop() {
  unsigned long SUMS[4] = {};
  memset(SUMS,0,sizeof(SUMS));
  for(int i=0;i<100;i++){
    SUMS[ROLL] += rcValue[ROLL];
    SUMS[PITCH] += rcValue[PITCH];
    SUMS[THROTTLE] += rcValue[THROTTLE];
    SUMS[YAW] += rcValue[YAW];
    delay(5);
  }
  Serial.print(SUMS[ROLL]/100);     Serial.print("     ");
  Serial.print(SUMS[PITCH]/100);    Serial.print("     ");
  Serial.print(SUMS[THROTTLE]/100); Serial.print("     ");
  Serial.print(SUMS[YAW]/100);      Serial.print("     ");
  Serial.print(rcValue[AUX1]);     Serial.print("     ");
  Serial.print(rcValue[AUX2]);     Serial.print("     ");
  Serial.print(rcValue[AUX3]);     Serial.print("     ");
  Serial.print(rcValue[AUX4]);     Serial.println("     ");
}

////////////////////////////////////////////////////////////////////////////////////

// Convert a value in the range 1000-2000 to 0-255
byte stickValue(int rcVal) {
  return map( constrain(rcVal - 1000, 0, 1000), 0, 1000, 0, 255);
}

// Some adjustments I used because my cheapass TX has no trims.
// If your TX has trims you will not need this and you can simply
// return the 'diff' value unchanged.
uint16_t adjust(uint16_t diff, uint8_t chan) {
  /*switch (chan) {
    case THROTTLE: return diff + 50;
    case YAW:      return diff + 60;
    case PITCH:    return diff + 60;
    case ROLL:     return diff + 90;
    case AUX1:     return diff + 10;
  }*/
  return diff;
}

// PPM interrupt routine courtesy of MultiWii source code
void rxInt(void) {
  uint16_t now,diff;
  static uint16_t last = 0;
  static uint8_t chan = 0;
  now = micros();
  sei();
  diff = now - last;
  last = now;
  if(diff>3000 || chan >= RC_CHANS) chan = 0;
  else {
    if(900<diff && diff<2200) {
      rcValue[chan] = adjust(diff,chan);
    }
    ++chan;
  }
}