/********************************************************************************
Bkav
Product: Platform STM32F100
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
#ifndef __TIMER_H__
#define __TIMER_H__
/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Data type definitions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Constant definitions  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Macro definitions  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Global variables  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Function prototypes  */
/*-----------------------------------------------------------------------------*/
VOID ProcessTimerEvents();
VOID StartLongTimer(WORD nSeconds, SYSTEMCALLBACK fnCallback, PVOID pParameter);
VOID StartShortTimer(WORD nMillis, SYSTEMCALLBACK fnCallback, PVOID pParameter);
VOID CancelTimer(SYSTEMCALLBACK fnCallback);
BOOL IsRunning(SYSTEMCALLBACK fnCallback);
VOID Delay_ms(volatile DWORD nTime);

#endif //__TIMER_H__

