#include "main.h"
#include "crc32.h"
#include "em_msc.h"


volatile uint32_t BootloaderVersion		=	3;

uint8_t mainNewFwUpgradeImageCheck();
void mainImageCopy(uint32_t ImageLength);
void bootloaderPrint(char* String);

volatile const int mainRestartFlag __attribute__ ((section (".ResetartFlagSection"))) = 0xffffffff;

#define FWUPGRADE_FLASH_IMAGE_BASE_ADDRESS 	(uint32_t)0x28000
#define FWUPGRADE_FLASH_APP_IMAGE_START		(uint32_t)0x2000
#define FLASH_SECTOR_SIZE					(uint32_t)(8*1024)

// void (*ApplicationStartFunctionPtr)() = (uint8_t*)0x8004194;


typedef struct {
	uint32_t  HeaderLength; // uint32	HeaderLength
	uint32_t  ImageLength;  // uint32	ImageLength
	uint32_t  ImageCrc32;   // uint32	TBD0
	uint32_t  TBD1;         // uint32	TBD1;
	uint32_t  TBD2;         // uint32	TBD2;
	uint32_t  TBD3;         // uint32	TBD3;
	uint32_t  TBD4;         // uint32	TBD4;
	uint32_t  HeaderCrc32;  // uint32	HeaderCrc32;
}  bootloaderOtaHeaderStruct_T;

bootloaderOtaHeaderStruct_T OtaImageStructure;

uint32_t FlashTargetAddress = FWUPGRADE_FLASH_IMAGE_BASE_ADDRESS;

void mainImageCopy();






__attribute__ ((section(".myBootSection")))  void Bootloader ();
void  Bootloader()
{

	bootloaderPrint("******************************Bootloader Start\n\r");
	uint8_t* FlashPtr = (uint8_t*)FWUPGRADE_FLASH_IMAGE_BASE_ADDRESS;
	memcpy(&OtaImageStructure, FlashPtr , sizeof(OtaImageStructure));

	OtaImageStructure.HeaderCrc32 = HTONL(OtaImageStructure.HeaderCrc32);
	OtaImageStructure.HeaderLength = HTONL(OtaImageStructure.HeaderLength);
	OtaImageStructure.ImageCrc32 = HTONL(OtaImageStructure.ImageCrc32);
	OtaImageStructure.ImageLength = HTONL(OtaImageStructure.ImageLength);
	OtaImageStructure.TBD1 = NTOHL(OtaImageStructure.TBD1);
	OtaImageStructure.TBD2 = NTOHL(OtaImageStructure.TBD2);
	OtaImageStructure.TBD3 = NTOHL(OtaImageStructure.TBD3);
	OtaImageStructure.TBD4 = NTOHL(OtaImageStructure.TBD4);

	if(!mainNewFwUpgradeImageCheck() )
	{
		bootloaderPrint("Fw upgrade image all 0xff\n\r");
		// If the alternate image is all 0xff, we proceed to normal application start
		uint32_t MyCalculatedCrc32 = crc32BuffCalc((uint8_t*)((FWUPGRADE_FLASH_IMAGE_BASE_ADDRESS) + sizeof(bootloaderOtaHeaderStruct_T)), 0, OtaImageStructure.ImageLength);
		if(MyCalculatedCrc32 == OtaImageStructure.ImageCrc32){
			bootloaderPrint("Fw upgrade spi imageCRC32 OK\n\r");
			uint32_t MyCalculatedCrc32 = crc32BuffCalc((uint8_t*)(FWUPGRADE_FLASH_APP_IMAGE_START), 0, OtaImageStructure.ImageLength);
			if(MyCalculatedCrc32 != OtaImageStructure.ImageCrc32){
				bootloaderPrint("Application CRC32 fail, copy spi to app\n\r");
				mainImageCopy(OtaImageStructure.ImageLength);
				// Clear the FOTA image header
				MSC_ErasePage(FWUPGRADE_FLASH_IMAGE_BASE_ADDRESS) ;
				NVIC_SystemReset();
				} else {
					bootloaderPrint("Application CRC32 OK, go to app\n\r");
				}
		} else {
			bootloaderPrint("Spi image CRC fail, go to app\n\r");
		}
	} else {
		bootloaderPrint("Image space not 0xff, go to app\n\r");
	}

	bootloaderPrint("Start app\n\r");
	//ApplicationStartFunctionPtr();
 	__asm("b 0x2141");
}

/*

The FW image is located between sectors 1 to 19

The FW Upload image is located between sectors 20 to 39

*/


__attribute__ ((section(".myBootSection")))  void  mainImageCopy();
void mainImageCopy(uint32_t ImageLength)
{
	  uint32_t i;
	  uint32_t SourceFlashAddress 		= FWUPGRADE_FLASH_IMAGE_BASE_ADDRESS+sizeof(bootloaderOtaHeaderStruct_T);
	  uint32_t DestinationFlashAddress 	= FWUPGRADE_FLASH_APP_IMAGE_START;


	  for(i=FWUPGRADE_FLASH_APP_IMAGE_START; i<FWUPGRADE_FLASH_IMAGE_BASE_ADDRESS; i+=FLASH_SECTOR_SIZE){
	      MSC_ErasePage( (uint32_t*)i ) ;
	      while ( ( MSC->STATUS & MSC_STATUS_BUSY ) ) ; // Waiting for completion
	  }


    for(i=FWUPGRADE_FLASH_APP_IMAGE_START; i<FWUPGRADE_FLASH_IMAGE_BASE_ADDRESS; i+=FLASH_SECTOR_SIZE){
        MSC_WriteWord( (uint32_t*)DestinationFlashAddress, (uint8_t*)SourceFlashAddress, FLASH_SECTOR_SIZE ) ;
        while ( ( MSC->STATUS & MSC_STATUS_BUSY ) ); // Waiting for completion
        DestinationFlashAddress += FLASH_SECTOR_SIZE;
        SourceFlashAddress += FLASH_SECTOR_SIZE;
    }
}


__attribute__ ((section(".myBootSection")))  uint8_t  mainNewFwUpgradeImageCheck();
uint8_t mainNewFwUpgradeImageCheck()
{
	uint8_t MyReturn = 0;
	uint32_t Counter;
	uint8_t* FlashTargetAddress = (uint8_t*)FWUPGRADE_FLASH_IMAGE_BASE_ADDRESS;

	for(Counter = 0; Counter < sizeof(bootloaderOtaHeaderStruct_T); Counter++)
		if(*FlashTargetAddress != 0xff)
			break;

	if(Counter < sizeof(bootloaderOtaHeaderStruct_T))
		MyReturn = 0;
	else
		MyReturn = 1;

	return(MyReturn);
}

__attribute__ ((section(".myBootSection")))  void  bootloaderPrint(char* String);
void bootloaderPrint(char* String)
{


 // UARTDRV_ForceTransmit( inst, (uint8_t*) String, strlen(String), 10);
}
