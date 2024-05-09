#ifndef CRC32_H
#define CRC32_H




#include "types.h"

uint32_t crc32ByteCalc(uint32_t crc32Base, uint8_t NewDataByte);
uint32_t crc32BuffCalc(uint8_t * Message, uint32_t Offset, uint32_t  nBytes);
uint32_t crc32Init();
#endif
