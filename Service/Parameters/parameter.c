/********************************************************************************  

Product: SHTMS

Module: parameter.c

Version: 1.0 

Author: NGUYEN KIM THANH

Created: 08-2014 

Modified:  
<Name> 
<Date> 
<Change> 

Released: 

Description: Including functions for parameter

Note: <Note> 

********************************************************************************/ 

/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/
#include "flash.h"
#include "parameter.h"
/*-----------------------------------------------------------------------------*/
/* Constant definitions  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Macro definitions  */
/*-----------------------------------------------------------------------------*/
#define ACTIVE_FLAG		0x55AAF50A
#define UNUSED_FLAG		0x00000000

#ifdef FLASH_32K // chaunm - need to check with datasheet again
#define LOW_SECTION     0x1E00 
#define HIGH_SECTION    0x1F00
#define SECTION_SIZE	0x0100	//1kB
#endif
#ifdef LOWFLASH_64K
#define LOW_SECTION		0x3C00	
#define HIGH_SECTION	0x3E00
#define SECTION_SIZE	0x0200	//2kB
#elif HIGHFLASH_128K
#define LOW_SECTION		0x7C00	
#define HIGH_SECTION	0x7E00
#define SECTION_SIZE	0x0200	//2kB
#endif
#if HIGHFLASH_256
#define LOW_SECTION		0xFC00	
#define HIGH_SECTION	0xFE00
#define SECTION_SIZE	0x0200	//2kB
#endif

#define FREE_ITEM 		0xFF
#define ZERO_ITEM		0x00
#define FREE_BLOCK		0xFFFFFFFF
#define ZERO_BLOCK		0x00000000

#define SECTION_ACTIVE(block) (*((PDWORD)(FlashToMemory(block))) == ACTIVE_FLAG)
#define g_pParamItems PARAM_ITEMS

/*-----------------------------------------------------------------------------*/
/* Data type definitions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Global variables  */
/*-----------------------------------------------------------------------------*/
PPARAMITEM PARAM_ITEMS[MAX_PARAM_ITEM] = { 0 };
INTERNAL BYTE g_nParamCount = 0;
INTERNAL WORD g_nActiveSection = 0;
INTERNAL SYSTEMCALLBACK g_fnParamEvents[PARAM_EVENT_COUNT] = { NULL };
/*-----------------------------------------------------------------------------*/
/* Function prototypes  */
/*-----------------------------------------------------------------------------*/
// Internal functions

INTERNAL PPARAMITEM NewItem(PPARAMITEM pSource);
INTERNAL PPARAMITEM SearchItem(WORD nParam);
INTERNAL VOID FormatSection(WORD nAddr);
INTERNAL WORD SwapSections(VOID);

// API

