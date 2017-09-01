#ifndef __JunwooHWNAG_L3G4200D_H__
#define __JunwooHWNAG_L3G4200D_H__

//#include"JunwooHWANG.h"

#define GYRO_ADDRESS 0x69//address of gyro(L3G4200D,SD0pin(4) held HIGH)
#define GYRO_CALIBRATE_COUNT 1000
#define GYRO_THRESHOLD_MULTIPLE 4//Determines Threshold Value

float Gyro_dps_per_digit = 0.0175;//0.0175 degree/s = 1 digit.
int Gyro_RAW[3];//X,Y,Z value straight from the sensor.
int Gyro_Cal[3];//calibration data
float Gyro_dps[3];//final result
int Gyro_Threshold[3];//threshold for Gyro_RAW
boolean Gyro_Calibrated = false;

//1
boolean Gyro_init() {
	if (ReadByte(GYRO_ADDRESS, 0x0F) != 0xD3)//'WHO_AM_I' register Check.
		return false;
	if (!WriteByte(GYRO_ADDRESS, 0x20, B00101111))//<CTRL_REG1> 'DR to 400Hz(B11)' & 'set PD bit(3) to 1(normal/sleep mode)' & 'XYZ enable'
		return false;
	if (!WriteByte(GYRO_ADDRESS, 0x23, B10010000))//<CTRL_REG4> set FS10 bit(5,4) = 0,1. AND BDU bit(7) =1<BLOCK DATA UPDATE>. (500 dps scale.)
		return false;
	return true;
}

//2
boolean Gyro_readData() {
	Wire.beginTransmission(GYRO_ADDRESS);
	Wire.write(0x28 | 1 << 7);//Read from <OUT_X_L>. AND, MSB for the 'auto-increment'
	if (Wire.endTransmission(false)) return false;
	Wire.requestFrom(GYRO_ADDRESS, 6);
	while (Wire.available()<6);
	for (int i = 0; i<3; i++) {
		Gyro_RAW[i] = Wire.read() | Wire.read() << 8;
		if (Gyro_Calibrated) Gyro_dps[i] = (abs(Gyro_RAW[i])>Gyro_Threshold[i]) ? ((Gyro_RAW[i] - Gyro_Cal[i])*Gyro_dps_per_digit) : (0);
		else Gyro_dps[i] = Gyro_RAW[i] * Gyro_dps_per_digit;
	}
	return true;
}

//3
void Gyro_getCalibrationData() {
	//place to store all squared data, at GYRO_CALIBRATE_COUNT==1000, each squared shouldn't exceed: (2^32-1)/1000.
	//Which means that each number shouldn't exceed 2072.43. and that is, 36.267 deg/s. So I consider this safe for now.
	long SUM[3] = {};//place to store sum of all data
	unsigned long Squared[3] = {};
	for (int i = 0; i<3; i++) { SUM[i] = 0; Squared[i] = 0; }

	for (int i = 0; i<GYRO_CALIBRATE_COUNT; i++) {
		Gyro_readData();
		for (int j = 0; j<3; j++) {SUM[j] += Gyro_RAW[j];Squared[j] += Gyro_RAW[j] * Gyro_RAW[j];delayMicroseconds(4000);}//for each axis
	}

	for (int i = 0; i < 3; i++) {
		Gyro_Cal[i] = SUM[i] / GYRO_CALIBRATE_COUNT;//int type
		Gyro_Threshold[i] = abs( sqrt(Squared[i] / GYRO_CALIBRATE_COUNT - Gyro_Cal[i] * Gyro_Cal[i]) )*GYRO_THRESHOLD_MULTIPLE;}
	// sqrt {sigma_squared/totalNum - Mean^2 } * CONSTANT
	Gyro_Calibrated = true;
}

#endif