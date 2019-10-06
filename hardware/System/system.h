/********************************************************************************
Bkav
Product: 
Module: Header file of device.c module
Version: 1.0
Author: DongNX
Created: 4/2015
Modified: 
<Name>
<Date>
<Change>
Released: <Date>
Description: <Description>
Note: <Note>
********************************************************************************/
#ifndef __SYSTEM_H__
#define __SYSTEM_H__
/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/
#define TICK_INTERRUPTS_EVENT   0
#define TICK_INTERRUPTS_COUNT   1
/*-----------------------------------------------------------------------------*/
/* Data type definitions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Constant definitions  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Macro definitions  */
/*-----------------------------------------------------------------------------*/
#ifdef RELEASE
#ifdef FLASH_32K
#define APPLICATION_ADDRESS     (DWORD)0x08000800 
#endif
#ifdef HIGHFLASH_256K
#define APPLICATION_ADDRESS		(DWORD)0x08001000
#endif
#endif
#ifdef DEBUG
#define APPLICATION_ADDRESS     (DWORD)0x08000000
#endif
/*-----------------------------------------------------------------------------*/
/* Global variables  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Function prototypes  */
/*-----------------------------------------------------------------------------*/
VOID InitSystem();
WORD GetTickCount();
WORD GetSecondCount();
WORD Random(WORD nRange);
BYTE ByteRandom();
VOID Reboot();
VOID CopyMemory(PBYTE pDest, PBYTE pSource, WORD nSize);
VOID CopyInvert(PBYTE pDest, PBYTE pSource, BYTE nSize);
VOID ZeroMemory(PBYTE pDest, BYTE nSize);
VOID ActivateWatchdog();
VOID ResetWatchdog();
VOID SetDefaultCacheMode();
VOID SetPrefetchCacheMode();
VOID RegisterTickCallback(BYTE nEvent, SYSTEMCALLBACK fnCallback);

#endif  //__SYSTEM_H__