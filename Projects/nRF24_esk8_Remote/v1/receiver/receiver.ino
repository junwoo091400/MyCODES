#include <SPI.h>
#include <nRF24L01.h>
#include "RF24.h"

// #define DEBUG//for debugging.

#include "Tim2_multiServo.h"

Tim2_Servo myServo;

//Good Servo signal discussion(20 times interrupt(1ms*20). BitBang!) : https://www.avrfreaks.net/forum/servo-control-8-bit-timer

#include <SoftwareSerial.h>
SoftwareSerial junwooSerial = SoftwareSerial(2, 3); // Hardcoded software-serial.

#include "VescUart.h"

#include "crc.h"//

//#include <Servo.h>
//Servo PPM_servo;

struct vescValues {
  float ampHours;
  float inpVoltage;
  long rpm;
  long tachometerAbs;
};

struct radioCommand {
  int motorSpeed;//2 byte.
  int extraChannel[3];//6 byte.
  uint16_t crc;//checksum.
};

radioCommand command_recv;

//
RF24 radio(9, 10);
const uint64_t pipe = 0xE8E8F0F0E1LL;

bool recievedData = false;
uint32_t lastTimeReceived = 0;
//

int timeoutMax = 500;
int speedPin = 5;//D5!!
//

struct bldcMeasure measuredValues;

struct vescValues data;
unsigned long lastDataCheck;

bool radioComamnd_isValid(radioCommand *rc)
{
  uint16_t expected_crc = crc16((unsigned char*)rc, sizeof(radioCommand) - 2);//exclude last crc.
  if(expected_crc == rc -> crc) {
    return true;
  }
  else {
    return false;
  }
}

void setup() {
  SERIALIO.begin(38400);//SS 'junwooSerial'

  #ifdef DEBUG
  Serial.begin(115200);//DEBUG
  #endif
  
  radio.begin();
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1, pipe);
  radio.startListening();

  myServo.begin(5, 1500);//neutral pulse.

  //PPM_servo.attach(5);//Digital Pin 5.
  //pinMode(speedPin, OUTPUT);
  //analogWrite(speedPin, motorSpeed);
}

void loop() {

  getVescData();

  // If transmission is available
  if (radio.available())
  {
    // The next time a transmission is received on pipe, the data in gotByte will be sent back in the acknowledgement (this could later be changed to data from VESC!)
    radio.writeAckPayload(pipe, &data, sizeof(data));

    // Read the actual message
    radio.read(&command_recv, sizeof(radioCommand));
    //radio.read(&motorSpeed, sizeof(motorSpeed));

    if(radioComamnd_isValid(&command_recv)) {
      #ifdef DEBUG
      Serial.print("Got motorSpeed:");
      Serial.println(command_recv.motorSpeed);
      Serial.print("crc ");
      Serial.println(command_recv.crc,HEX);
      Serial.print("Current inpVolt ");
      Serial.println(data.inpVoltage);
      #endif
      recievedData = true;
    }
    else {
      recievedData = false;
    }
    
  }

  if (recievedData == true)
  {
    // A speed is received from the transmitter (remote).

    lastTimeReceived = millis();
    recievedData = false;

    // Write the PWM signal to the ESC (0-255).
    myServo.writeUS(map(command_recv.motorSpeed,0,255,1000,2000));
    //analogWrite(speedPin, motorSpeed);
  }
  else if ((millis() - lastTimeReceived) > timeoutMax)
  {
    // No speed is received within the timeout limit.
    command_recv.motorSpeed = 127;
    myServo.writeUS(1500);
    //analogWrite(speedPin, motorSpeed);
  }
}

void getVescData() {

  if (millis() - lastDataCheck >= 250) {

    lastDataCheck = millis();

    // Only transmit what we need
    if (VescUartGetValue(measuredValues)) {
      data.ampHours = measuredValues.ampHours;
      data.inpVoltage = measuredValues.inpVoltage;
      data.rpm = measuredValues.rpm;
      data.tachometerAbs = measuredValues.tachometerAbs;
      SerialPrint(measuredValues);//DEBUGGGGG
    } else {
      data.ampHours = 0.0;
      data.inpVoltage = 0.0;
      data.rpm = 0;
      data.tachometerAbs = 0;
    }
  }
}
