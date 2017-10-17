#pragma once
#pragma warning(disable:4996)
#include"Directory.h"
enum BMP_ERROR_e {
	BMP_ERROR_NONE,
	BMP_HEADER_MISSING,
	BMP_COLORPLANE_NOT_1,
	BMP_BackgroundStream_notSet,
	BMP_Stream_notSet,
	BMP_Folder_notSet,
	BMP_Background_too_Small,
	BMP_SecretHeader_Missing,
	BMP_EncryptFile_NametooLong,
	BMP_EncryptFile_SizetooBig,
	BMP_EncryptMethood_notPresent,
	BMP_Compression_Ratio_Over8,
	BMP_Encryption_BitAllocate_Sum_not_8,
};
enum BMP_Encrypt_Method {
	Simple_LSB_modify = 0x00
};
//
typedef struct {
	unsigned int SizeofBMPFile = 0;
	unsigned int Reserved4Byte = 0;
	unsigned int PixelOffsetAddr = 0;//where 'pixel data' starts
									 //------------------------------------------------------------
	unsigned int SizeofDIBHeader = 0;
	unsigned int Width = 0;
	int Height = 0;//CAN be '-1'
	unsigned short ColorPlaneNum = 1;//must be 0
	unsigned short BitperPixel = 0;
}BMP_MAIN_HEADER;

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

class BMP {

public:
	bool auto_OutputNameSelect = true;
	void set_autoNameselect(bool a) { auto_OutputNameSelect = a; }
	//------------------------------------------------------------------------------------------------------------------------//
	//	Folder/File Name setting functions.
	void set_Folder_Name(int i, char* s) {
		puts("<<set_Folder_Name()>>");
		if (BMP_Folder_Name[i] != NULL) { free(BMP_Folder_Name[i]); }

		if (s[strlen(s) - 1] != '\\') { BMP_Folder_Name[i] = Make_new_Strcat(s, "\\"); }

		else { BMP_Folder_Name[i] = (char*)malloc(strlen(s) + 1); strcpy(BMP_Folder_Name[i], s); };

		printf("BMP_Folder_Name[%d] set to \"%s\"\n", i, BMP_Folder_Name[i]);
		puts("");
	}

	bool set_File_Name(int i, char* s) {

		puts("<<set_File_Name()>>");

		if (i < 0 || i>4) { puts("Wrong index given"); return false; }

		if (BMP_File_Name[i] != NULL) { free(BMP_File_Name[i]); }

		if (!isFileName(s)) {//Not a file name

			printf("OOPS, \"%s\" is not a file name...\n", s);

			BMP_File_Name[i] = FileNamefromDir(s);
			if (BMP_File_Name[i] == NULL) { puts("No File Name detected[maybe it is Directory OR Not a String]...\n"); return false; }

			puts("But That's OK, It will be copied into Correct Folder of this Program!\n");

			removeFile(BMP_Folder_Name[i], BMP_File_Name[i]);//JUST IN CASE.

			if (!CopyFile_func(BMP_Folder_Name[i], s)) { puts("File Copying Failed..."); return false; }//Copy Binary file into the folder SAFELY.
			puts("Successfully copied the file into System's own folder!!");
		}
		else { BMP_File_Name[i] = Make_new_Strcpy(s); }//Is a file name.

		puts("");
		///////////////////////////////////////////////////////////////
		if (i == 0) { fputs("Background", stdout); }
		else if (i == 1) { fputs("EncryptSource", stdout); }
		else if (i == 2) { fputs("Encrypt", stdout); }
		else if (i == 3) { fputs("DecryptSource", stdout); }
		else if (i == 4) { fputs("Decrypt", stdout); }
		printf(" File Name set to \"%s\"\n", BMP_File_Name[i]);//printf("BMP_File_Name[%d] set to \"%s\"\n", i, BMP_File_Name[i]);
															   ///////////////////////////////////////////////////////////////

		if (BMP_Folder_Name[i] == NULL) { puts("!ERROR! : Folder Name NOT SET!"); BMP_Stream_set[i] = false; return false; }

		switch (i) {		//Attempt Making STREAM.
		case 0://Background
		case 1://Encrypt
		case 3:
			BMP_Stream[i] = getStream(BMP_Folder_Name[i], BMP_File_Name[i], "rb");
			if (BMP_Stream[i] != NULL) BMP_Stream_set[i] = true;
			else BMP_Stream_set[i] = false;
			break;

		case 2://Output
			if (!auto_OutputNameSelect) {
				removeFile(BMP_Folder_Name[i], BMP_File_Name[i]);//JUST IN CASE.
				BMP_Stream[i] = getStream(BMP_Folder_Name[i], BMP_File_Name[i], "wb");
				if (BMP_Stream[i] != NULL) BMP_Stream_set[i] = true;
				else BMP_Stream_set[i] = false;
			}
			break;

		case 4://Decrypt
			removeFile(BMP_Folder_Name[i], BMP_File_Name[i]);//JUST IN CASE.
			BMP_Stream[i] = getStream(BMP_Folder_Name[i], BMP_File_Name[i], "wb");
			if (BMP_Stream[i] != NULL) BMP_Stream_set[i] = true;
			else BMP_Stream_set[i] = false;
			break;
		}//end of Attempt Making STREAM.
		puts("");
		printf("Attempt Stream to: %s%s\n", BMP_Folder_Name[i], BMP_File_Name[i]);
		printf("BMP_Stream_set[%d]=%s\n", i, BMP_Stream_set[i] ? "True" : "False");
		puts("");
		return true;
	}
	///////////

