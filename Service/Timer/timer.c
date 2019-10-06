/********************************************************************************  

Product: SHTMS

Module: timer.c

Version: 1.0 

Author: NGUYEN KIM THANH

Created: 08-2014 

Modified:  
<Name> 
<Date> 
<Change> 

Released: 

Description: Including functions for Timer

Note: <Note> 

********************************************************************************/ 

/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/
#include "system.h"
#include "timer.h"
/*-----------------------------------------------------------------------------*/
/* Constant definitions  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Macro definitions  */
/*-----------------------------------------------------------------------------*/
#define TIMER_TYPE_SHORT	0
#define TIMER_TYPE_LONG		1

#define MAX_TIMER_COUNT		32

#define TIMER_EXPIRED(timer) ((((timer).nType == TIMER_TYPE_SHORT) && ((WORD)(GetTickCount() - (timer).nStart) >= (timer).nTime)) \
|| (((timer).nType == TIMER_TYPE_LONG) && ((WORD)(GetSecondCount() - (timer).nStart) >= (timer).nTime)))
/*-----------------------------------------------------------------------------*/
/* Data type definitions */
/*-----------------------------------------------------------------------------*/
typedef struct tagTIMER
{
	BYTE nType;
	WORD nStart;
	WORD nTime;
	SYSTEMCALLBACK fnCallback;
	PVOID pParameter;
} TIMER, *PTIMER;
/*-----------------------------------------------------------------------------*/
/* Global variables  */
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
/* Function prototypes  */
/*-----------------------------------------------------------------------------*/

INTERNAL TIMER g_pTimers[MAX_TIMER_COUNT] = { NULL };
/*------------------------------------------------------------------------------- 
Function: VOID ProcessTimerEvents(VOID);

Purpose: Excuting callback function respectively event which was registered above.

Parameters: None

Return: None

Comments: If timer interrupt come, ProcessTimerEvents will check value with some circumstance to do something respectively

Modified: 
<Modified by> 
<Date> 
<Change>
--------------------------------------------------------------------------------*/ 
VOID ProcessTimerEvents()
{
	INTERNAL BYTE nIndex;
	INTERNAL SYSTEMCALLBACK fnCallback;
	INTERNAL PVOID pParameter;
	for (nIndex = 0; nIndex < MAX_TIMER_COUNT; nIndex++)
	{
		if ((g_pTimers[nIndex].fnCallback != NULL) && TIMER_EXPIRED(g_pTimers[nIndex]))
		{
			fnCallback = g_pTimers[nIndex].fnCallback;
			pParameter = g_pTimers[nIndex].pParameter;
			g_pTimers[nIndex].fnCallback = NULL;
			fnCallback(pParameter);
		}
	}
}
/*------------------------------------------------------------------------------- 
Function: VOID StartLongTimer(WORD wSecondCount, SYSTEMCALLBACK Callback, PVOID pData);

Purpose: Registering a "Callback" function with timer value is "wSecondCount".

Parameters: wSecondCount, Callback, pData

Return: None

Comments: MAXCALLBACKFUNC is 32.

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/ 
VOID StartLongTimer(WORD nSeconds, SYSTEMCALLBACK fnCallback, PVOID pParameter)
{
	for (BYTE nIndex = 0; nIndex < MAX_TIMER_COUNT; nIndex++)
	{
		if ((g_pTimers[nIndex].fnCallback == NULL) || (g_pTimers[nIndex].fnCallback == fnCallback))
		{
			g_pTimers[nIndex].nType = TIMER_TYPE_LONG;
			g_pTimers[nIndex].nStart = GetSecondCount();
			g_pTimers[nIndex].nTime = nSeconds;
			g_pTimers[nIndex].fnCallback = fnCallback;
			g_pTimers[nIndex].pParameter = pParameter;						
			for (nIndex = nIndex + 1; nIndex < MAX_TIMER_COUNT; nIndex++)
			{
				if (g_pTimers[nIndex].fnCallback == fnCallback)
				{
					g_pTimers[nIndex].fnCallback = NULL;
				}
			}
			return;
		}
	}
	// No timer available, critical error, must reboot to recover
	Reboot();	
}
/*------------------------------------------------------------------------------- 
Function: VOID  StartShortTimer (WORD wTickCount, SYSTEMCALLBACK Callback, PVOID pData);

Purpose: Registering a "Callback" function with timer value is "wTickCount".

Parameters: wTickCount, Callback, pData

Return: None

Comments: MAXCALLBACKFUNC is 32.

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/
VOID StartShortTimer(WORD nMillis, SYSTEMCALLBACK fnCallback, PVOID pParameter)
{
	BYTE nIndex;
	for (nIndex = 0; nIndex < MAX_TIMER_COUNT; nIndex++)
	{
		if (g_pTimers[nIndex].fnCallback == NULL || g_pTimers[nIndex].fnCallback == fnCallback)
		{
			g_pTimers[nIndex].nType = TIMER_TYPE_SHORT;
			g_pTimers[nIndex].nStart = GetTickCount();
			g_pTimers[nIndex].nTime = nMillis;
			g_pTimers[nIndex].fnCallback = fnCallback;
			g_pTimers[nIndex].pParameter = pParameter;
			for (nIndex = nIndex + 1; nIndex < MAX_TIMER_COUNT; nIndex++)
			{
				if (g_pTimers[nIndex].fnCallback == fnCallback)
				{
					g_pTimers[nIndex].fnCallback = NULL;
				}
			}
			return;
		}
	}
	// No timer available, critical error, must reboot to recover
	Reboot();	
}
/*------------------------------------------------------------------------------- 
Function: VOID CancelTimer(SYSTEMCALLBACK Callback, PVOID pData);

Purpose: Cancelling a function which has name is Callback. 

Parameters: Callback, pData

Return: none 

Comments: MAXCALLBACKFUNC is 32.

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/
VOID CancelTimer(SYSTEMCALLBACK fnCallback)
{
	BYTE nIndex;
	for (nIndex = 0; nIndex < MAX_TIMER_COUNT; nIndex++)
	{
		if (g_pTimers[nIndex].fnCallback == fnCallback)
		{
			g_pTimers[nIndex].fnCallback = NULL;
			break;
		}
	}
}
/*------------------------------------------------------------------------------- 
Function: BOOL IsRunning (SYSTEMCALLBACK Callback, PVOID pData);

Purpose: Checking "Callback" function, isn't running?

Parameters: Callback, pData

Return: Respective circumstance.

Comments: MAXCALLBACKFUNC is 32.

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/ 
BOOL IsRunning(SYSTEMCALLBACK fnCallback)
{
	BYTE nIndex;
	for (nIndex = 0; nIndex < MAX_TIMER_COUNT; nIndex++)
	{
		if (g_pTimers[nIndex].fnCallback == fnCallback)
		{
			return TRUE;
		}
	}
	return FALSE;
}
/*------------------------------------------------------------------------------- 
Function: VOID Delay_ms(__IO DWORD nTime)

Purpose: Set delay time with ms unit.

Parameters: None

Return: None

Comments: None

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/ 
VOID Delay_ms(volatile DWORD nTime)
{
	WORD nTickStart = 0;
	nTickStart = GetTickCount();
	while ((GetTickCount() - nTickStart) < nTime);
}