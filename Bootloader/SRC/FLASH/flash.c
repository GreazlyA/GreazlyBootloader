/*************************************************************************
*
*
*
*
*
*
************************************************************************/

#include "FLASH/flash.h"
#include "em_msc.h"
#include "string.h"
#include "MEM/mem.h"
#include "string.h"
#include "main.h"
#include "string.h"
#include "crc32.h"



FlashFsmState_T FlashFsmState;









/****************************************************************
 * GLOBAL VARIABLES
 ****************************************************************/


static void drv_FLASH_StatusWait( uint32_t mask ,  uint32_t bit_val  ) ;





/*******************************************************************
*** flashWriteUint8
*
*
*
*
*******************************************************************/
void 	flashWriteUint8	(uint32_t addr, uint8_t* pbuff_of_uint8, uint32_t num_of_uint8)
{
	uint32_t i;
	uint8_t * ptr;
	uint8_t * myPtr;
	uint32_t PageSize;

	if(addr < FLASH_SIZE) {
	    PageSize = FLASH_PAGE_SIZE;
	} else
	if ((addr >= USERDATA_BASE ) && (addr < USERDATA_BASE + USERDATA_SIZE))
	    PageSize = USERDATA_SIZE;
	 else
	   return;

	if(!pbuff_of_uint8)
		return;
	if(num_of_uint8 > PageSize)
		return;
	// Get the begin page address corresponding with the required address
	uint8_t * flashPageBegin = (uint8_t *)(((uint32_t)addr / (uint32_t)PageSize) * (uint32_t)PageSize);
	// Calculate the true programming size which can not acceded the end of the page
	if (  num_of_uint8 >  (uint32_t)(flashPageBegin + PageSize - addr))
		return;
	// Allocate one page of memory for temporary storage
	ptr = memMalloc(PageSize);
	if(!ptr)
		return;
	// myPtr points to the offset of the data in the ram page allocated
	myPtr = ptr + ((uint8_t*)addr - flashPageBegin);
	// Read the page to the allocated memory
	memcpy(ptr, flashPageBegin, PageSize );
	// update the allocated memory with the data pointed to by pbuff_of_uint8
	for(i=0; i<num_of_uint8; i++)
	{
		*myPtr++ = *pbuff_of_uint8++;
	}
	// Write the new page
	flashWritePage((uint32_t) flashPageBegin, ptr, PageSize);
	// Free the memory
	memFree(ptr);
}

/*******************************************************************
*** uint32_t 	flashReadUint8(uint32_t addr,uint8_t* 	pbuff_of_uint8, uint32_t num_of_uint8)
*
*
*
*
*******************************************************************/
uint32_t 	flashReadUint8(uint32_t addr,uint8_t* 	pbuff_of_uint8, uint32_t num_of_uint8)
{
	uint32_t 	idx 	= 0					;
	volatile uint32_t*	paddr32	= (volatile uint32_t*)addr 			;
	uint32_t* 			pbuff_of_uint32 = (uint32_t*)pbuff_of_uint8 ;
	uint32_t			num = num_of_uint8 ;


	while( num > 0 )
	{
		pbuff_of_uint32[ idx++ ] = *( paddr32++ ) ;
		num -= 4	;
	}
	return ( num_of_uint8 ) ;
}


/*******************************************************************
*** uint8_t   flashDataWriteEraseIfNeeded(uint32_t  page_addr, uint8_t*   pbuff_of_uint8, uint32_t  num_of_uint8)
*
*
*
*
*******************************************************************/
uint8_t  	flashDataWriteEraseIfNeeded(uint32_t	page_addr, uint8_t* 	pbuff_of_uint8, uint32_t	num_of_uint8)
{
  uint8_t MyReturn;

  if(!(page_addr % 4)) {

      if(!(page_addr % FLASH_PAGE_SIZE)){
          uartsDebugStringWrite("flash.c: Sector erase\n\r");
          MSC_ErasePage( (uint32_t*)page_addr ) ;
      }
      uartsDebugStringWrite("flash.c: Entry write\n\r");
      MSC_WriteWord( (uint32_t*)page_addr, pbuff_of_uint8, num_of_uint8) ;
      drv_FLASH_StatusWait( MSC_STATUS_BUSY, 0 )  ;

      MyReturn = 1;
  } else {
      MyReturn = 0;
  }

  return(MyReturn);
}


/*******************************************************************
*** void  flashWritePage(uint32_t page_addr, uint8_t*   pbuff_of_uint8, uint32_t  num_of_uint8)
*
*
*
*
*******************************************************************/
void  flashWritePage(uint32_t page_addr, uint8_t*   pbuff_of_uint8, uint32_t  num_of_uint8)
{

//  CORE_CRITICAL_SECTION (
  MSC_ErasePage( (uint32_t*)page_addr ) ;
  MSC_WriteWord( (uint32_t*)page_addr, pbuff_of_uint8, num_of_uint8  ) ;
  //  while ( ( MSC->STATUS & MSC_STATUS_BUSY ) ) ; // Waiting for completion
  drv_FLASH_StatusWait( MSC_STATUS_BUSY, 0 )  ;

//  )
}

/*******************************************************************
*** void 	flashWriteUnit8ToErasedPage(uint32_t page_addr, uint8_t* pbuff_of_uint8, uint32_t	num_of_uint8)
*
*
*
*
*******************************************************************/
void 	flashWriteUnit8ToErasedPage(uint32_t page_addr, uint8_t* pbuff_of_uint8, uint32_t	num_of_uint8)
{
  CORE_CRITICAL_SECTION(
	MSC_WriteWord( (uint32_t*)page_addr, pbuff_of_uint8, num_of_uint8  ) ;
	//	while ( ( MSC->STATUS & MSC_STATUS_BUSY ) ) ;	// Waiting for completion
	drv_FLASH_StatusWait( MSC_STATUS_BUSY, 0 )	;
	)
}



