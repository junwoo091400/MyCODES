#ifndef __JunwooHWNAG_ADXL345_H__
#define __JunwooHWNAG_ADXL345_H__

//#include"JunwooHWANG.h"

#define ACC_ADDRESS 0x53//address of accelerometer(ADXL345,ALT ADDRESS(12) held LOW)

float Acc_g_per_digit = 0.0039;//0.0039 g force = 1 digit.(+-2g mode, or Full_Resolution Mode)
int Acc_RAW[3];//X,Y,Z value straight from the sensor.
float Acc_g[3];//final result

//1
boolean Acc_init() {
	if (ReadByte(ACC_ADDRESS, 0x00) != 0xE5) return false;//DVID register check.
	if (!WriteByte(ACC_ADDRESS, 0x2C, 0x0C)) return false;//Output Data Rate 400Hz(B1100) & Normal_Operation Mode.
	if (!WriteByte(ACC_ADDRESS, 0x2D, 0x08)) return false;//'Measure_Mode' ON.
	if (!WriteByte(ACC_ADDRESS, 0x31, 0x09)) return false;//Full Resolution Mode ( 0.4 mg / LSB no matter what Range is selected.) & +-4g limit.
	return true;
}

//2
boolean Acc_readData() {
	Wire.beginTransmission(ACC_ADDRESS);
	Wire.write(0x32 | 1 << 7);//Read from AND, MSB for the 'auto-increment' OR ELSE, auto_increment won't happen and Slave will keep giving the same data!
	if (Wire.endTransmission(false)) return false;
	Wire.requestFrom(ACC_ADDRESS, 6);//read 6 bytes, then give 'stop'
	while (Wire.available()<6);//wait until all data arrives
	for (int i = 0; i<3; i++) {
		Acc_RAW[i] = Wire.read() | Wire.read() << 8;//RAW data reading...
		Acc_g[i] = Acc_RAW[i] * Acc_g_per_digit;
	}
	return true;
}

//3

#endif