	//////////////////////////////////////////////////////////////////
	BMP_ERROR_e Readin_BMP_HEADER(int i) {
		puts("<<Readin_BMP_HEADER()>>");
		if (BMP_Stream_set[i]) {
			fseek(BMP_Stream[i], 0, SEEK_SET);//go to 'first'

											  //0.Check HEADER
			if (fgetc(BMP_Stream[i]) != 'B' || fgetc(BMP_Stream[i]) != 'M') {
				printf("Header 'BM' not detected!\n");
				return BMP_HEADER_MISSING;
			}
			puts("0: HEADER \"BM\" valid...");

			//1.Read In HEADER_ALL!
			for (int j = 0; j < sizeof(BMP_MAIN_HEADER); j++)//don't use 'i' because that is Stream No.
				((unsigned char*)&Main_H)[j] = fgetc(BMP_Stream[i]);//get Data Least Endian.
			puts("1: Reading in Entire Header...");

			//2.Check Color Plane ERROR(must be 1)
			if (Main_H.ColorPlaneNum != 1)
				return BMP_COLORPLANE_NOT_1;
			puts("2: ColorPlaneNumber valid...");

			return BMP_ERROR_NONE;
		}
		else { puts("BMP_BackgroundStream_notSet!"); return BMP_BackgroundStream_notSet; }
	}

