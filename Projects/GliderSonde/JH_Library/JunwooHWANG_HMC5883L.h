#ifndef __JunwooHWANG_HMC5883L_H__
#define __JunwooHWANG_HMC5883L_H__

#define MAG_ADDRESS 0x1E

int Mag_RAW[3] = {};
float Mag_Gauss_per_LSB = 0.00073;//GN=0
float Mag_gauss[3] = {};
/*
----------------------------------------------------------
<Additional Information>
Earth's Magnetic Field Strength is in [0.25 ~ 0.65[Gauss]]
----------------------------------------------------------
*/

/*
----------------------------------------------------------
<Sensor Output rate & measurement configuration>

	[in Configuration Register A(0x00)]
	| 7 (0) | 6 MA1(0) | 5 MA(0) | 4 DO2(1) || 3 DO1(0) | 2 DO0(0) | 1 MS1(0) | 0 MS0 (0) |

	+Bit6:5(MA) = #of samples averaged(1~8) per measurement output (00 = 1, 01 = 2, 10 = 4, 11 = 8)
	+Bit4:2(DO) = Data Output Rate[Hz] (0.75, 1.5, 3, 7.5, 15(Default), 30, 75, Reserved)
	+Bit1:0(MS) = Measurement Mode (00 = Normal measurement, 01 = Positive bias, 10 = Negative bias, 11 = reserved)

----------------------------------------------------------
<Sensor Sensitivity>
	
	[in Configuration Register B(0x01) Bit7:5 (3-bit) called 'GN(Gain)' ] 
	
	+Recommended Sensor Field Range+
	| 0: 0.88[Ga] | 1: 1.3[Ga] | 2: 1.9[Ga] | 3: 2.5[Ga] |
	| 4: 4.0[Ga]  | 5: 4.7[Ga] | 6: 5.6[Ga] | 7: 8.1[Ga] |
	
	+Digital Resuolution (mG/LSB)+
	| 0: 0.73 | 1: 0.92 | 2: 1.22 | 3: 1.52 |
	| 4: 2.27 | 5: 2.56 | 6: 3.03 | 7: 4.35 |
	
	+Sensor Output Value+
	0xF800 ~ 0x07FF (-2048 ~ 2047)

----------------------------------------------------------
<Sensor SelfTest>
	HOWTO:
	1) WriteByte(MAG_ADDRESS,0x00,0x71) // (CRA) 8-average, 15Hz default, positive self test measurement
	2) WriteByte(MAG_ADDRESS,0x01,0xA0) // (CRB) Gain=5 (2.56mG/LSB)
	3) WriteByte(MAG_ADDRESS<0x02,0x00) // (Mode Register) continuous-Measurement Mode
	4) wait 6ms or monitor status register(0x09) bit0(RDY bit).

*/

//1
void Mag_init() {
	WriteByte(MAG_ADDRESS, 0x02, 0x00);//MODE register 'continous'
}

//2
void Mag_readData() {
	Wire.beginTransmission(MAG_ADDRESS);
	Wire.write(0x03);//X LSB address
	Wire.endTransmission(false);
	Wire.requestFrom(MAG_ADDRESS, 6);
	while (Wire.available() < 6);
	Mag_RAW[0] = Wire.read() | Wire.read() << 8;//X
	Mag_RAW[2] = Wire.read() | Wire.read() << 8;//Z
	Mag_RAW[1] = Wire.read() | Wire.read() << 8;//Y
	for (int i = 0; i < 3; i++) { Mag_gauss[i] = Mag_RAW[i]*Mag_Gauss_per_LSB; }
}

//3

#endif