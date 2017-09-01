#ifndef __JunwooHWNAG_PID_H__
#define __JunwooHWNAG_PID_H__

//#include"JunwooHWANG.h"

// Roll
float Pid_Kp_Roll = 0;
float Pid_Ki_Roll = 0;
float Pid_Kd_Roll = 0;

float Pid_setpoint_Roll = 0;
int Pid_Output_Roll = 0;

float Pid_i_mem_Roll = 0;
float Pid_d_mem_Roll = 0;
int Pid_max_Roll = 400.0;

// Pitch
float Pid_Kp_Pitch = 0;
float Pid_Ki_Pitch = 0;
float Pid_Kd_Pitch = 0;

float Pid_setpoint_Pitch = 0;
int Pid_Output_Pitch = 0;

float Pid_i_mem_Pitch = 0;
float Pid_d_mem_Pitch = 0;
int Pid_max_Pitch = 400.0;

float Error_Temp = 0;

//1
void Pid_calculate_Output(boolean i_en=false,boolean d_en=false) {
	//Roll CALCULATION
	Error_Temp = Pid_setpoint_Roll - Gyro_dps[1];//Roll = Y Axis.
	Pid_Output_Roll = (Error_Temp)*Pid_Kp_Roll;// + Pid_i_mem_Roll + (Gyro_dps[1]-Pid_d_mem_Roll)*Pid_Kd_Roll;
	if (i_en) {
		Pid_i_mem_Roll += Error_Temp*Pid_Ki_Roll; if (Pid_i_mem_Roll > Pid_max_Roll) { Pid_i_mem_Roll = Pid_max_Roll; }
		else if (Pid_i_mem_Roll < -1 * Pid_max_Roll) { Pid_i_mem_Roll = -Pid_max_Roll; }
	}
	if (d_en) {
		Pid_Output_Roll -= (Error_Temp - Pid_d_mem_Roll)*Pid_Kd_Roll;
		Pid_d_mem_Roll = Error_Temp;
	}
	if (Pid_Output_Roll>Pid_max_Roll) { Pid_Output_Roll = Pid_max_Roll; }
	else if (Pid_Output_Roll<-1 * Pid_max_Roll) { Pid_Output_Roll = -Pid_max_Roll; }

	//Pitch CALCULATION
	Error_Temp = Pid_setpoint_Pitch - Gyro_dps[0];//Pithc = X Axis.
	Pid_Output_Pitch = (Error_Temp)*Pid_Kp_Pitch;
	if (i_en) {
		Pid_i_mem_Pitch += Error_Temp*Pid_Ki_Pitch; if (Pid_i_mem_Pitch > Pid_max_Pitch) { Pid_i_mem_Pitch = Pid_max_Pitch; }
		else if (Pid_i_mem_Pitch < -1 * Pid_max_Pitch) { Pid_i_mem_Pitch = -Pid_max_Pitch; }
	}
	if (d_en) {
		Pid_Output_Pitch -= (Error_Temp - Pid_d_mem_Pitch)*Pid_Kd_Pitch;
		Pid_d_mem_Pitch = Error_Temp;
	}
	if (Pid_Output_Pitch>Pid_max_Pitch) { Pid_Output_Pitch = Pid_max_Pitch; }
	else if (Pid_Output_Pitch<-1 * Pid_max_Pitch) { Pid_Output_Pitch = -Pid_max_Pitch; }
}

#endif