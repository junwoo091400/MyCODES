#ifndef __JunwooHWANG_BMP085_H__
#define __JunwooHWANG_BMP085_H__

#define BMP085_AC1_REG 0xAA
#define BMP085_ADDRESS 0x77//7 bit I2C address.

typedef struct {
	short AC1;	//Stores the data read from EEPROM of BMP085. Used for calibration
	short AC2; 
	short AC3;
	unsigned short AC4;
	unsigned short AC5;
	unsigned short AC6;
	short BB1;
	short B2;
	short MB;
	short MC;
	short MD;
}BMP085_CAL;

BMP085_CAL BMcal;//variable.
long UPP;//Pressure DATA
long UT;//Temperature DATA
short oss = 3;//oversampling setting(0~3), Can be added as 6&7th bit in 0xF4(default) Register.

const float p0 = 101325;//Pressure at sea level(Pa)
long Baro_final_T;//in 0.1 degCel.
long Baro_final_P;//int Pa. At Sea Level : 101300.

boolean gotCalibrationData = false;

//1
void getCalibrationData_BMP085() {//Gets EEPROM Data 
	Wire.beginTransmission(BMP085_ADDRESS);
	Wire.write(BMP085_AC1_REG);//first register of EEPROM Data
	Wire.endTransmission(false);//don't release the I2C line.
	Wire.requestFrom(BMP085_ADDRESS, 22);//total 176 bit. (16 bit chunk)*Total of 11 members!
	while (Wire.available()<22);//wait until all data gets received.
	for (int i = 0; i<sizeof(BMP085_CAL) / sizeof(short); i++) {//2 Bit COOKIE~~~! Cuz Arduino puts LSB first on 'insertion'
		((short*)&BMcal)[i] = (short)((unsigned short)Wire.read() << 8 | Wire.read());
	}
}

//2
void applyCalibration_BMP085() {
	long X1 = ((UT - BMcal.AC6)*BMcal.AC5) >> 15;//UT is 'long' so no worries
	long X2 = ((long)BMcal.MC << 11) / (X1 + BMcal.MD);//If no 'long', than bit disappears!!(Magic!)
	long B5 = X1 + X2;
	Baro_final_T = (B5 + 8) >> 4;//ex) 150 = 14.0(degC) + Calculate <Temperature>

	long B6 = B5 - 4000;
	X1 = (BMcal.B2*(B6*B6) >> 12) >> 11;
	X2 = (BMcal.AC2*B6) >> 11;
	long X3 = X1 + X2;
	long B3 = ((((long)BMcal.AC1 * 4 + X3) << oss) + 2) >> 2;//uses 'X3'
	X1 = (BMcal.AC3*B6) >> 13;
	X2 = (BMcal.BB1*(B6*B6) >> 12) >> 16;
	X3 = (X1 + X2 + 2) >> 2;
	unsigned long B4 = (BMcal.AC4*(unsigned long)(X3 + 32768)) >> 15;
	unsigned long B7 = ((unsigned long)(UPP - B3)*(50000 >> oss));

	if (B7<0x80000000)
		Baro_final_P = (B7 << 1) / B4;
	else
		Baro_final_P = (B7 / B4) << 1;

	X1 = (Baro_final_P >> 8)*(Baro_final_P >> 8);//upper bit Squared.
	X1 = (X1 * 3038) >> 16;
	X2 = (-7357 * Baro_final_P) >> 16;
	Baro_final_P += (X1 + X2 + 3791) >> 4;
	//Calculate <Pressure>
}

//3
void Baro_readData() {
	WriteByte(BMP085_ADDRESS, 0xF4, 0x2E);//Start reading Temperature Data
	delayMicroseconds(4500);//Wait for 4.5ms to allow sensor to write Data on 0xF6 OUTPUT register.
	Wire.beginTransmission(BMP085_ADDRESS);
	Wire.write(0xF6);//address of Temperature Register.
	Wire.endTransmission(false);//keep on talking.
	Wire.requestFrom(BMP085_ADDRESS, 2);//2 Byte Temp.data
	while (Wire.available()<2);
	UT = Wire.read() << 8 | Wire.read();//MSB first

	WriteByte(BMP085_ADDRESS, 0xF4, (0x34 | oss << 6));//Write (0x34|oss<<6) into 0xF4 register!!
	delay(25);//Wait for 25ms to allow sensor to write Data on 0xF6 OUTPUT register.
	Wire.beginTransmission(BMP085_ADDRESS);
	Wire.write(0xF6);//address of Pressure Register.
	Wire.endTransmission(false);//keep on talking.
	Wire.requestFrom(BMP085_ADDRESS, 3);//3 Byte Pressure.data
	while (Wire.available()<3);//[MSB,LSB,XLSB]
	UPP = (((unsigned long)Wire.read() << 8 | Wire.read()) << 8 | Wire.read()) >> (8 - oss);

	applyCalibration_BMP085();//Apply Calibration
}

//4
float Calculate_Altitude(long pressure) {
	float altitude = (float)44330 * (1 - pow(((float)pressure / p0), 0.190295));
	return altitude;//[m]
}

//5
void Baro_init() {if (!gotCalibrationData) { getCalibrationData_BMP085(); }; gotCalibrationData = true;}

#endif