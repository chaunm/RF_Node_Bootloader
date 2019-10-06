#ifndef __TYPES_H__
#define __TYPES_H__

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef BYTE BOOL;
typedef unsigned char CHAR;
typedef void VOID;

#define CHECK_BIT(addr, bit) (((addr) & ((DWORD)1 << (bit))) == ((DWORD)1 << (bit)))

typedef BYTE* PBYTE;
typedef WORD* PWORD;
typedef DWORD* PDWORD;
typedef UINT* PUINT;
typedef CHAR* PCHAR;
typedef VOID* PVOID;

typedef const BYTE* PCBYTE;

typedef BYTE RESULT;

typedef struct tagTIME_TypeDef
{
    BYTE nSecondCount;
    BYTE nMinuteCount;
    BYTE nHourCount;
    BYTE nDayWeek;
    BYTE nDay;
    BYTE nMonth;
    BYTE nYear;
} TIME_TypeDef, *PTIME_TypeDef;


#define FALSE   0
#define TRUE    1
#define NULL	0
#define MAX_WORD 0xFFFF

#define RESULT_SUCCESS	0
#define RESULT_FAILED   1

#define ON	1
#define OFF 0

typedef VOID (*SYSTEMCALLBACK)(PVOID);


#define INTERNAL static
#define EXTERNAL extern
#define INLINE inline

#endif // __TYPES_H__
