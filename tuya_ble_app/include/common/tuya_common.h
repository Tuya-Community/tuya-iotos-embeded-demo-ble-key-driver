/**
 * @file tuya_common.h
 * @author lifan
 * @brief common types and micro define
 * @version 1.0
 * @date 2021-08-31
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_COMMON_H__
#define __TUYA_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/
#ifndef OFF
#define OFF     0
#endif

#ifndef ON
#define ON      1
#endif

#ifndef CLR
#define CLR     0
#endif

#ifndef SET
#define SET     1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef NULL
#define NULL    0
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef CONST
#define CONST const
#endif

#ifndef STATIC
#define STATIC static
#endif

#ifndef SIZEOF
#define SIZEOF sizeof
#endif

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef void VOID_T;
typedef VOID_T *PVOID_T;
typedef float FLOAT_T;
typedef FLOAT_T *PFLOAT_T;
typedef char CHAR_T;
typedef CHAR_T *PCHAR_T;
typedef signed char SCHAR_T;
typedef SCHAR_T *PSCHAR_T;
typedef unsigned char UCHAR_T;
typedef UCHAR_T *PUCHAR_T;
typedef short SHORT_T;
typedef unsigned short USHORT_T;
typedef short *PSHORT_T;
typedef signed int INT_T;
typedef INT_T *PINT_T;
typedef unsigned int UINT_T;
typedef UINT_T *PUINT_T;
typedef long LONG_T;
typedef LONG_T *PLONG_T;
typedef unsigned long ULONG_T;
typedef ULONG_T *PULONG_T;
typedef unsigned char BYTE_T;
typedef BYTE_T *PBYTE_T;
typedef unsigned short WORD_T;
typedef WORD_T *PWORD_T;
typedef unsigned int DWORD_T;
typedef DWORD_T *PDWORD_T;
typedef long long DLONG_T;
typedef DLONG_T *PDLONG_T;
typedef unsigned long long UDLONG_T;
typedef UDLONG_T *PUDLONG_T;
typedef int BOOL_T;
typedef BOOL_T *PBOOL_T;

typedef struct {
    UCHAR_T bit0 : 1;
    UCHAR_T bit1 : 1;
    UCHAR_T bit2 : 1;
    UCHAR_T bit3 : 1;
    UCHAR_T bit4 : 1;
    UCHAR_T bit5 : 1;
    UCHAR_T bit6 : 1;
    UCHAR_T bit7 : 1;
} FLAG_BIT;

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_COMMON_H__ */
