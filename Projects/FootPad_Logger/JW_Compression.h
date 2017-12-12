/*

*/

#define COMPRESS_BASE_NUMBER 58 
#define COMPRESS_SYMBOL_START 'A' // This is interpreted as 0.

void CompressData(char* dst,unsigned int packet)
{

	if(dst == NULL)
		return;

	char FirstDigit = packet % COMPRESS_BASE_NUMBER;
	//packet /= COMPRESS_BASE_NUMBER;
	//char 2ndDigit = packet % COMPRESS_BASE_NUMBER;
	//packet /= COMPRESS_BASE_NUMBER;
	//char 3rdDigit = packet % COMPRESS_BASE_NUMBER;
	//*dst++ = (3rdDigit+COMPRESS_SYMBOL_START);
	//*dst++ = (2ndDigit+COMPRESS_SYMBOL_START);
	*dst++ = (FirstDigit+COMPRESS_SYMBOL_START); // completed.
}

uint8_t deCompressData(char dst){
  uint8_t tempData = (dst) - COMPRESS_SYMBOL_START;
  return tempData;
}

