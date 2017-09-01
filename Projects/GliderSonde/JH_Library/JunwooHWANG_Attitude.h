#ifndef __JunwooHWNAG_Attitude_H__
#define __JunwooHWNAG_Attitude_H__

//#include"JunwooHWANG.h"

float Acc_Pitch = 0;
float Acc_Roll = 0;

float Vehicle_Pitch = 0;
float Vehicle_Roll = 0;

float Acc_data_correction = 0.05;
float One_minus_Acc_data_correction = 1 - Acc_data_correction;

//1
void Attitude_calculate() {
	float Total_g_vec = sqrt(Acc_g[0] * Acc_g[0] + Acc_g[1] * Acc_g[1] + Acc_g[2] * Acc_g[2]);
	if (abs(Acc_g[1]) <= Total_g_vec) Acc_Pitch = asin(Acc_g[1] / Total_g_vec)*RAD_TO_DEG;//calculate Acc angle 
	if (abs(Acc_g[0]) <= Total_g_vec) Acc_Roll = -asin(Acc_g[0] / Total_g_vec)*RAD_TO_DEG;
	Vehicle_Pitch += Gyro_dps[0] * (0.004);//add gyroscope angle, 250Hz
	Vehicle_Roll += Gyro_dps[1] * (0.004);
	Vehicle_Pitch = Vehicle_Pitch*One_minus_Acc_data_correction + Acc_Pitch*Acc_data_correction;//Apply Acc angle correction
	Vehicle_Roll = Vehicle_Roll*One_minus_Acc_data_correction + Acc_Roll*Acc_data_correction;
}

#endif