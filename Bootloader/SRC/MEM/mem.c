#include "mem.h"
#include "types.h"


// L O C A L    D E F I N I T I O N S


// L O C A L    P R O T O T Y P E S


// G L O B A L S

/***********************************************************************************
*** ReturnCode_T 		bitInit()
*
*
*
*
***********************************************************************************/
void * 		memMalloc(uint32_t size)
{

	void * myTempPtr = (void*) malloc(size);


	if(!myTempPtr)
		while(1);

	return(myTempPtr);


}




/***********************************************************************************
*** ReturnCode_T 		bitStart()
*
*
*
*
***********************************************************************************/
 void		memFree(void * myPtr)
{
	 free(myPtr);
}

