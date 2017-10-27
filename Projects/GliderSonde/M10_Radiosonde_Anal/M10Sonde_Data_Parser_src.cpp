#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#pragma warning(disable:4996)
typedef struct {
	double Altitude;//[m]
	double Latitude;//[deg]
	double Longitude;//[deg]
	double WindF;//[m/s]
	double T;//[deg C]
	double Dir;//[deg]
	double RH;//[%]
	double Distance;
	int time;
}M10RAW;
typedef struct {
	char* FileName;//Name of the file that this 'data' is in.
	int DataCount = 0;
	M10RAW* Data[20000];//20K to be safe(max 20000sec = about 5H 33M 20S)
}M10OneFlight;

M10OneFlight* getData(char* Address) {
	FILE* fp = fopen(Address, "rt");
	if (fp == NULL)
		return NULL;
	M10OneFlight* OneFlight = (M10OneFlight*)malloc(sizeof(M10OneFlight));

	OneFlight->FileName = (char*)malloc(strlen(Address) + 1);
	strcpy(OneFlight->FileName, Address);//coppy file name to 'FileName'
	OneFlight->DataCount = 0;
	////////////////////////////////////////////////////////////////////
	char Buffer[100];
	fgets(Buffer, sizeof(Buffer), fp);//Read the First 'instruction' line.
	/////////////////////////////////////////////////
	while (!feof(fp)) {//Really 'reading in' datas Untill Reaches 'EOF'
		OneFlight->Data[OneFlight->DataCount] = (M10RAW*)malloc(sizeof(M10RAW));
		double* A = &(OneFlight->Data[OneFlight->DataCount]->Altitude);//Total 8 double pointers will succeed.
		int* B = &(OneFlight->Data[OneFlight->DataCount]->time);
		fscanf(fp,"%lf %lf %lf %lf %lf %lf %lf %lf %d\n",A,A+1,A+2,A+3,A+4,A+5,A+6,A+7,B);//erase '\n' Token
		/*
		for (int i = 0; i < 8; i++)
			printf("!%f!", A[i]);
		printf("%d\n", *B);*/
		//fgets(Buffer, sizeof(Buffer), fp);
		//printf("!%s!\n", Buffer);
		OneFlight->DataCount++;
	}
	return OneFlight;
}

bool Export_M10OneFlight_csv(char* address,M10OneFlight* OF) {
	FILE* fp = fopen(address, "wt");
	if (fp == NULL)
		return false;
	fprintf(fp,"Source Address,,%s\n", OF->FileName);
	fprintf(fp, "Total DATA count,,%d\n", OF->DataCount);
	fprintf(fp,"Time,Altitude[m],Latitude[¡Æ],Longitude[¡Æ],Windf[m/s],Temp[¡ÆC],Dir[¡Æ],RH[%%]\n");
	for (int i = 0; i < OF->DataCount; i++) {
		double* A = &(OF->Data[i]->Altitude);//Total 8 double pointers will succeed.
		int* B = &(OF->Data[i]->time);
		fprintf(fp, "%d", *B);
		for (int j = 0; j < 7; j++)
			fprintf(fp,",%f", A[j]);
		fputc('\n',fp);
	}
}
void Free_M10OneFlight(M10OneFlight* OFfree) {
	for (int i = 0; i < OFfree->DataCount; i++)
		free(OFfree->Data[i]);
	free(OFfree);
}
const char* TXT = ".txt";
const char* CSV = ".csv";
int main() {
	char SourceHeader[] = "C:\\noUNICODE\\170402\\M10SondeDATA\\";
	char DumpHeader[] = "C:\\noUNICODE\\170402\\M10SondeExport\\";
	char* NewSourceName=NULL;
	char* NewDumpName = NULL;
	M10OneFlight* DATA;
	while (1) {
		char tempName[30];
		
		printf("File Name? :");
		gets_s(tempName,sizeof(tempName));
		NewSourceName = (char*)malloc(strlen(SourceHeader) + strlen(tempName)+strlen(TXT) + 1);//Allocate Memory
		strcpy(NewSourceName, SourceHeader);
		strcat(NewSourceName, tempName);
		strcat(NewSourceName, TXT);
		printf("Reading File Named : \"%s\" \n", NewSourceName);

		DATA = getData(NewSourceName);
		
		if (DATA != NULL) {
			printf("DATA importing Successful!\n");
			printf("Total DATA count : %d\n", DATA->DataCount);
			NewDumpName = (char*)malloc(strlen(DumpHeader) + strlen(tempName) + strlen(CSV) + 1);
			strcpy(NewDumpName, DumpHeader);
			strcat(NewDumpName, tempName);
			strcat(NewDumpName, CSV);
			printf("Exporting to : \"%s\" \n", NewDumpName);
			if (Export_M10OneFlight_csv(NewDumpName, DATA))
				printf("File Exporting Successful!\n");
			else
				printf("File Exporting Unsuccessful! :(\n");
		}
		else
			printf("File Reading Failed...\n");

		Free_M10OneFlight(DATA);
		free(NewSourceName);
		free(NewDumpName);
	}
}