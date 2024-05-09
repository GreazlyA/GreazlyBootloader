#include "crc32.h"


// G L O B A L S 
uint32_t  Crc32Value;

/****************************************************************************
 *** uuint32_t_t crc32ByteCalc(uint32_t crc32Base, uint8_t NewDataByte)
 * This function receives the current CRC32 value and new data byte and generates new 
 * CRC32 result
 *
 *
 *
 *****************************************************************************/
uint32_t crc32ByteCalc(uint32_t crc32Base, uint8_t NewDataByte)
{
	int i, j;
	uint32_t byte, crc, mask;
	int nBytes =1;
	i = 0;

	crc = crc32Base;
	for(i=0; i<nBytes; i++)
	{
		byte = NewDataByte;
		crc = crc ^ byte;
		for (j = 7; j >= 0; j--) {    // Do eight times.
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
	}
	return crc; // ~crc;
}


/****************************************************************************
 *** void crc32BuffCalc()
 *
 *
 *
 *
 *
 *****************************************************************************/
uint32_t crc32BuffCalc(uint8_t * Message, uint32_t Offset, uint32_t nBytes)
{
	int i, j;
	uint32_t byte, crc, mask;

	i = 0;
	crc = 0xFFFFFFFF;
	for(i=0; i<nBytes; i++)
	{
		byte = Message[i+Offset];     // Get next byte.
		crc = crc ^ byte;
		for (j = 7; j >= 0; j--) {    // Do eight times.
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
	}
	return  ~crc;
}



/****************************************************************************
 *** uint32_t crc32ByteInit()
 *
 *
 *
 *
 *
 *****************************************************************************/
uint32_t crc32Init()
{
	// Return the initial CRC32 value
	return(0xFFFFFFFF);
}
