
#ifndef FLASH_H_
#define FLASH_H_

#include "stdint.h"
#include "em_device.h"



/******************************************
 * 	Flash Internal Structure
 ******************************************/

///***********************************************************
#define  FLASH_TOTAL_SUBSECTORS       64  //
#define  FLASH_SECTOR_SIZE            (8*1024)
#define  FLASH_FLASH_START_SECTOR     0

// * 		FLASH DATA BASE DEFINITION
#define FLASH_FW_START_ADDRESS        0x00000   // Start Address for FW  (160 B)
#define FLASH_FW_END_ADDRESS          0x27fff   // End address of FW
#define FLASH_OTA_START_ADDRESS       0x28000   // Start Address for OTA (160 KB)
#define FLASH_OTA_END_ADDRESS         0x4ffff   // End Address for OTA

#define FLASH_ARCIVE_START_ADDRESS    0x50000   // Start Address for archive
#define FLASH_ARCIVE_END_ADDRESS      0x77fff   // End Address for archive

#define FLASH_TOKEN_STORAGE_START_ADDRESS    0x78000   // Start Address for Context storage
#define FLASH_TOKEN_STORAGE_END_ADDRESS      0x79fff   // End Address Context storage

#define FLASH_CONFIG_DATA_START_ADDRESS       USERDATA_BASE
#define FLASH_CONFIG_DATA_END_ADDRESS         USERDATA_END




// ***********************************************************/
typedef enum {
    FLASH_FSM_STATE_IDLE,
    FLASH_FSM_STATE_WRITE_WAIT,
    FLASH_FSM_STATE_ERASE_WAIT,
} FlashFsmState_T;

typedef enum {
    FLASH_STIMULI_BUFFER_WRITE,
    FLASH_STIMULI_BUFFER_ERASE,
    FLASH_STIMULI_ONLY_TIMEOUT
    // Add other commands as needed
} FlashFsmStimuli_T;


typedef struct {
    uint32_t Address;   // Flash memory address for the operation
    uint8_t* DataPtr;   // Pointer to data buffer for write/read operations
    uint16_t Length;    // Length of the data to be written/read
    void (*CompletionCallBackPtr);
    uint8_t MemoryReleaseFlag;
} FlashCommandDescriptor_T;


typedef struct {
    uint32_t TokenLength;         // Flash memory address for the operation
    uint32_t TokenCrc32;          // Pointer to data buffer for write/read operations
    char     TokenString[100];    // Length of the data to be written/read
} FlashTokenStructure_T;


typedef  struct
{
  // Requests sent by external entity to the FSM
  FlashFsmStimuli_T   RequestType;
  uint8_t*            DataPtr;
} FlashQueueEntry_T;





#define FLASH_STATE_CHANGE_TIMEOUT( NextState, Timeout) \
    FlashFsmState = NextState; \
    schedulerTimerSchedule(FlashTaskHandlerPtr, (uint32_t) Timeout)

#define FLASH_STATE_CHANGE_NO_TIMEOUT( NextState) \
    FlashFsmState = NextState





// Prototypes
void flashInit(void);
void flashTimerCallBack(void);

//FLASH_CTRLBLOCK_t*  flashConfig(void);
void flashClearConfig(void);
void 	flashWriteUint8	(uint32_t addr, uint8_t* pbuff_of_uint8, uint32_t num_of_uint8);
uint32_t 	flashReadUint8(uint32_t addr,uint8_t* 	pbuff_of_uint8, uint32_t num_of_uint8);
void 	 flashWritePage(uint32_t	page_addr, uint8_t* 	pbuff_of_uint8, uint32_t	num_of_uint8);
void 	 flashWriteUnit8ToErasedPage(uint32_t page_addr, uint8_t* pbuff_of_uint8, uint32_t	num_of_uint8);
void 	 flashErasePage(uint32_t page_addr);
void 	 flashEraseAll(void);
void flashOperation(FlashFsmStimuli_T Command, uint32_t Address, uint8_t* BufferPtr, uint16_t Length,
                    void (*CompletionCallBack)(), uint8_t MemoryReleaseFlag);



#endif /* FLASH_H_ */
