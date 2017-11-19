#pragma warning(disable:4996)
#include<stdio.h>
#include"DataBase.h"
#include"Communicator.h"
//#include<conio.h>

void Print_Schedule(Schedule dude) {
	/*
	This function prints the schedule of 'dude'
	STRUCTURE : Schedule(1)-One_Day(5)-One_Class(9).
	Hmmm.. it is hard to get everything right. Hangul is so LARGE!!!
*********************************************************************
	<Schedule.ClassNum반 Schedule.Number번 Schedule.name의 시간표> 
	
		|	월요일	|	화요일	|	수요일	|	목요일	|	금요일	|
=====================================================================		
1교시	|			|			|			|			|			|
		|			|			|			|			|			|
		|			|			|			|			|			|
---------------------------------------------------------------------
2교시	|			|			|			|			|			|
		|			|			|			|			|			|
		|			|			|			|			|			|
---------------------------------------------------------------------
3교시	|			|			|			|			|			|
		|			|			|			|			|			|
		|			|			|			|			|			|
---------------------------------------------------------------------
4교시	|			|			|			|			|			|
		|			|			|			|			|			|
		|			|			|			|			|			|
---------------------------------------------------------------------
5교시	|			|			|			|			|			|
		|			|			|			|			|			|
		|			|			|			|			|			|
---------------------------------------------------------------------
6교시	|			|			|			|			|			|
		|			|			|			|			|			|
		|			|			|			|			|			|
---------------------------------------------------------------------
7교시	|			|			|			|			|			|
		|			|			|			|			|			|
		|			|			|			|			|			|
---------------------------------------------------------------------
8교시	|			|			|			|			|			|
		|			|			|			|			|			|
		|			|			|			|			|			|
---------------------------------------------------------------------
9교시	|			|			|			|			|			|
		|			|			|			|			|			|
		|			|			|			|			|			|
=====================================================================
*/
}
void BigBoyHotSetting(){
	for (int i = 0; i < 6; i++) {
		char String[] = "BigBoy2-0";
		String[8] = i + '1';//bigboy0 = class1.
		BigBoy[i].Name = (char*)malloc(10);
		strcpy(BigBoy[i].Name, String);
		BigBoy[i].ClassNum = i + 1;
		BigBoy[i].Number = 0;
	}
}
int main() {
	/*
	FILE* fp = fopen("C:\\CALL\\Class_OnE.txt","wt");
	Schedule Customer_1 = {};//person
	if (fp == NULL)
		printf("Null pointer Exceptin\n");
	Customer_1.ClassNum = 99;
	Customer_1.Name = "Junwoo HWAGN";
	Customer_1.Number = 69;
	Export_Schedule_csv(Customer_1, fp);
	*/
	BigBoyHotSetting();//initialize BigBoy
	Page_Main();

	return 0;
}