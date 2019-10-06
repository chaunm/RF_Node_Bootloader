/********************************************************************************  

Product: SHTMS

Module: flash.c

Version: 1.0 

Author: NGUYEN KIM THANH

Created: 08-2014 

Modified:  
<Name> 
<Date> 
<Change> 

Released: 

Description: Including functions for flash

Note: <Note> 

********************************************************************************/

/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/
#include "stm32f0xx_flash.h"
#include "flash.h"
/*-----------------------------------------------------------------------------*/
/* Constant definitions  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Macro definitions  */
/*-----------------------------------------------------------------------------*/
#define FLASH_BEGIN_ADDRESS		0x08000000
#define BLOCK_TO_ADDR(block) 	(((block) << 2) + FLASH_BEGIN_ADDRESS)
/*-----------------------------------------------------------------------------*/
/* Data type definitions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Global variables  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Function prototypes  */
/*-----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------- 
Function: VOID EraseFlash(DWORD nPage, DWORD nCount);

Purpose: Erase Flash from Address nPage

Parameters: nPage, nCount

Return: None

Comments:  
nPage: Address which was deleted: 0x08000000 < nPage < 0x080FFFFF)
nCount: Number of page for delete

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/
VOID EraseFlash(DWORD nPage, DWORD nCount)
{
	DWORD nAddress;
	while (nCount != 0)
	{
		nAddress = (nPage << FLASH_PAGE_FACTOR) + FLASH_BEGIN_ADDRESS;
        FLASH_Unlock();
        FLASH_ErasePage(nAddress);
        FLASH_Lock();
		nPage++;
		nCount--;
	}
}
/*------------------------------------------------------------------------------- 
Function: VOID WriteFlash(PVOID pData, WORD nSize, DWORD nAddr);

Purpose: Writing nSize to Flash from "nAddress"

Parameters: pData, nSize, nAddr

Return: None

Comments:
nSize: Size of data which want to flash (nSize%4 = 0)
nAddress was written to DWORD in Flash. Address must be multiple 4
pData: Data pointer to write
Flash_ProgramWord() write 32 bits every times.
Note input format of pData.
Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/
VOID WriteFlash(PVOID pData, WORD nSize, DWORD nAddr)
{
	DWORD nAddress = nAddr;
	PDWORD pPointer = (PDWORD)pData;
	while (nSize > 3)
	{
		nAddress = (nAddr << 2) + FLASH_BEGIN_ADDRESS;
        FLASH_Unlock();
		FLASH_ProgramWord(nAddress, *(pPointer));
        FLASH_Lock();
		pPointer++;
		nSize -= 4;
		nAddr++;
	}
}
/*------------------------------------------------------------------------------- 
Function: VOID WriteFlash(PVOID pData, WORD nSize, DWORD nAddr);

Purpose: Writing nSize to Flash from "nAddress"

Parameters: pData, nSize, nAddr

Return: None

Comments:
nSize: Size of data which want to flash (nSize%4 = 0)
nAddress was written to DWORD in Flash. Address must be multiple 4
pData: Data pointer to write
Flash_ProgramWord() write 32 bits every times.
Note input format of pData.
Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/
VOID ReadFlash(PVOID pBuffer, WORD nSize, DWORD nAddr)
{
	PDWORD pDest = (PDWORD)pBuffer;
	if ((nSize % 4) != 0)
	  	return;
	while (nSize > 0)
	{
	  	*pDest = (DWORD)(*(PDWORD)FlashToMemory(nAddr));
		nAddr++;
		pDest++;
		nSize -= 4;
	}
}
/*------------------------------------------------------------------------------- 
Function: PVOID FlashToMemory(DWORD nAddr);

Purpose: Reading data from "nAddr" in Flash

Parameters: nAddr

Return: BLOCK_TO_ADDR(nAddr)

Comments: None

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/
PVOID FlashToMemory(DWORD nBlkAddr)
{
	return (PVOID)(BLOCK_TO_ADDR(nBlkAddr));
}

// this function is for backward compatible
VOID InitFlash()
{
}



