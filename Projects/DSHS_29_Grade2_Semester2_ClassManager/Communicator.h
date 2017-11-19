#pragma once
#include<stdlib.h>
#include<string.h>
#include<windows.h>
void Remove_ln() {
	if (!feof(stdin))
		while (getc(stdin) != '\n');
}
void Page_ModifyData();
void Page_Import_csv();
void Page_DataExport();
void Ask_WhereToGo();

char* Export_Dir = "C:\\Users\\기본\\Desktop\\10K Hours-JunwooHWANG\\!Programming\\DSHS_29_Grade2_Semester2_ClassManager\\Storage\\Exported\\";
char* Import_Dir = "C:\\Users\\기본\\Desktop\\10K Hours-JunwooHWANG\\!Programming\\DSHS_29_Grade2_Semester2_ClassManager\\Storage\\ToImport\\";

/*
enum USER_POSITION {
	MAIN=0,
	SeeMySchedule,
	ModifyData,
	DataExport
};
USER_POSITION Current_User = MAIN;
*/

char* Combine_Directory_N_Filename(char *Main, char*Sub) {
	int Main_len=0, Sub_len=0;
	while (Main[Main_len++] != '\0');
	while (Sub[Sub_len++] != '\0');
	char* Newstring = (char*)malloc(Main_len + Sub_len - 1);
	for (int i = 0; i < Main_len + Sub_len - 1; i++) {
		if (i < Main_len - 1)
			Newstring[i] = Main[i];
		else
			Newstring[i] = Sub[i - (Main_len - 1)];
	}
	return Newstring;
}

void Export_Schedule_csv(Schedule dude, FILE* fp) {
	fprintf(fp, ",2-%d반,%d번,%s\n\n", dude.ClassNum, dude.Number, dude.Name);//information
	fprintf(fp, "교시,,월요일,,화요일,,수요일,,목요일,,금요일\n");
	for (int Period = 0; Period < 9; Period++) {//교시
		fprintf(fp, "%d,과목명", Period + 1);
		for (int Day = 0; Day < 5; Day++)//일
			fprintf(fp, ",%s,", dude.DayofWeek[Day].PeriodofDay[Period].ClassName);
		fputc('\n', fp);

		fprintf(fp, ",담당교사");
		for (int Day = 0; Day < 5; Day++)//일
			fprintf(fp, ",%s,", dude.DayofWeek[Day].PeriodofDay[Period].ClassTeacher);
		fputs(",\n", fp);

		fprintf(fp, ",강의실");
		for (int Day = 0; Day < 5; Day++)//일
			fprintf(fp, ",%s,", dude.DayofWeek[Day].PeriodofDay[Period].Location);;
		fputs(",\n", fp);

	}
}

char* NextString(FILE* fp) {

	int *buffer = (int*)malloc(16 * sizeof(int));//max. 15 Korean Letters
	int MaxPosition = 16;//initial
	int Position = 0;
	while (1) {
		//printf("POSITION : %d\n", Position);
		buffer[Position] = fgetc(fp);//get 'em!
		if (buffer[Position] == ',' || buffer[Position] == EOF)
			break;
		else if (buffer[Position] == '\n') {//Change Line is MY ENEMY! You made 'Class2 not same as \nClass2' DANG!
			Position--;//override this area again next time.
		}
		else if (Position == MaxPosition - 1) {//if 'sizeof', man. You are calculating the size of the POINTER!!!
			MaxPosition += 10;
			int *TEMP = (int*)malloc(MaxPosition);//add 5 more letters.
			for (int i = 0; i <= Position; i++)
				TEMP[i] = buffer[i];
			free(buffer);
			buffer = TEMP;//update 'pointer'
		}
		else if (Position == 100) {//100th. This is CRAZY, return DUMMY
								   //because 30+10*7, so 7 malloc was called,, you bastard!
			buffer[Position] = '!';//send this to the 'if' statement
			break;
		}
		Position++;//don't forget to update!
	}

	if (buffer[Position] != ',') {//not what we wanted
		free(buffer);
		const char ErrorMessage[] = "ERROR, No Comma Found!";
		char* String = (char*)malloc(sizeof(ErrorMessage));
		strcpy(String, ErrorMessage);
		return String;
	}
	else {//have a stable ','
		char* String = (char*)malloc(Position + 1);

		for (int i = 0; i < Position; i++)
			String[i] = (char)buffer[i];
		String[Position] = '\0';//last one is 'null'
		free(buffer);
		//printf("EHY\n");
		return String;
	}
}

