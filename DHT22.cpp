/*
  DHT22.cpp - DHT22 sensor library
  Developed by Ben Adams - 2011

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


Humidity and Temperature Sensor DHT22 info found at
http://www.sparkfun.com/products/10167

Version 0.7: 1 April 2017 by Junwoo HWANG
-Noticed Too much SYNC_TIMEOUT so wanted to see accuracy of delayMicroseconds() function.
-Turns out, by doing delayMicroseconds(2) => AVG. '1.22(us)' delay happended!
-Which means that if Target Time is J[us], then we should set the value:
-J/(11/9) = "(9/11)*J". EX) Delay 11us = delayMicroseconds(2)*9 times!
-Based on my calculations, correct while loop counts would be:
ACK_1 (66) -> 80us / ACK_2 (66) -> 80us / LOW (41) -> 50us / '1' (58) -> 70us / '0' (25) -> 26~28us...
!!!!!!!!!!!!!!!! "1.22 microseconds / (delayMicroseconds(2)+Loop)" is what I found out !!!!!!!!!!!!!!!!!!!!!!!!!!!!!

Version 0.6: 28 Mar 2017 by Junwoo HWANG
-Updated the method of getting data to seperate '2-part ACK pulse'.
-2 ACK Pulse with each length of 80(us) will be processed in 2 step.
-Reuducing the DHT22_DATA_BIT_COUNT to 41->40.
-if you include the second ack. as the bit-read,
-There is no Proceding 50(us) Start_LOW pulse. So it could give the Sync Timeout
-at the First for statement of Data READOUT. That is why I reduced to tal DATA num & added 2-part ACK processing.

Version 0.5: 15 Jan 2012 by Craig Ringer
- Updated to build against Arduino 1.0
- Made accessors inline in the header so they can be optimized away

Version 0.4: 24-Jan-2011 by Ben Adams
Added return code constants to keywords.txt
Returns DHT_ERROR_CHECKSUM on check sum mismatch 

Version 0.3: 17-Jan-2011 by Ben Adams
This version reads data
Needs check sum code added at the end of readData

Version 0.2: 16-Jan-2011 by Ben Adams
Changed coding style to match other Arduino libraries.
This version will not read data either!

Version 0.1: 10-Jan-2011 by Ben Adams nethoncho AT gmail.com
First Version is a skeleton. This version will not read data!
Code adapted from the following sources:
The Arduino OneWire lib
http://sheepdogguides.com/arduino/ar3ne1humDHT11.htm

*/

#include "DHT22.h"
#include <Arduino.h>
#include <pins_arduino.h>

extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
}

#define DIRECT_READ(base, mask)		(((*(base)) & (mask)) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask)	((*(base+1)) &= ~(mask))
#define DIRECT_MODE_OUTPUT(base, mask)	((*(base+1)) |= (mask))
#define DIRECT_WRITE_LOW(base, mask)	((*(base+2)) &= ~(mask))
#define DIRECT_WRITE_HIGH(base, mask)	((*(base+2)) |= (mask))

DHT22::DHT22(uint8_t pin)
{
    _bitmask = digitalPinToBitMask(pin);
    _baseReg = portInputRegister(digitalPinToPort(pin));
    _lastReadTime = millis();
    _lastHumidity = DHT22_ERROR_VALUE;
    _lastTemperature = DHT22_ERROR_VALUE;
}

