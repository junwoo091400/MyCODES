#include<stdio.h>
#pragma warning(disable:4996)
#include<stdlib.h>
#include<string.h>

char* newStrcat(char* folder, char*file) {
	char* newstr = (char*)malloc(strlen(folder) + strlen(file) + 1);
	strcpy(newstr, folder);
	strcat(newstr, file);
	return newstr;
}

unsigned char Buffer[4196];

int main() {
	char* FROM = "C:\\Users\\기본\\Desktop\\Dude\\";
	char* TO = "C:\\Users\\기본\\Desktop\\Dude\\";
	char FileName[100];

	while (true) {
		puts("Name?");
		scanf("%s", FileName);
		FILE* fpfrom = fopen(newStrcat(FROM, FileName), "rb");

		if (fpfrom == NULL)
		{
			puts("File doesn't exist!");
		}
		else
		{
			FILE* fpto = fopen(newStrcat(TO, FileName), "wb");

			if (fpto == NULL) { puts("File doesn't exist!"); }
			else {
				size_t readCnt;
				while (readCnt = fread(Buffer, 1, sizeof(Buffer), fpfrom)) {
					for (int i = 0; i < readCnt; i++) Buffer[i] = ~Buffer[i];//Just Invert!!
					fwrite(Buffer, 1, readCnt, fpto);
				}
				puts("DONE.");
				fclose(fpto);
			}
			fclose(fpfrom);
		}
	}
	return 0;
}