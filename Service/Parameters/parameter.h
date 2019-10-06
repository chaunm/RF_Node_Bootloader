/********************************************************************************
Bkav
Product: Platform STM32F051
Module: 
Version: 1.0
Author: ThiVD
Created: 4/2015
Modified: 
<Name>
<Date>
<Change>
Released: <Date>
Description: <Description>
Note: <Note>
********************************************************************************/
#ifndef __PARAMETER_H__
#define __PARAMETER_H__
/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Data type definitions */
/*-----------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct tagPARAMCHANGEEVENT {
	BYTE nEvent;
	BYTE nIndex;
	WORD nParam;
	WORD nValue;
} PARAMCHANGEEVENT, *PPARAMCHANGEEVENT;

#pragma pack(1)
typedef struct tagPARAMITEM {
	WORD nParam;
	WORD nValue;
} PARAMITEM, *PPARAMITEM;
/*-----------------------------------------------------------------------------*/
/* Constant definitions  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Macro definitions  */
/*-----------------------------------------------------------------------------*/

#define MAX_PARAM_ITEM		   40

#define PARAM_DEFINE_EVENT     0
#define PARAM_CHANGE_EVENT     1

#define PARAM_EVENT_COUNT      2
/*-----------------------------------------------------------------------------*/
/* Global variables  */
/*-----------------------------------------------------------------------------*/
EXTERNAL PPARAMITEM PARAM_ITEMS[];
#define PARAMETER(index) (PARAM_ITEMS[(index)]->nValue)
#define DEFINE_PARAM(index, param, value) { PARAM_ITEMS[(index)]->nParam = (param); PARAM_ITEMS[(index)]->nValue = (value); }
/*-----------------------------------------------------------------------------*/
/* Function prototypes  */
/*-----------------------------------------------------------------------------*/
VOID InitParameter(SYSTEMCALLBACK fnDefineProc);
BYTE GetParamCount(VOID);
BOOL SetParam(WORD nParam, WORD nValue);
WORD GetParam(WORD nParam);
VOID ResetParams(VOID);
VOID RegisterParamCallback(BYTE nEvent, SYSTEMCALLBACK fnCallback);
VOID ClearParams();

#endif // __PARAMETER_H__

