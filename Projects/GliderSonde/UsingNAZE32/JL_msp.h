#pragma once

#define MspSerial Serial

#define MSP_RAW_GPS      106
#define MSP_ALTITUDE     109

#define MSP_SET_RAW_RC 200


byte crc = 0;
const unsigned char MSP_HEADER[] = { '$', 'M', '>' };

struct MSP_LOC {
	uint8_t size;
	uint8_t type;

	uint8_t fixType;
	uint8_t numSat;
	int32_t lat;
	int32_t lon;
	int16_t alt; // meters
	int16_t groundSpeed;
	int16_t groundCourse;
	//uint16_t hdop;         
};

struct MSP_ALT {
	uint8_t size;
	uint8_t type;

	int32_t alt;
	int16_t vario;
};

union MSPMessageIn {
	MSP_LOC msploc;
	MSP_ALT mspalt;
};
MSPMessageIn MspmsgIn;

//////////////////////////////////
void mspSerialize8(byte b) {
	MspSerial.write(b);
	crc ^= b;
}
void mspSerialize16(int16_t a) {
	mspSerialize8((a) & 0xFF);
	mspSerialize8((a >> 8) & 0xFF);
}
void mspSerialize32(uint32_t a) {
	mspSerialize8((a) & 0xFF);
	mspSerialize8((a >> 8) & 0xFF);
	mspSerialize8((a >> 16) & 0xFF);
	mspSerialize8((a >> 24) & 0xFF);
}
//////////////////////////////////////
byte calcMSPChecksum(int payloadSize) {
	byte b = 0;
	//softSerial.print("calcing ");
	for (int i = 0; i < payloadSize; i++) b ^= ((byte*)(&MspmsgIn))[i];
	return b;
}
///////////////////////////////////////
void mspRequest(byte a) {
	mspSerialize8('$');
	mspSerialize8('M');
	mspSerialize8('<');
	crc = 0;
	mspSerialize8(0);
	mspSerialize8(a);
	MspSerial.write(crc);
	crc = 0;
}
////////////////////////////////
boolean mspGet(byte a) {
	
	static int fpos = 0;
	static byte checksum;
	static int payloadSize = sizeof(MSPMessageIn);
	while (Serial.available()) {
		byte c = Serial.read();

		if (fpos<3) {
			if (c == MSP_HEADER[fpos]) fpos++;
			else { fpos = 0; }
		}

		else {//not 0~2
			int payloadPos = fpos - 3;
			if (payloadPos < payloadSize) ((byte*)(&MspmsgIn))[payloadPos] = c;
			if (payloadPos == 1) {
				if (a != c) break;//if not intended
				switch (c) {
				case MSP_RAW_GPS: payloadSize = sizeof(MSP_LOC); break;
				case MSP_ALTITUDE: payloadSize = sizeof(MSP_ALT); break;
				}
			}//Message Type Checked.

			fpos++;
			if (payloadPos == payloadSize) {
				if (c == calcMSPChecksum(payloadSize)) { fpos = 0; return true; }
				else { return false; }
			}
			else if (payloadPos>payloadSize) { fpos = 0; }
		}//fpos>3
	}//while Available
	return false;
}