/*******************************************************************
*** void 	flashErasePage	( uint32_t page_addr	)
*
*
*
*******************************************************************/
void 	flashErasePage	( uint32_t page_addr	)
{
  MSC_ErasePage( (uint32_t*)page_addr ) ;
	while ( ( MSC->STATUS & MSC_STATUS_BUSY ) ) ;	// Waiting for completion
	//drv_FLASH_StatusWait( MSC_STATUS_BUSY, 0 )	;
}


/*******************************************************************
*** void 	flashEraseAll	( void	)
*
*
*
*******************************************************************/
void 	flashEraseAll	( void	)
{
	//MSC_MassErase( ) ;
//	while ( ( MSC->STATUS & MSC_STATUS_BUSY ) ) ;	// Waiting for completion
	drv_FLASH_StatusWait( MSC_STATUS_BUSY, 0 )	;
}



/*******************************************************************
*** tatic void drv_FLASH_StatusWait( uint32_t mask ,  uint32_t bit_val  )
*
*
*
*******************************************************************/
static void drv_FLASH_StatusWait( uint32_t mask ,  uint32_t bit_val  )
{
	uint32_t status;
	int timeOut = MSC_PROGRAM_TIMEOUT;

	while (1)
	{
		status = MSC->STATUS;

		// Check errors
		if ( status & ( MSC_STATUS_LOCKED | MSC_STATUS_INVADDR ) )
		{
			// Disable write access
//			MSC->WRITECTRL &= ~(MSC_WRITECTRL_WREN | MSC_WRITECTRL_WDOUBLE);
			MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;

			// Set error flag and enter busy loop
			if ( status & MSC_STATUS_LOCKED )
			{
//				EFM_ASSERT(0) ;
			}

			// Set error flag and enter busy loop
			if ( status & MSC_STATUS_INVADDR )
			{
//				EFM_ASSERT(0) ;
			}
		}

		// Process WORDTIMEin case when waiting for WDATAREADY only
		if ( (status & MSC_STATUS_TIMEOUT) && (mask == MSC_STATUS_WDATAREADY) )
		{
//			EFM_ASSERT(0) ;
		}

		// Check end condition
		if ( ( status & mask ) == bit_val )
		{
			// ok
			break;
		}

		timeOut--;
		if ( timeOut == 0 )
		{
			//Timeout
//			EFM_ASSERT(0) ;
		}
	}
}



void flashWriteBuffer(uint32_t WriteAddress, uint8_t* pBuffer, uint16_t NumByteToWrite, uint8_t FreeTxBufferFlag)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, AlignedAddr = 0, count = 0, temp = 0;

  AlignedAddr = WriteAddress % FLASH_PAGE_SIZE;
  count = FLASH_PAGE_SIZE - AlignedAddr;
  NumOfPage =  NumByteToWrite / FLASH_PAGE_SIZE;
  NumOfSingle = NumByteToWrite % FLASH_PAGE_SIZE;

  if (AlignedAddr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
    {
      if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
        {
          flashDataWriteEraseIfNeeded(WriteAddress, pBuffer, NumByteToWrite);
          WriteAddress += NumByteToWrite;
        }
      else /*!< NumByteToWrite > sFLASH_PAGESIZE */
        {
          if(NumOfPage>1){
              while (NumOfPage--)
                {
                  flashDataWriteEraseIfNeeded(WriteAddress, pBuffer, FLASH_PAGE_SIZE);
                  WriteAddress += FLASH_PAGE_SIZE;
                  pBuffer += FLASH_PAGE_SIZE;
                }

              flashDataWriteEraseIfNeeded(WriteAddress, pBuffer, NumOfSingle);
             WriteAddress += NumOfSingle;
          }
          else{
              flashDataWriteEraseIfNeeded(WriteAddress, pBuffer, NumByteToWrite);
              WriteAddress += NumByteToWrite;
          }
        }
    }
  else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
    {
      if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
        {
          if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
            {
              temp = NumOfSingle - count;
              flashDataWriteEraseIfNeeded(WriteAddress, pBuffer, count);
              //spiflashLogMemory.spiflashCurrentLogAddress +=  count;
              WriteAddress += count;
              pBuffer += count;

              flashDataWriteEraseIfNeeded(WriteAddress, pBuffer, temp);
              //spiflashLogMemory.spiflashCurrentLogAddress += temp;
              WriteAddress += temp;
            }
          else
            {
              flashDataWriteEraseIfNeeded(WriteAddress, pBuffer, NumByteToWrite);
              //spiflashLogMemory.spiflashCurrentLogAddress += NumByteToWrite;
              WriteAddress += NumByteToWrite;
            }
        }
      else /*!< NumByteToWrite > sFLASH_PAGESIZE */
        {
          NumByteToWrite -= count;
          NumOfPage =  NumByteToWrite / FLASH_PAGE_SIZE;
          NumOfSingle = NumByteToWrite % FLASH_PAGE_SIZE;

          flashDataWriteEraseIfNeeded(WriteAddress, pBuffer, count);
          WriteAddress += count;
          pBuffer += count;

          while (NumOfPage--)
            {
              flashDataWriteEraseIfNeeded(WriteAddress, pBuffer, FLASH_PAGE_SIZE);
              WriteAddress += FLASH_PAGE_SIZE;
              pBuffer += FLASH_PAGE_SIZE;
            }

          if (NumOfSingle != 0)
            {
              flashDataWriteEraseIfNeeded(WriteAddress, pBuffer, NumOfSingle);
              WriteAddress += NumOfSingle;
            }
        }
    }
}




