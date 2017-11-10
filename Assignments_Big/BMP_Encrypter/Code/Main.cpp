#pragma warning(disable:4996)
#include<stdio.h>
#include"BMP.h"
#include"Graphics.h"
#include"Directory.h"
#include<conio.h>
#include<windows.h>

///////////////////////////////////////////////////////////////////////////
const char* BackgroundFolder = "..\\DATA\\NO_OPEN_storage\\Background";//0

const char* Encrypt_Folder = "..\\DATA\\NO_OPEN_storage\\EncryptSource";//1

const char* DecryptSourceFolder = "..\\DATA\\NO_OPEN_storage\\DecryptSource";//3

																			 //-------------------------------------------------------------------------------//

const char* OutputFolder = "..\\DATA\\YES_OPEN_Output\\Encrypt";//2

const char* DecryptFolder = "..\\DATA\\YES_OPEN_Output\\Decrypt";//4

																 ////////////////////////////////////////////////////////////////////////
void clear_Stdin() { char c; while ((c = getchar()) != '\n' && c != EOF); }
void rstrip(char* str) { if (str[strlen(str) - 1] == '\n') str[strlen(str) - 1] = 0; }
void WaitUntilInput() { puts("Type something to Continue!"); char c = getchar(); if (c != '\n') clear_Stdin(); }
////////////////////////////////////////////////////////////////////////
int main() {
	//////////////////
	bool EugroBooting = true;
	bool EugroLoading = true;
	//////////////////
	bool Again = true;
	bool Encrypt_Page = false;
	bool Decrypt_Page = false;
	//////////////////
	bool GraphicOK = true;
	BMP newBMP = BMP();
	if (!GraphicStream) { puts("GRAPHIC STREAM OPENING ERROR!!!"); GraphicOK = false; }
	char Trashchar = 0x00;
	char TrashString[256] = {};
	//////////////////////////////////
	if (EugroBooting) {
		puts("+++++++++++++++++++++++++++");
		puts("PROGRAM BOOTING...");
		puts("+++++++++++++++++++++++++++"); Sleep(400);
		newBMP.set_Folder_Name(0, (char*)BackgroundFolder); Sleep(400);
		newBMP.set_Folder_Name(1, (char*)Encrypt_Folder); Sleep(400);
		newBMP.set_Folder_Name(2, (char*)OutputFolder); Sleep(400);
		newBMP.set_Folder_Name(3, (char*)DecryptSourceFolder); Sleep(400);
		newBMP.set_Folder_Name(4, (char*)DecryptFolder); Sleep(400);
		system("cls");
	}
	///////////////////////////////////
	graphic_print(Main_Title);
	if (EugroLoading) { puts("Loading"); for (int i = 0; i < 50; i++) { fputc('*', stdout); Sleep(40); } }
	puts("\npress Enter to Start...");

	Trashchar = getchar();
	if (Trashchar != '\n') clear_Stdin();//Flush only if enter was not read.
	system("cls");
	//////////////////////////////////////////
	while (Again) {
		system("cls");
		puts("<<MAIN MENU>>");
		puts("1.Encrypt");
		puts("2.Decrypt");
		puts("3.Help?");
		puts("4.Program Exit");
		Trashchar = getchar();
		if (Trashchar != '\n') clear_Stdin();
		system("cls");
		switch (Trashchar) {
		case '1':
			Encrypt_Page = true;
			break;
		case '2':
			Decrypt_Page = true;
			break;
		case '3':
			graphic_print(Help_Page);
			WaitUntilInput();
			break;
		case '4':
			Again = false;
			break;
		}
		while (Encrypt_Page) {
			system("cls");
			puts("<<Encrypt_Page>>");
			puts("Things to do:");
			puts("First, select Background(.bmp) File");
			puts("Second, select Encrypt File(File you want to encrypt into background)");
			puts("Third, select output File Name(automatic naming possible)");
			puts("Then... program should encrypt the file! GOT IT?? ['Y' to continue]");
			Trashchar = getchar();
			if (Trashchar != '\n') clear_Stdin();
			if (Trashchar != 'Y'&&Trashchar != 'y') { Encrypt_Page = false; break; }
			////////////////////////////////////////////////////
			system("cls");
			puts("Step 1: <<Background File Select Page>>");
			//puts("-Enter the name of file that will be the Background File.");
			puts("DRAG and DROP the photo(.bmp) onto the command prompt and Enter.");
			fputs("\n>>", stdout);
			fgets(TrashString, sizeof(TrashString), stdin);
			rstrip(TrashString);
			if (!newBMP.set_File_Name(0, TrashString)) { Encrypt_Page = false; WaitUntilInput(); break; };
			WaitUntilInput();
			///////////////////////////////////////////////////
			system("cls");
			puts("Step 2: <<EncryptSource File Select Page>>");
			//puts("-Enter the name of file that will be the Encrypted.");
			puts("-DRAG and DROP the file(anything) onto the command prompt and Enter.");
			printf("!!Fil Size should be less than approx. %u[Bytes]!\n", newBMP.getBMPEncryptablility(0));
			fputs("\n>>", stdout);
			fgets(TrashString, sizeof(TrashString), stdin);
			rstrip(TrashString);
			if (!newBMP.set_File_Name(1, TrashString)) { Encrypt_Page = false; WaitUntilInput(); break; };
			WaitUntilInput();
			////////////////////////////////////////////////////
			system("cls");
			puts("Step 3: <<Encrypt File Name Select>>");
			puts("Do you want auto_FilenameSelect?[Y to select]");
			Trashchar = getchar();
			if (Trashchar != '\n') clear_Stdin();

			if (Trashchar == 'Y' || Trashchar == 'y') { newBMP.set_autoNameselect(true); }
			else {
				newBMP.set_autoNameselect(false);
				puts("-Type the name of the Output file you wish.");
				fputs("\n>>", stdout);
				fgets(TrashString, sizeof(TrashString), stdin);
				rstrip(TrashString);
				if (!newBMP.set_File_Name(2, TrashString)) { Encrypt_Page = false; WaitUntilInput(); break; };
				WaitUntilInput();
			}
			////////////////////////////////////////////////////
			system("cls");
			puts("Step 4: <<Encrypting!!>>");
			newBMP.Encrypt_BMP(Simple_LSB_modify);
			WaitUntilInput();
			////////////////////////////////////////////////////	
		}
		while (Decrypt_Page) {
			system("cls");
			puts("<<Decrypt_Page>>");
			puts("Things to do:");
			puts("First, select DecryptSource(.bmp) File");
			puts("Then... program should Decrypt the file! GOT IT?? ['Y' to continue]");
			Trashchar = getchar();
			if (Trashchar != '\n') clear_Stdin();
			if (Trashchar != 'Y'&&Trashchar != 'y') { Decrypt_Page = false; break; }
			////////////////////////////////////////////////////
			system("cls");
			puts("Step 1: <<DecryptSource File Select Page>>");
			//puts("-Enter the name of file that will be Decrypted.");
			puts("-DRAG and DROP the photo(.bmp) onto the command prompt and Enter.");
			fputs("\n>>", stdout);
			fgets(TrashString, sizeof(TrashString), stdin);
			rstrip(TrashString);
			if (!newBMP.set_File_Name(3, TrashString)) { Encrypt_Page = false; WaitUntilInput(); break; };
			WaitUntilInput();
			///////////////////////////////////////////////////
			system("cls");
			puts("Step 2: <<Decrypting!!>>");
			newBMP.Decrypt_BMP();
			WaitUntilInput();
			////////////////////////////////////////////////////	
		}
		if (Again) { puts("Going back to the Main Menu..."); Sleep(500); }

	}//End of AGAIN
	newBMP.close_all_Stream();
	graphic_end();
	return 0;
}