	BMP_ERROR_e Encrypt_BMP(BMP_Encrypt_Method b) {

		puts("<<Encrypt_BMP()>>");

		//if (!BMP_Stream_set[0] || !BMP_Stream_set[1]) return BMP_Stream_notSet;
		//if (!auto_OutputNameSelect && !BMP_Stream_set[2]) return BMP_Stream_notSet;

		if (auto_OutputNameSelect) {
			puts("auto_OutputNameSelect mode...");
			switch (b) {
			case Simple_LSB_modify:
				BMP_File_Name[2] = (char*)malloc(strlen(BMP_File_Name[0]) + strlen(autoExtension_BASE)
					+ strlen(autoExtension_Simple_LSB_modify) + 1);
				strcpy(BMP_File_Name[2], autoExtension_BASE);
				strcat(BMP_File_Name[2], autoExtension_Simple_LSB_modify);
				strcat(BMP_File_Name[2], BMP_File_Name[0]);//Extension name should be at the last!
				puts("Encrypt_Method = 'Simple_LSB_modify'");
				break;
			}
			if (BMP_Folder_Name[2] == NULL) BMP_Folder_Name[2] = BMP_Folder_Name[0];//if folder not set, set it same as background folder.
			removeFile(BMP_Folder_Name[2], BMP_File_Name[2]);//JUST IN CASE.//YEAP. JUST IN CASE

			BMP_Stream[2] = getStream(BMP_Folder_Name[2], BMP_File_Name[2], "wb");

			if (BMP_Stream[2] == NULL) { puts("What the... I can't create stream for Encrypt Folder man... Why???"); return BMP_Stream_notSet; }
			else BMP_Stream_set[2] = true;
		}

		puts("-------------------------------------------------");
		puts("GO HERE TO RETRIEVE DATA!!");
		printf("EncryptOutputFolder = \"%s\"\n", BMP_Folder_Name[2]);
		printf("EncryptOutputeName = \"%s\"\n", BMP_File_Name[2]);
		puts("---------------------------------------------------");

		//	0) Are unchecked "Background & EncryptSource" STREAMs SET?
		if (!BMP_Stream_set[0] || !BMP_Stream_set[1]) {
			puts("!ERROR! : BMP_Stream_notSet");
			puts("");
			for (int i = 0; i < 3; i++) { printf("BMP_Stream_set[%d]=%s\n", i, BMP_Stream_set[i] ? "true" : "false"); }
			close_all_Stream();
			for (int i = 0; i < 3; i++) { removeBMPFile_short(i); }//CLOSE ALL FILES... for next time
			return BMP_Stream_notSet;
		}
		puts("All Streams are set(Background, Encrypt, Output)");
		puts("");

		//	1) Readin BMP Header
		if (Readin_BMP_HEADER(0) != BMP_ERROR_NONE) {//from background[0]
			close_all_Stream();
			for (int i = 0; i < 3; i++) { removeBMPFile_short(i); }//CLOSE ALL FILES... for next time
			return Readin_BMP_HEADER(0);
		}
		puts("Reading BMP Header successful");
		puts("");

		//  2) Check if Encrypting is possible(size-wise)<Encrypt File should fit inside Background>
		switch (b) {
		case Simple_LSB_modify:

			unsigned int Pixel_Size = get_realPixelSize();
			unsigned int needed_Size = (strlen(Encryption_secretHEADER) + 1 + 1 + Compression_Ratio)*Default_Compression_Ratio
										//"SecretHeader(no NULL) + Encrypt_Method + C_R + C_R_array
				+ (1 + 4 + strlen(BMP_File_Name[1]) + 1 ) * Compression_Ratio;
				//EncryptedFileNameSize + TotalSizeofEncryptedFile + RealFileName(with NULL)
			printf("This BACKGROUND can hold upto %u[BYTEs] of encrypt DATA.\n", (Pixel_Size - needed_Size) / Compression_Ratio);
			puts("");

			needed_Size += getFileSize(BMP_Stream[1]) * Compression_Ratio;//size of file being encrypted 

			printf("\n[Byte] Have / Need = ( %u / %u ) ...\n\n", Pixel_Size, needed_Size);

			if (Pixel_Size < needed_Size)
			{
				puts("ERROR!! \"BMP_Background_too_Small\" !!");
				printf("Short by %u[BYTES] \n", needed_Size - Pixel_Size);
				puts("");
				puts("Removing File(just in case there might be the same one)...");
				puts("");
				close_all_Stream();
				for (int i = 0; i < 3; i++) { removeBMPFile_short(i); }//CLOSE ALL FILES... for next time
				return BMP_Background_too_Small;
			}

			puts("Encrypt File can fit inside Background");
			puts("");

			if (strlen(BMP_File_Name[1]) + 1 > 256) {
				puts("!ERROR! : Encrypt File Name exceeded 255bytes!");
				close_all_Stream();
				close_all_Stream();
				for (int i = 0; i < 3; i++) { removeBMPFile_short(i); }//CLOSE ALL FILES... for next time
				return BMP_EncryptFile_NametooLong;
			}

			/*if ((unsigned long)getFileSize(BMP_Stream[1]) > 0xFFFFFFUL) {
				close_all_Stream();
				close_all_Stream();
				for (int i = 0; i < 3; i++) { removeBMPFile_short(i); }//CLOSE ALL FILES... for next time
				puts("!ERROR! : Encrypt File Size Exceeded 4GigaBytes! [Are you Kidding Me?]");
				return BMP_EncryptFile_SizetooBig;
			}
			break;*/
		}

		//  3) Copy the Header & Stuff before pixel data exactly the same!
		fseek(BMP_Stream[0], 0, SEEK_SET);
		fseek(BMP_Stream[2], 0, SEEK_SET);
		for (unsigned int i = 0; i < Main_H.PixelOffsetAddr; i++) { fputc(fgetc(BMP_Stream[0]), BMP_Stream[2]); }

		//  4) Now Real Encrypting STARTs!!
		switch (b) {
		case Simple_LSB_modify:
			// HEADER print //

			// ** DEFAULT COMPRESSION PRINT//
			for (unsigned int i = 0; i<strlen(Encryption_secretHEADER); i++) insertByte(Encryption_secretHEADER[i], b,true);//-HEADER without null end.

			insertByte(b, b,true);//-Encrypt_Method(Default_Compression)

			insertByte(Compression_Ratio, b, true);
			for (unsigned char i = 0; i < Compression_Ratio; i++) insertByte(Encryption_BitAllocate[i], b, true);
			printf("Compression_Ratio = %d\n", (int)Compression_Ratio);
			for (unsigned char i = 0; i < Compression_Ratio; i++) printf("%d:", (int)Encryption_BitAllocate[i]);
			puts("");

			// ** USER DEFINED COMPRESSION PRINT//
			insertByte((unsigned char)((strlen(BMP_File_Name[1]) + 1) & 0xFF), b);//-File Name Size
			for (int i = 0; i < 4; i++) { insertByte((getFileSize(BMP_Stream[1]) >> (i * 8))&(0xFF), b); }//-File Size

			for (unsigned int i = 0; i <= strlen(BMP_File_Name[1]) && i <= 0xFF; i++)//don't let name exceed 0xFF in total....
				insertByte(BMP_File_Name[1][i], b);//-Original Encrypt File Name 'with' null end!

			// DATA print //
			fseek(BMP_Stream[1], 0, SEEK_SET);
			for (long i = 0; i<getFileSize(BMP_Stream[1]); i++) { insertByte(fgetc(BMP_Stream[1]), b); }

			// Rest of the BACKGROUND print //
			size_t readSize;
			char Buffer[8192];
			while (readSize = fread(Buffer, 1, sizeof(Buffer), BMP_Stream[0])) { fwrite(Buffer, 1, readSize, BMP_Stream[2]); }

			// Close the BACKGROUND & ENCRYPTSOURCE & OUTPUT file //
			close_all_Stream();
			// Delete the BACKGROUND & EnncryptSource File //
			for (int i = 0; i < 2; i++) { removeBMPFile_short(i); }//CLOSE ALL FILES... for next time
			break;
		}

		puts("Encryption Done");
		puts("");
		return BMP_ERROR_NONE;
	}

