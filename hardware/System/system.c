/********************************************************************************

Product: SHTMS

Module: system.c

Version: 1.0

Author: NGUYEN KIM THANH

Created: 08-2014

Modified:
<Name>
<Date>
<Change>

Released:

Description: Including functions for system

Note: <Note>

********************************************************************************/

/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/
//#include "hw_remap.h"
#include "STM32f0xx_iwdg.h"
#include "system.h"
/*-----------------------------------------------------------------------------*/
/* Constant definitions  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Macro definitions  */
/*-----------------------------------------------------------------------------*/
#ifdef INTERRUPT_VECTOR_RAM
#define VECTOR_INTERRUPT_COUNT	48
#endif //INTERRUPT_VECTOR_RAM
/*-----------------------------------------------------------------------------*/
/* Data type definitions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Global variables  */
/*-----------------------------------------------------------------------------*/
// Vector table at the beginning of RAM
#ifdef INTERRUPT_VECTOR_RAM
#pragma location = 0x20000000
__no_init __IO DWORD VectorTable[VECTOR_INTERRUPT_COUNT];
#endif //INTERRUPT_VECTOR_RAM

INTERNAL volatile WORD g_nTickCount = 0;
INTERNAL volatile WORD g_nSecondCount = 0;
INTERNAL volatile WORD g_nTemp = 0;

INTERNAL SYSTEMCALLBACK g_pTickCallback[TICK_INTERRUPTS_COUNT] = { NULL };
/*-----------------------------------------------------------------------------*/
/* Function prototypes  */
/*-----------------------------------------------------------------------------*/
INTERNAL VOID SysTickConfig (VOID);

/*-------------------------------------------------------------------------------
Function:  InitSystem(VOID)

Purpose: Init neccessary hardware for running this platform.

Parameters: None

Return: None

Comments:  None

Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
VOID InitSystem(VOID)
{
  // chaunm - check xem co can move vector sang RAM hay khong
#ifdef INTERRUPT_VECTOR_RAM
	__disable_irq();
    // Copy vector table from flash to the base addr of sram at 0x20000000
    for (BYTE nIndex = 0; nIndex < VECTOR_INTERRUPT_COUNT; nIndex++)
    {
        VectorTable[nIndex] = *(__IO PDWORD)(APPLICATION_ADDRESS + (nIndex << 2));
    }
    // Relocate the vector table to internal SRAM at 0x20000000
	// enable clock
	RCC->APB2ENR |= (DWORD)0x01;
    SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
#endif // RELEASE
	__enable_irq();
    //Init clock and hardware
    SysTickConfig();
}
/*-------------------------------------------------------------------------------
Function:  GetTickCount()

Purpose: get tick count

Parameters: None

Return: WORD g_nTickCount

Comments:  None

Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
WORD GetTickCount()
{
    return g_nTickCount;
}
/*-------------------------------------------------------------------------------
Function:  GetSecondCount()

Purpose: get second count

Parameters: None

Return: WORD:g_nSecondCount

Comments:  None

Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
WORD GetSecondCount()
{
    return g_nSecondCount;
}
/*-------------------------------------------------------------------------------
Function: VOID Reboot(VOID);

Purpose: Reboot device

Parameters: None

Return: None

Comments: Reboot device

Modified:
<Modified by> Locdv
<Date> 18/04/2015
<Change> Jump_To_IAP thanh fnJumpToIAP
--------------------------------------------------------------------------------*/
VOID Reboot(PVOID pParam)
{
    NVIC_SystemReset();
	/* chau nguyen - need to check
    SYSTEMCALLBACK fnJumpToIAP;
    DWORD nJumpAddress = 0x00;

    nJumpAddress = *(__IO PDWORD)(BOOTLOADER_ADDRESS + 4);
    fnJumpToIAP = (SYSTEMCALLBACK) nJumpAddress;

    __set_MSP(*(__IO PDWORD) APPLICATION_ADDRESS);  //Initialize stack Pointer
    fnJumpToIAP(NULL); //go to IAP now
*/
}
/*-------------------------------------------------------------------------------
Function: VOID ActivateWatchdog();

Purpose: Initial Watchdog

Parameters: byBCD

Return: (byLow + byHigh)

Comments:  Watchdog 6.5s

Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/

VOID ActivateWatchdog()
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_64);
    IWDG_SetReload(0x0FFF);
    IWDG_ReloadCounter();
    IWDG_Enable();
}

/*-------------------------------------------------------------------------------
Function: VOID ResetWatchdog();

Purpose: reset Watchdog

Parameters: byBCD

Return: (byLow + byHigh)

Comments:  Watchdog 6.5s

Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
VOID ResetWatchdog()
{
	IWDG_ReloadCounter();
}
/*******************************************************************************
Function:VOID RegisterTickCallback;

Purpose: 

Parameters: None

Return: None

Comments: None

Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/ 
VOID RegisterTickCallback(BYTE nEvent, SYSTEMCALLBACK fnCallback)
{
    if (nEvent < TICK_INTERRUPTS_COUNT)
    {
        g_pTickCallback[nEvent] = fnCallback;
    }
}
/*******************************************************************************
Function:VOID SysTick_Handler(VOID);

Purpose: SysTick_Handler

Parameters: None

Return: None

Comments: None

Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
VOID SysTick_Handler(VOID)
{
    g_nTickCount++;
	g_nTemp++;
	if (g_nTemp == 1000)
	{
		g_nSecondCount++;
		g_nTemp = 0;
	}
    
    if (g_pTickCallback[TICK_INTERRUPTS_EVENT] != NULL)
    {
        g_pTickCallback[TICK_INTERRUPTS_EVENT](NULL);
    }
}


/*-------------------------------------------------------------------------------
Function:  VOID SysTickConfig (VOID)

Purpose: Config Systick

Parameters: VOID

Return: None

Comments:  None

Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
INTERNAL VOID SysTickConfig (VOID)
{
    while (SysTick_Config(SystemCoreClock / 1000));
}