void Update_csv(char* Link) {

	FILE* fp = fopen(Link, "rt");
	if (fp == NULL) {
		printf("Can't open the File!\n");
		return;
	}
	BigBoy;
	char ClassHeader[7] = "Class1";
	char* ClassnameHeader = "과목명";
	char* TeacherHeader = "담당교사";
	char* LocationHeader = "강의실";

	for (int ClassCount = 0; ClassCount < 6; ClassCount++) {//Class

		printf("-------------------------\n");
		printf("ClassCount = %d ", ClassCount);
		ClassHeader[5] = (char)ClassCount + '1';//'i' ASCII nujmber FINAL (1~6 class)
		ClassHeader[6] = '\0';//NULL
		printf("ClassHeader Set As \"%s\"\n", ClassHeader);
		char *TEMP_String;//For DEBUGGING in the Future!

		////////////////////////////////////////////////////////////////////////////////////////////////////////
		//0) Find 'ClassHeader'
		printf("Looking for ClassHeader...");
		while (strcmp(ClassHeader, TEMP_String = NextString(fp)));//Stay in this loop until Finding a Class Header.
		printf("Found... ");
		printf("Confirmed as a Class%d\n", ClassCount);
		
		//1) Get 'CLASS NAME'
		printf("Looking for ClassnameHeader...");
		while ( strcmp(ClassnameHeader, TEMP_String = NextString(fp) ) );//Stay in this loop until Finding a Class Header.
		printf("Found... ");
		for (int DayCount = 0; DayCount < 5; DayCount++)//9 period. ClassName
			for (int PeriodCount = 0; PeriodCount < 9; PeriodCount++)
				BigBoy[ClassCount].DayofWeek[DayCount].PeriodofDay[PeriodCount].ClassName = NextString(fp);
		printf("Done Uploading ClassName\n");
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		//2) Get 'Teacher Info'
		printf("Looking for TeacherHeader...");
		while ( strcmp(TeacherHeader, TEMP_String = NextString(fp) ) );//Stay in this loop until Finding a Teacher Header.
		printf("Found... ");
			for (int DayCount = 0; DayCount < 5; DayCount++)//9 period. ClassName
				for (int PeriodCount = 0; PeriodCount < 9; PeriodCount++)
					BigBoy[ClassCount].DayofWeek[DayCount].PeriodofDay[PeriodCount].ClassTeacher = NextString(fp);
		printf("Done Uploading ClassTeacher\n");
		//////////////////////////////////////////////////////////////////////////////////////////////////////////

		//3) Get 'Class Location'
		printf("Looking for LocationHeader...");
		while (strcmp(LocationHeader, NextString(fp)));//Stay in this loop until Finding a Location Header.
		printf("Found... ");
			for (int DayCount = 0; DayCount < 5; DayCount++)//9 period. ClassName
				for (int PeriodCount = 0; PeriodCount < 9; PeriodCount++)
					BigBoy[ClassCount].DayofWeek[DayCount].PeriodofDay[PeriodCount].Location = NextString(fp);
		printf("Done Uploading Location\n");
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
	}//end of 'ClassCount' for statement.
	printf("DONE Updating BigBoy via csv format\n");
}


/*
//////////////////////////////<PAGES>/////////////////////////////////
*/

void Page_DeveloperMode() {
	printf("Developer Mode\n");
	Update_csv(Combine_Directory_N_Filename(Import_Dir, "Ha_ShorterVer.txt"));
	printf("CSV update done.\n");
	Ask_WhereToGo();
}

void Ask_WhereToGo() {
	printf("-------------------------\n");
	printf("What Page To go?\n");
	printf("1>See My Schedule\n2>Modify Data\n3>Data Export\n4>Import Data (csv)\n5>DeveloperMode\n");
	printf("TYPE> ");
	int User_Input;
	scanf("%d", &User_Input);
	switch (User_Input) {
	case 1: break;
	case 2: Page_ModifyData(); break;
	case 3: Page_DataExport(); break;
	case 4: Page_Import_csv(); break;
	case 5: Page_DeveloperMode(); break;
	}
}

void Page_Import_csv() {
	system("cls");
	printf("File Name?\n");
	printf("> ");
	char DataLoc[50];
	scanf("%s", DataLoc);
	Remove_ln();
	Update_csv(Combine_Directory_N_Filename(Import_Dir,DataLoc));
	Ask_WhereToGo();
}

void Page_ModifyData() {
	system("cls");
	printf("For Modifying Data, Select Type\n");
	printf("1)ModifyClass\t2)Modify선택과목\t3)ModifyALL\n");
	printf(">");
	int Type;
	scanf("%d", &Type);
	Remove_ln();
	switch (Type) {
	case 1:
	case 2:
	case 3: break;
	}
	Ask_WhereToGo();
}

void Page_DataExport() {
	system("cls");
	printf("What kind of data do you want to Export?");
	printf("1>BigBoyData\t2>선택과목Data\t3>학생목록Data\n");
	printf("> ");
	int Selection;
	scanf("%d", &Selection);
	Remove_ln();//remove \n
	char STRING[50];
	if (Selection == 1) {
		printf("What Range of Big Boy Data do you want to export?\n");
		printf("(a~b)> ");
		char Start = 0, End = -1;
		scanf("%d~%d", &Start, &End);
		if (Start >= 0 && Start < 6 && End < 6 && End >= 0 && Start <= End) {
			printf("File Name?\n");
			printf("> ");
			scanf("%s", STRING);
			FILE* TEMP = fopen(Combine_Directory_N_Filename(Export_Dir,STRING), "wt");
			
			if (TEMP != NULL) {
				for (int i = Start; i <= End; i++)
					Export_Schedule_csv(BigBoy[i], TEMP);
			}
			else
				printf("Failed to Create File\n");
		}//correct input of "a~b"
		else
			printf("Wrong input of \"a~b\"\n");
	}
	else if (Selection == 2) {//선택과목 export

	}
	else if(Selection==3){//학생목록 export

	}
	else
		printf("Wrong Input\n");//Wrong input
	Ask_WhereToGo();
}

void Page_Main() {
	/*
	1.SEE MY Schedule
	2.Modify DATA
	3.Program Export
	*/
	system("cls");
	printf("<DSHS_29_Grade2_ClassManager>\n");
	printf("Program developed by Junwoo HWANG\n");
	Ask_WhereToGo();
}