	BMP_ERROR_e Decrypt_BMP() {
		puts("<<Decrypt_BMP()>>");

		//0) Check if Decrypt_Source Stream is SET
		if (!BMP_Stream_set[3]) { puts("[ERROR] : Decrypt Source Stream not set!"); return BMP_Stream_notSet; }

		printf("+Decrypting+\nFolder : %s\nFile : %s\n", BMP_Folder_Name[3], BMP_File_Name[3]);
		puts("");

		//1) Check if Decrypt_Source has valid HEADER.
		if (Readin_BMP_HEADER(3) != BMP_ERROR_NONE) {
			puts("[ERROR] : unable to read-in Decrypt_Source BMP-Header!");
			close_all_Stream();
			removeBMPFile_short(3);
			return Readin_BMP_HEADER(3);
		}

		unsigned char BUFFER;

		fseek(BMP_Stream[3], Main_H.PixelOffsetAddr, SEEK_SET);//Goto where These Encrypted DATAs are present!

	    //2) Readin SECRET Header to valid that THIS PROGRAM encrypted it!!
		fputs("Reading-in Secret Header BYTE |", stdout);
		for (int i = 0; i < strlen(Encryption_secretHEADER); i++) {
			decryptByte(&BUFFER, Simple_LSB_modify,true);//Default Compression Reading
			if (BUFFER != Encryption_secretHEADER[i]) { puts(""); printf("%x\n", BUFFER); close_all_Stream(); removeBMPFile_short(3); return BMP_SecretHeader_Missing; }
			printf("%d|", i);
		}
		puts("... Done");
		puts("Reading in Secret Header Successful.");
		puts("");

		BMP_Encrypt_Method Decrypting_Method;

		//3) Readin 'Encrypt_Method', to know exactly how to decrypt this file.
		decryptByte(&BUFFER, Simple_LSB_modify,true);//Default Compression Reading
		switch (BUFFER) {

		case Simple_LSB_modify:
			Decrypting_Method = Simple_LSB_modify;
			break;

		default:
			puts("!ERROR! : Decrypt_Source file doesn't have VALID Encrypt_Method flag!");
			close_all_Stream();
			removeBMPFile_short(3);
			return BMP_EncryptMethood_notPresent;
			//Decrypting_Method = Simple_LSB_modify;//Default decrypting Method is "Simple_LSB"
		}
		printf("Decrypting... Decrypting_Method : %x\n", Decrypting_Method);
		
		//3.5) Readin 'Compression_Ratio' and 'Encryption_BitAllocate' Array.
		decryptByte(&Compression_Ratio, Decrypting_Method,true);
		for (int i = 0; i < Compression_Ratio; i++) decryptByte(Encryption_BitAllocate + i, Decrypting_Method, true);//Default Comp. read.
		printf("Decrypting... Compression_Ratio = %d\nEncryption_BitAllocate = {", (int)Compression_Ratio);
		for (int i = 0; i < Compression_Ratio; i++) printf("%d,",(int)Encryption_BitAllocate[i]);
		puts("}");

		if (Compression_Ratio > 8) { puts("!ERROR! : Compression Ratio exceeded 8...");
		close_all_Stream();removeBMPFile_short(3); return BMP_Compression_Ratio_Over8; }

		int Sum = 0;
		for (int i = 0; i < Compression_Ratio; i++) Sum += Encryption_BitAllocate[i];
		if (Sum != 8) { puts("!ERROR! : BitAllocate Array SUM not 8!!");
		close_all_Stream();removeBMPFile_short(3); return BMP_Encryption_BitAllocate_Sum_not_8;}

		//4) Readin 'File_NameSize' to know how long(including NULL) the "Encrypted File Name" will be.
		unsigned char Salvaging_File_NameSize;
		decryptByte(&Salvaging_File_NameSize, Decrypting_Method);
		printf("Salvaging_File_NameSize = %d\n", (int)BUFFER);

		//5) Readin 'File_Size' to know how long the File encrypted will be.
		unsigned int Salvaging_File_Size = 0;
		for (int i = 0; i < 4; i++) decryptByte(((unsigned char*)&Salvaging_File_Size)+i, Decrypting_Method);//make sure (8*i)!
		printf("Salvaging_File_Size = %u\n", Salvaging_File_Size);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Assigns Memory for 'Decrypt' File Name BELOW!!!

		//6) Assign Memory for 'Decrypt' File Name.
		char* DecryptingFileName = (char*)malloc(Salvaging_File_NameSize);

		//7) Readin Name of Encrypted(DecryptSource) File & Create Stream for Decrypt File
		puts("Reading in DecryptingFileName...");
		puts("");
		for (int i = 0; i < Salvaging_File_NameSize; i++) decryptByte((unsigned char*)DecryptingFileName+i, Decrypting_Method);
		set_File_Name(4, DecryptingFileName);//automatically create Stream TOO!

		//8) Check Stream validity
		if (!BMP_Stream_set[3] || !BMP_Stream_set[4]) {
			puts("!ERROR! : Stream for Decrypt source/file is not set!");
			close_all_Stream(); for (int i = 3; i <= 4; i++)removeBMPFile_short(i);
			return BMP_Stream_notSet;
		}
		puts("Stream for Decrypt_Source and Decrypt File is BOTH SET!(correct)");
		puts("-------------------------------------------------");
		puts("GO HERE TO RETRIEVE DATA!!");
		printf("DecryptOutputFolder = \"%s\"\n", BMP_Folder_Name[4]);
		printf("DecryptOutputeName = \"%s\"\n", BMP_File_Name[4]);
		puts("---------------------------------------------------");
		//8) Readin File
		for (unsigned int i = 0; i < Salvaging_File_Size; i++) {
			decryptByte(&BUFFER, Decrypting_Method);
			fputc(BUFFER, BMP_Stream[4]);
		}

		//9) Close all Stream
		close_all_Stream();
		removeBMPFile_short(3);//Remove DecryptSourceFile for next run...

		return BMP_ERROR_NONE;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	unsigned int getBMPEncryptablility(int i) {
		if (i != 0 && i != 3) return 0;//For Background&DecryptSource Files...
		if (!BMP_Stream_set[i]) return 0;//if Not Set.. ONLY WORKS when STREAM IS SET!!!
		if(Readin_BMP_HEADER(i)!=BMP_ERROR_NONE) return 0;//necessary before reading in data...
		unsigned int Pixel_Size = get_realPixelSize();
		char* TestString = "There_is_no_Spoon_Neo.txt";
		unsigned int needed_Size = (strlen(Encryption_secretHEADER) + 1 + 1 + Compression_Ratio)*Default_Compression_Ratio
			//"SecretHeader(no NULL) + Encrypt_Method + C_R + C_R_array
			+ (1 + 4 + strlen(TestString) + 1) * Compression_Ratio;
		//EncryptedFileNameSize + TotalSizeofEncryptedFile + RealFileName(with NULL)
		Pixel_Size -= needed_Size;
		Pixel_Size /= Compression_Ratio;
		//puts("");
		//puts("<<getBMPEncryptability()>>");
		//printf("This BMP can hold upto %u[BYTE] of encrypt DATA. w/ Compression_Ratio = %d!\n", Pixel_Size,(int)Compression_Ratio);
		return Pixel_Size;
	}

	bool decryptByte(unsigned char* C, BMP_Encrypt_Method a,bool Default_Compress=false) {//readin byte accordingly...
		if (a == Simple_LSB_modify) {
			int TEMP;
			unsigned char Stack = 0;//Max 7
			*C = 0x00;
			if (Default_Compress) {
				for (unsigned char i = 0; i < Default_Compression_Ratio; i++) { 
					*C |= ( (TEMP = fgetc(BMP_Stream[3]))&( (1 << Default_Encryption_BitAllocate[i]) - 1) )<< Stack;//from bit0 ~ bit7
					Stack += Default_Encryption_BitAllocate[i];//default!
					if (TEMP == EOF) return false;
				}
				return true;
			}
			else {//User defined Compression
				for (unsigned char i = 0; i < Compression_Ratio; i++) {
					*C |= ( ((TEMP = fgetc(BMP_Stream[3])) & ( (1 << Encryption_BitAllocate[i]) - 1)) << Stack );//from bit0 ~ bit7
					Stack += Encryption_BitAllocate[i];
					if (TEMP == EOF) return false;
				}
				return true;
			}
		}
	}

	void insertByte(unsigned char bt, BMP_Encrypt_Method a, bool Default_Compress = false) {//insert Byte distributed.
		if (a == Simple_LSB_modify) {//1 byte => into 8 byte.
			if (Default_Compress) {
				for (unsigned char i = 0; i < Default_Compression_Ratio; i++) {
					unsigned char byte_2_Encrypt = fgetc(BMP_Stream[0]);//Should be from 'Background(0)'
					byte_2_Encrypt &= ~((1 << Default_Encryption_BitAllocate[i]) - 1);
					byte_2_Encrypt |= bt&((1 << Default_Encryption_BitAllocate[i]) - 1);
					bt >>= Default_Encryption_BitAllocate[i];
					fputc(byte_2_Encrypt, BMP_Stream[2]);
				}
			}
			else {//User-defined Compression
				for (unsigned char i = 0; i < Compression_Ratio; i++) {
					unsigned char byte_2_Encrypt = fgetc(BMP_Stream[0]);//Should be from 'Background(0)'
					byte_2_Encrypt &= ~((1 << Encryption_BitAllocate[i]) - 1);
					byte_2_Encrypt |= bt&((1 << Encryption_BitAllocate[i]) - 1);
					bt >>= Encryption_BitAllocate[i];
					fputc(byte_2_Encrypt, BMP_Stream[2]);
				}
			}
		}
	}

	long getFileSize(FILE* fp) {
		long original = ftell(fp);
		fseek(fp, 0L, SEEK_END);
		long len = ftell(fp);
		fseek(fp, original, SEEK_SET);//move to original
		return len;
	}

	void Debug_printHeader() {
		puts("--------------------------------");
		puts("<<Debug_printHeader()>>");
		printf("SizeofBMPFile = %u | %x\n", Main_H.SizeofBMPFile, Main_H.SizeofBMPFile);
		printf("Reserved4Byte = %u | %x\n", Main_H.Reserved4Byte, Main_H.Reserved4Byte);
		printf("PixelOffsetAddr = %u | %x\n", Main_H.PixelOffsetAddr, Main_H.PixelOffsetAddr);//where 'pixel data' starts
																							  //------------------------------------------------------------
		printf("SizeofDIBHeader = %u | %x\n", Main_H.SizeofDIBHeader, Main_H.SizeofDIBHeader);
		printf("Width = %u | %x\n", Main_H.Width, Main_H.Width);
		printf("Height = %u | %x\n", Main_H.Height, Main_H.Height);//CAN be '-1'
		printf("ColorPlaneNum = %u | %x (must be 1)\n", Main_H.ColorPlaneNum, Main_H.ColorPlaneNum);//must be 0
		printf("BitperPixel = %u | %x\n", Main_H.BitperPixel, Main_H.BitperPixel);
		puts("--------------------------------");
	}

	void Debug_printFileSize() {
		if (BMP_Stream_set[0]) { printf("BACKGROUND_Stream_Size = %ld\n", getFileSize(BMP_Stream[0])); }
		if (BMP_Stream_set[1]) { printf("ENCRYPT_Stream_Size = %ld\n", getFileSize(BMP_Stream[1])); }
		if (BMP_Stream_set[2]) { printf("DECRYPT_Stream_Size = %ld\n", getFileSize(BMP_Stream[2])); }
	}

	void Debug_print_PixelOffsetAddr() {
		puts("<<Debug_print_PixelOffsetAddr()>>\n");
		printf("%u[DEC] | %x[HEX]\n\n", Main_H.PixelOffsetAddr, Main_H.PixelOffsetAddr);
	}

	unsigned int get_realRowSize() {
		unsigned short _BitperPixel = Main_H.BitperPixel;
		unsigned int _ImageWidth = Main_H.Width;
		unsigned int ROWSIZE = ((unsigned int)_BitperPixel*_ImageWidth + 31) / 32;
		ROWSIZE *= 4; // 4 byte padding
		printf("get_realRowSize() = %u\n", ROWSIZE);
		return ROWSIZE;
	}

	unsigned int get_realPixelSize() {
		unsigned int PixelSIZE = get_realRowSize()*(Main_H.Height>0 ? Main_H.Height : -Main_H.Height);
		printf("get_realPixelSize() = %u\n", PixelSIZE);
		return PixelSIZE;
	}//Main_H.Height can be -1. So abs calculation needed.

	void close_all_Stream() {
		puts("Closing All Streams..\n");
		for (short i = 0; i < 5; i++) if (BMP_Stream_set[i]) { fclose(BMP_Stream[i]); BMP_Stream_set[i] = false; }
	}

	char* getFileName(int i) { return BMP_File_Name[i]; }
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
private:
	BMP_MAIN_HEADER Main_H;

	char* BMP_Folder_Name[5] = { NULL ,NULL,NULL,NULL,NULL };//Background, EncryptSource, Encrypt, DecryptSource, Decrypt
	char* BMP_File_Name[5] = { NULL ,NULL,NULL,NULL,NULL };
	FILE* BMP_Stream[5] = { NULL ,NULL,NULL,NULL,NULL };
	bool BMP_Stream_set[5] = { false,false,false,false,false };
	const char* autoExtension_BASE = "Encrypted_";
	const char* autoExtension_Simple_LSB_modify = "SimLSB_";

	const char* Encryption_secretHEADER = "JUNWOOkingdom";

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
	const static unsigned char Default_Compression_Ratio = 8;
	const unsigned char Default_Encryption_BitAllocate[Default_Compression_Ratio] = { 1,1,1,1,1,1,1,1 };
	unsigned char Compression_Ratio = 5;
	unsigned char Encryption_BitAllocate[8] = {2,1,2,1,2};
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

	/////////////////////////////////////////////////////////
	FILE* getStream(char* folder, char* file, const char* Mode) {
		char* Temp_C = (char*)malloc(strlen(folder) + strlen(file) + 1);
		strcpy(Temp_C, folder);
		strcat(Temp_C, file);
		FILE* temp_F = fopen(Temp_C, Mode);
		free(Temp_C);
		return temp_F;
	}
	int removeFile(char* folder, char* file) {
		char* Temp_C = Make_new_Strcat(folder, file);
		int result = remove(Temp_C);
		if (!result) { printf("\n[removeFile] : \"%s\"\n\n", Temp_C); }//result should be 0 if success.
		free(Temp_C);
		return result;
	}
	int removeBMPFile_short(int i) { return removeFile(BMP_Folder_Name[i], BMP_File_Name[i]); }
	bool fileExists(char* folder, char* file) {//Check if file exists as 'folder's 'file'
		char* Temp_C = Make_new_Strcat(folder, file);
		if (File_Exists(Temp_C)) { free(Temp_C); return true; }
		else { free(Temp_C); return false; }
	}
	bool isSameFile(char*folder, char*file, char*compfile) {//Check if strcat of folder&file == compfile
		char* newC = Make_new_Strcat(folder, file);
		if (!strcmp(newC, compfile)) { free(newC); return true; }
		else { free(newC); return false; }
	}
	///////////////////////////////////////////////////////////

};