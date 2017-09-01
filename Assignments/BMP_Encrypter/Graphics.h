#pragma once
#include<string.h>
#include<conio.h>
//#include<stdio.h>
char* Address = "..\\DATA\\NO_OPEN_storage\\Graphic.txt";
FILE* GraphicStream = fopen(Address, "rt");
char BUFFER[256];

enum GRAPHIC_LIST {
	Main_Title = 0,
	Help_Page,
};

void graphic_print(GRAPHIC_LIST a) {
	switch (a) {
	case Main_Title:
		fseek(GraphicStream, 0, SEEK_SET);
		while (strcmp("<Main_Title>\n", fgets(BUFFER, sizeof(BUFFER), GraphicStream)) && !feof(GraphicStream));
		if (!feof(GraphicStream)) {
			while (strcmp("<END>\n", fgets(BUFFER, sizeof(BUFFER), GraphicStream)) && !feof(GraphicStream)) { fputs(BUFFER, stdout); }
		}
		else { puts(""); }
		break;
	case Help_Page:
		fseek(GraphicStream, 0, SEEK_SET);
		while (strcmp("<Help_Page>\n", fgets(BUFFER, sizeof(BUFFER), GraphicStream)) && !feof(GraphicStream));
		if (!feof(GraphicStream)) {
			while (strcmp("<END>\n", fgets(BUFFER, sizeof(BUFFER), GraphicStream)) && !feof(GraphicStream)) { fputs(BUFFER, stdout); }
		}
		else { puts(""); }
		break;
	}
}

void graphic_end() { fclose(GraphicStream); }