//
// Read the 40 bit data stream from the DHT 22
// Store the results in private member data to be read by public member functions
//
DHT22_ERROR_t DHT22::readData()
{
  uint8_t bitmask = _bitmask;
  volatile uint8_t *reg asm("r30") = _baseReg;
  uint8_t retryCount;
  uint8_t bitTimes[DHT22_DATA_BIT_COUNT];
  int currentHumidity;
  int currentTemperature;
  uint8_t checkSum, csPart1, csPart2, csPart3, csPart4;
  unsigned long currentTime;
  int i;

  currentHumidity = 0;
  currentTemperature = 0;
  checkSum = 0;
  currentTime = millis();
  for(i = 0; i < DHT22_DATA_BIT_COUNT; i++)
  {
    bitTimes[i] = 0;
  }

  if(currentTime - _lastReadTime < 2000)
  {
    // Caller needs to wait 2 seconds between each call to readData
    return DHT_ERROR_TOOQUICK;
  }
  _lastReadTime = currentTime;

  // Pin needs to start HIGH, wait until it is HIGH with a timeout
  cli();
  DIRECT_MODE_INPUT(reg, bitmask);
  sei();
  retryCount = 0;
  do
  {
    if (retryCount > 125)//152 microseconds.
    {
      return DHT_BUS_HUNG;
    }
    retryCount++;
    delayMicroseconds(2);
  } while(!DIRECT_READ(reg, bitmask));
  
  // Send the activate pulse
  cli();
  DIRECT_MODE_OUTPUT(reg, bitmask);
  DIRECT_WRITE_LOW(reg, bitmask); // Output Low
  sei();
  delayMicroseconds(1100); // 1.1 ms Start Transmistting Pulse! ///////////////////////////"1.1ms LOW":Transmission Start Pulse by Master
  cli();
  DIRECT_MODE_INPUT(reg, bitmask);	// Switch back to input so pin can float
  sei();

  /////////////////////////////////////////////////////////////////////////////////////////
  // Find the Start of the First ACK Pulse <Wait for 20~40 microseconds.>
  retryCount = 0;
  do
  {
    if (retryCount > 41) //(Spec is 20 to 40 us, <<<41*(1.22) == 50 us>>>
    {
      return DHT_ERROR_NOT_PRESENT;
    }
    retryCount++;
    delayMicroseconds(2);
  } while(DIRECT_READ(reg, bitmask));//untill input goes LOW.(Start of the ACK pulse)//:Find the Starting point of the First_ACK by sensor
  
  //NOW we know the sensor has sent an ACK_low pulse
  // Find the end of the First ACK Pulse(80 microseconds)
  retryCount = 0;
  do
  {
    if (retryCount > 82) //(Spec is 80 us, <<82*1.22 == 100 us>>
    {
      return DHT_ERROR_ACK_TOO_LONG;
    }
    retryCount++;
    delayMicroseconds(2);
  } while(!DIRECT_READ(reg, bitmask));////////"Read ACK Pulse-1":80 microseconds Pull_LOW by the sensor

  //NOW We got successful 80 microsecond of First ACK Pulse (LOW)
  //We need to find the next 80 microsecond of Second ACK Pulse (HIGH)
  retryCount = 0;
  do
  {
	  if (retryCount > 82) //(Spec is 80 us, <<82*1.22 == 100 us>>
	  {
		  return DHT_ERROR_ACK_TOO_LONG;
	  }
	  retryCount++;
	  delayMicroseconds(2);
  } while (DIRECT_READ(reg, bitmask));////////"Read ACK Pulse-2":80 microseconds Pull_HIGH by the sensor.
  //It got pulled down Successfully. NOW We can start reading in some DATAs

  // Read the 40 bit data stream
  for(i = 0; i < DHT22_DATA_BIT_COUNT; i++)
  {
    // Find the start of the sync pulse
    retryCount = 0;
    do
    {
      if (retryCount > 57) //(Spec is 50 us, 57*1.22 == 70 us): "50 microseconds Pull_LOW at Start of bit Trans. by the sensor"
      {
		  KbitTimes[i] = 69;
        return DHT_ERROR_SYNC_TIMEOUT;
      }
      retryCount++;
      delayMicroseconds(2);
    } while(!DIRECT_READ(reg, bitmask));
    // Measure the width of the data pulse
    retryCount = 0;
    do
    {
      if (retryCount > 82) //(Spec is ( '26~28<REAL30>' OR '70<REAL74>' ) us, 82*1.22 == 100 us)
      {
        return DHT_ERROR_DATA_TIMEOUT;
      }
      retryCount++;
      delayMicroseconds(2);
    } while(DIRECT_READ(reg, bitmask));
    bitTimes[i] = retryCount;
  }
  // Now bitTimes have the number of retries (us *2)
  // that were needed to find the end of each data bit
  // Spec: 0 is 26 to 28 us
  // Spec: 1 is 70 us
  // 50 microseconds(middle point) = (1.22)*'41'[retryCounts]
  // bitTimes[x] <= 11 is a 0
  // bitTimes[x] >  11 is a 1
  for(i = 0; i < 16; i++)
  {
    if(bitTimes[i] > 41)//More than 50 micros approx. == Bit '1'
    {
      currentHumidity |= (1 << (15 - i));
    }
  }
  for(i = 0; i < 16; i++)
  {
    if(bitTimes[i + 16] > 41)//More than 50 micros approx. == Bit '1'
    {
      currentTemperature |= (1 << (15 - i));
    }
  }
  for(i = 0; i < 8; i++)
  {
    if(bitTimes[i + 32] > 41)//More than 50 micros approx. == Bit '1'
    {
      checkSum |= (1 << (7 - i));
    }
  }

  _lastHumidity = currentHumidity & 0x7FFF;
  if(currentTemperature & 0x8000)
  {
    // Below zero, non standard way of encoding negative numbers!
    // Convert to native negative format.
    _lastTemperature = -(currentTemperature & 0x7FFF);
  }
  else
  {
    _lastTemperature = currentTemperature;
  }

  csPart1 = currentHumidity >> 8;
  csPart2 = currentHumidity & 0xFF;
  csPart3 = currentTemperature >> 8;
  csPart4 = currentTemperature & 0xFF;

  for (i = 0; i < DHT22_DATA_BIT_COUNT; i++)
	  KbitTimes[i] = bitTimes[i];

  if(checkSum == ((csPart1 + csPart2 + csPart3 + csPart4) & 0xFF))
  {
    return DHT_ERROR_NONE;
  }
  return DHT_ERROR_CHECKSUM;
}
//
// This is used when the millis clock rolls over to zero
//
void DHT22::clockReset()
{
  _lastReadTime = millis();
}