/*------------------------------------------------------------------------------- 
Function: VOID InitParameter(SYSTEMCALLBACK fnDefineProc);

Purpose: Initial for Parameter

Parameters: fnDefineProc

Return: none 

Comments: None

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/ 
VOID InitParameter(SYSTEMCALLBACK fnDefineProc)
{
    PARAMITEM pParamBuffer[MAX_PARAM_ITEM] = { 0 };
    PPARAMITEM pItem;
    BOOL bFormat = FALSE;
    
    if (SECTION_ACTIVE(LOW_SECTION))
	{
        g_nActiveSection = LOW_SECTION;
	}
    else if (SECTION_ACTIVE(HIGH_SECTION))
	{
        g_nActiveSection = HIGH_SECTION;
	}
    else 
    {
        FormatSection(LOW_SECTION);
        g_nActiveSection = LOW_SECTION;
        bFormat = TRUE;
    }
    
    // Define Parameters
    for (WORD nIndex = 0; nIndex < MAX_PARAM_ITEM; nIndex++)
	{
        g_pParamItems[nIndex] = &pParamBuffer[nIndex];
	}
    g_fnParamEvents[PARAM_DEFINE_EVENT] = fnDefineProc;
    g_fnParamEvents[PARAM_DEFINE_EVENT](NULL);
    
    g_nParamCount = 0;
    
    // Create or locate parameters in flash
    if (bFormat)
    {
        WORD nAddr = g_nActiveSection + 1;
        for (WORD nIndex = 0; nIndex < MAX_PARAM_ITEM; nIndex++)
        {
            if (g_pParamItems[nIndex]->nParam != 0)
            {
                WriteFlash(g_pParamItems[nIndex], sizeof(PARAMITEM), nAddr);
                g_pParamItems[nIndex] = (PPARAMITEM)FlashToMemory(nAddr);
                g_nParamCount++;
                nAddr++;
            }
        }	
    }
    else
    {
        for (WORD nIndex = 0; nIndex < MAX_PARAM_ITEM; nIndex++)
        {
            if (g_pParamItems[nIndex]->nParam != 0)
            {
                pItem = SearchItem(g_pParamItems[nIndex]->nParam);
                if (pItem == NULL)
				{
                    pItem = NewItem(g_pParamItems[nIndex]);
				}
                g_pParamItems[nIndex] = pItem;
                g_nParamCount++;
            }
        }
    }
}

/*------------------------------------------------------------------------------- 
Function: BYTE GetParamCount()

Purpose: Get Param Count

Parameters: None

Return: g_nParamCount 

Comments: None

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/ 
BYTE GetParamCount()
{
    return g_nParamCount;
}

/*------------------------------------------------------------------------------- 
Function: BOOL SetParam(WORD nParam, WORD nValue)

Purpose: Set Param Count

Parameters: nParam, nVale

Return: False 

Comments: None

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/ 
BOOL SetParam(WORD nParam, WORD nValue)
{
    for (BYTE nIndex = 0; nIndex < g_nParamCount; nIndex++)
    {
        if (g_pParamItems[nIndex]->nParam == nParam)
        {            
            PARAMITEM stItem = *g_pParamItems[nIndex];
            stItem.nValue = nValue;
            g_pParamItems[nIndex] = NewItem(&stItem);
            if (g_fnParamEvents[PARAM_CHANGE_EVENT] != NULL)
            {
                PARAMCHANGEEVENT stEvent = {
                    PARAM_CHANGE_EVENT,
                    nIndex,
                    nParam,
                    nValue };
                g_fnParamEvents[PARAM_CHANGE_EVENT](&stEvent);
            }
            return TRUE;
        }		
    }
    return FALSE;
}

/*------------------------------------------------------------------------------- 
Function: WORD GetParam(WORD nParam)

Purpose: Get Param

Parameters: nParam

Return: 0 

Comments: None

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/ 
WORD GetParam(WORD nParam)
{
    BYTE nIndex;
    for (nIndex = 0; nIndex < g_nParamCount; nIndex++)
    {
        if (g_pParamItems[nIndex]->nParam == nParam)
		{
            return g_pParamItems[nIndex]->nValue;
		}
    }
    return 0;
}

/*------------------------------------------------------------------------------- 
Function: VOID ResetParams()

Purpose: Reset Param

Parameters: None

Return: None 

Comments: None

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/ 
VOID ResetParams()
{
    PARAMITEM pParamBuffer[MAX_PARAM_ITEM] = { 0 };
    WORD nAddr;
    FormatSection(LOW_SECTION);
    g_nActiveSection = LOW_SECTION;
    
    // Define Parameters
    for (WORD nIndex = 0; nIndex < MAX_PARAM_ITEM; nIndex++)
	{
        g_pParamItems[nIndex] = &pParamBuffer[nIndex];
	}
    if (g_fnParamEvents[PARAM_DEFINE_EVENT] != NULL)
	{
        g_fnParamEvents[PARAM_DEFINE_EVENT](NULL);
	}
    
    g_nParamCount = 0;
    nAddr = g_nActiveSection + 1;
    for (WORD nIndex = 0; nIndex < MAX_PARAM_ITEM; nIndex++)
    {
        if (g_pParamItems[nIndex]->nParam != 0)
        {
            WriteFlash(g_pParamItems[nIndex], sizeof(PARAMITEM), nAddr);
            g_pParamItems[nIndex] = (PPARAMITEM)FlashToMemory(nAddr);
            g_nParamCount++;
            nAddr++;
        }
    }
}

/*------------------------------------------------------------------------------- 
Function: VOID RegisterParamCallback(BYTE nEvent, SYSTEMCALLBACK fnCallback);

Purpose: Register a function for Param Callback

Parameters: nEvent, SYSTEMCALLBACK, fnCallback

Return: None 

Comments: None

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/ 
VOID RegisterParamCallback(BYTE nEvent, SYSTEMCALLBACK fnCallback)
{
    if (nEvent < PARAM_EVENT_COUNT)
    {
        g_fnParamEvents[nEvent] = fnCallback;
    }
}

/*------------------------------------------------------------------------------- 
Function: INTERNAL PPARAMITEM NewItem(PPARAMITEM pSource);

Purpose: Get a new item

Parameters: pSource

Return: FlashToMemory(nAddr) 

Comments: None

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/ 
INTERNAL PPARAMITEM NewItem(PPARAMITEM pSource)
{
    WORD nAddr = g_nActiveSection + 1;
    PPARAMITEM pItem;
    while (nAddr < g_nActiveSection + SECTION_SIZE)
    {
        pItem = (PPARAMITEM)FlashToMemory(nAddr);
        if (pItem->nParam == pSource->nParam)
        {
            DWORD dwZero = ZERO_BLOCK;
            WriteFlash(&dwZero, sizeof(DWORD), nAddr);
        }
        else if ((*((PDWORD)pItem)) == FREE_BLOCK)
        {            
            WriteFlash(pSource, sizeof(DWORD), nAddr);
            return pItem;
        }
        nAddr++;
    }
    // No free block found, swap the sections
    nAddr = SwapSections();
    WriteFlash(pSource, sizeof(PARAMITEM), nAddr);
    return (PPARAMITEM)FlashToMemory(nAddr);
}

/*------------------------------------------------------------------------------- 
Function: INTERNAL PPARAMITEM SearchItem(WORD nParam);

Purpose: Search some item

Parameters: nParam

Return: NULL 

Comments: None

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/ 
INTERNAL PPARAMITEM SearchItem(WORD nParam)
{
    WORD nAddr = g_nActiveSection + 1;
    PPARAMITEM pItem;
    while (nAddr < g_nActiveSection + SECTION_SIZE)
    {
        pItem = (PPARAMITEM)FlashToMemory(nAddr);
        if (pItem->nParam == nParam)
		{
            return pItem;		
		}
        nAddr++;
    }	
    return NULL;
}

/*------------------------------------------------------------------------------- 
Function: INTERNAL VOID FormatSection(WORD nAddr)

Purpose: Write section header

Parameters: nAddr

Return: None 

Comments: None

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/ 
INTERNAL VOID FormatSection(WORD nAddr)
{
    DWORD dwHeader = ACTIVE_FLAG;
    // Erase section
    EraseFlash(nAddr >> (FLASH_PAGE_FACTOR - 2), 1);
    // Write section header
    WriteFlash(&dwHeader, sizeof(DWORD), nAddr);
}

/*------------------------------------------------------------------------------- 
Function: INTERNAL WORD SwapSections()

Purpose: Swap Sections

Parameters: None

Return: nAddr 

Comments: None

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/ 
INTERNAL WORD SwapSections()
{
    WORD nReserved;
    PARAMITEM stItem;
    WORD nAddr;
    DWORD dwFlag = UNUSED_FLAG;
    
    if (g_nActiveSection == LOW_SECTION)
    {
        FormatSection(HIGH_SECTION);
        nReserved = HIGH_SECTION;
    }
    else
    {
        FormatSection(LOW_SECTION);
        nReserved = LOW_SECTION;
    }
    
    nAddr = nReserved + 1;
    
    for (BYTE nIndex = 0; nIndex < g_nParamCount; nIndex++)
    {
        if (g_pParamItems[nIndex]->nParam != ZERO_ITEM)
        {
            stItem = *g_pParamItems[nIndex]; // The item must be read to RAM before writing to flash
            WriteFlash(&stItem, sizeof(PARAMITEM), nAddr);
			g_pParamItems[nIndex] = (PPARAMITEM)FlashToMemory(nAddr);
            nAddr++;
        }
    }
    
    WriteFlash(&dwFlag, sizeof(DWORD), g_nActiveSection);
    g_nActiveSection = nReserved;
    return nAddr;	
}
