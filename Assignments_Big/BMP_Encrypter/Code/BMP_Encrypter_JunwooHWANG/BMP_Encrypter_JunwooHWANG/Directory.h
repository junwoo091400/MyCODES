#pragma once
#include<string.h>
#include<stdlib.h>

char* Make_new_Strcpy(char* str) {
	char* newstr = (char*)malloc(strlen(str) + 1);
	strcpy(newstr, str);
	return newstr;
}

char* Make_new_Strcat(char* str1, char* str2) {
	char* newstr = (char*)malloc(strlen(str1) + strlen(str2) + 1);
	strcpy(newstr, str1);
	strcat(newstr, str2);
	return newstr;
}

bool isFileName(char* str) {
	int Wcount = 0;
	if (strlen(str) == 0) return false;
	if (str[strlen(str) - 1] == '\\') return false;
	for (int i = 0; i < strlen(str); i++) if (str[i] == '\\') { return false; }
	return true;
}

char* FileNamefromDir(char* str) {//new string will be allocated.
	int Wcount = 0;
	int LastPosition = 0;
	////////////////////////////////////////////////////////////////////
	if (strlen(str) == 0) return NULL;//this is not a string
	if (str[strlen(str) - 1] == '\\') return NULL;//this is directory
												  ////////////////////////////////////////////////////////////////////
	for (int i = 0; i < strlen(str); i++) if (str[i] == '\\') { LastPosition = i; Wcount++; }
	if (Wcount == 0) return NULL;
	char* newStr = (char*)malloc(strlen(str) - LastPosition);
	for (int i = LastPosition + 1, j = 0; i <= strlen(str); i++, j++) newStr[j] = str[i];
	return newStr;
}

bool File_Exists(char* dir) {
	FILE* fp = fopen(dir, "r");
	if (fp) { fclose(fp); return true; }
	else return false;
}

bool CopyFile_func(char* destDir, char* fromDir) {

	printf("<<CopyFile_func()>>\nFROM: \"%s\" TO: \"%s\"\n", fromDir, destDir);
	char* fileName = FileNamefromDir(fromDir);
	if (fileName == NULL) return false;
	printf("Extracted fileName = %s\n", fileName);
	char* tempdestDir;

	if (destDir[strlen(destDir) - 1] != '\\') {//need to modify destDir
		char* destDirModified = Make_new_Strcat(destDir, "\\");
		tempdestDir = Make_new_Strcat(destDirModified, fileName);
		free(destDirModified);
	}
	else { tempdestDir = Make_new_Strcat(destDir, fileName); }//dest dir has '\\' at the End.

	if (!remove(tempdestDir)) { printf("Removed Original File: \"%s\"", tempdestDir); }//JUST IN CASE same file exists. DELETE IT!!!!!!

	FILE* TO = fopen(tempdestDir, "wb");
	FILE* FROM = fopen(fromDir, "rb");
	if (TO == NULL || FROM == NULL) { fclose(TO); fclose(FROM); return false; }
	char Buffer[8192];
	size_t readSize;
	while (readSize = fread(Buffer, 1, sizeof(Buffer), FROM)) { fwrite(Buffer, 1, readSize, TO); }
	fclose(TO); fclose(FROM);
	return true;
}