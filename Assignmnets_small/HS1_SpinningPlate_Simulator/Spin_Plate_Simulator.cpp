/*
Experiment on a Subject without mass at absolute position,
only considering the friction as a force & torque as angular acceleration factor
MASS is not considered!
*/
#include<stdio.h>
#pragma warning(disable:4996)
#include<math.h>

int main() {
	unsigned long time = 0;//micros
	double R, w, sytaR;// distance from origin ,angular speed of rotor, atan2(Y,X)
	double g = 9.8, uk;// g_acceleration, friction constant

	double sytaFriction;

	double Last_SytaR = 0, dSytaR = 0, Last_dSytaR = 0, ddSytaR = 0, Last_ddSytaR = 0, dddSytaR = 0;

	double Vcm[2], Rcm[2], Vtan[2], Vrel_toall[2], Atotal[2];//Vectors

	///////////////////////////////////////////////////////////////////////////
	printf("ENTER w_rotor,X,Y\n");
	scanf("%lf,%lf,%lf", &w, &Rcm[0], &Rcm[1]);

	printf("ENTER VcmX,VcmY\n");
	scanf("%lf,%lf", &Vcm[0], &Vcm[1]);

	printf("ENTER uk\n");
	scanf("%lf", &uk);
	////////////////////////////////////////////////////////////////////////////
	
	printf("PRINTING EVERY 0.05 Seconds\n");
	printf("RcmX,RcmY,Angular_V,dAlpha/dt,dSyta+2dddSyta^3\n");

	/////////////////////////////
	while (1) {
		time++;
		Atotal[0] = 0;
		Atotal[1] = 0;//Initialize Force Vector

		Vtan[0] = -Rcm[1]*w;
		Vtan[1] = Rcm[0]*w;
		//Tangent Speed Calculated

		Vrel_toall[0] = Vcm[0] - Vtan[0];//X relative Spped
		Vrel_toall[1] = Vcm[1] - Vtan[1];//Y relative Speed
		//Relative Speed Calculated

		sytaFriction = atan2(-Vrel_toall[1], -Vrel_toall[0]);

		Atotal[0] = uk*g*cos(sytaFriction);
		Atotal[1] = uk*g*sin(sytaFriction);
		//Calculation of Acceleration Complete!!


		//////////////////////Position Calculation///////////////////////////
		Vcm[0] += (Atotal[0])*0.000001;//Vcm x-part add
		Vcm[1] += (Atotal[1])*0.000001;

		Rcm[0] += Vcm[0] * 0.000001;
		Rcm[1] += Vcm[1] * 0.000001;//X,Y coordinate calculated
		 //////////////////////////////////////////////////////////////////////////
		 
		 R = sqrt(Rcm[0] * Rcm[0] + Rcm[1] * Rcm[1]);
		 sytaR = atan2(Rcm[1], Rcm[0]);
		 
		 ////////////////What youknow wants calculation///////////////////////
		
		dSytaR = sytaR - Last_SytaR;
		//dSytaR /= 0.000001;//Rad/s of CM to Origin

		ddSytaR = (dSytaR - Last_dSytaR);//Right now, don' divide...
		//ddSytaR /= 0.000001;

		dddSytaR = ddSytaR - Last_ddSytaR;
		//dddSytaR /= 0.000001;

		Last_SytaR = sytaR;
		Last_dSytaR = dSytaR;
		Last_ddSytaR = ddSytaR;

		/////////////////////////////////////////////////////////////////
		if (time % 50000 == 0) {//every 0.05s
			
		double d_syta_dt, dd_syta_dt_2,ddd_syta_dt_3;//Declare New variables
		
		d_syta_dt = dSytaR * pow(10, 6);
		dd_syta_dt_2 = ddSytaR*pow(10, 12);
		ddd_syta_dt_3 = dddSytaR*pow(10, 18);//Wow

		
			printf("%f,%f,", Rcm[0], Rcm[1]);
			printf("%f,%f,", d_syta_dt, ddd_syta_dt_3);
			printf("%f\n", d_syta_dt + 2 * pow(ddd_syta_dt_3, 3));
		}
	}
	return 0;
}