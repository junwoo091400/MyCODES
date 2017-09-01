#pragma once

const uint8_t chipSelect = SS;
SdFat sd;
SdFile file;

#define FILE_BASE_NAME "Data"

boolean Sd_init() {

	const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
	char fileName[13] = FILE_BASE_NAME "00.txt";

	if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) return false;

	while (sd.exists(fileName)) {
		if (fileName[BASE_NAME_SIZE + 1] != '9') {
			fileName[BASE_NAME_SIZE + 1]++;
		}
		else if (fileName[BASE_NAME_SIZE] != '9') {
			fileName[BASE_NAME_SIZE + 1] = '0';
			fileName[BASE_NAME_SIZE]++;
		}
		else { return false; }
	}
	if (!file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) return false;
	return true;//All Set
}

void Sd_close() { file.close(); }

/////////////////////////////////////
void filewrite16(int16_t data) { file.write(data & 0xFF); file.write(data >> 8 & 0xFF); }
void filewrite32(int32_t data) { filewrite16(data & 0xFFFF); filewrite16(data >> 16 & 0xFFFF); }
//////////////////////////////////////////////////////////////////////////////////////////

void ExportHeader() {
  file.write('\r');
  file.write('\n');
	//file.write('@');
	filewrite32(loop_timer);
	file.write(Max_Altitude_Reached ? 'v' : '^');
}

void ExportGPS() {
	if (GPS_Valid) {
		file.write('G');
		file.write(MspmsgIn.msploc.numSat);
		filewrite32(MspmsgIn.msploc.lat);
		filewrite32(MspmsgIn.msploc.lon);
		filewrite16(MspmsgIn.msploc.alt);
		filewrite16(MspmsgIn.msploc.groundSpeed);
		filewrite16(MspmsgIn.msploc.groundCourse);
	}
	else { file.write('g'); }
}

void ExportAlt() {
	if (Altitude_Valid) {
		file.write('A');
		filewrite32(MspmsgIn.mspalt.alt);
		filewrite16(MspmsgIn.mspalt.vario);
	}
	else { file.write('a'); }
}

void ExportDHT(){
  if(DHT_No1_Valid){
    file.write('Q');
    filewrite16(DHT_No1.getTemperatureCInt());
    filewrite16(DHT_No1.getHumidityInt());
  }
  else{file.write('q');}
  if(DHT_No2_Valid){
    file.write('W');
    filewrite16(DHT_No2.getTemperatureCInt());
    filewrite16(DHT_No2.getHumidityInt());
  }
  else{file.write('w');}
}

