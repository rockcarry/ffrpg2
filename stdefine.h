/* 标准头文件 */
#ifndef __STDEFINE_H__
#define __STDEFINE_H__

#if defined(WIN32) || defined(__MINGW32__)
#include <windows.h>

#if !defined(__MINGW32__)
#pragma warning(disable:4996)
#define snprintf _snprintf_c
#endif

#else

/* 常量定义 */
#define TRUE   1
#define FALSE  0
#define __inline

/* 标准的类型定义 */
typedef int BOOL;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long     int   LONG;

/* 矩形类型定义 */
typedef struct {
    int left;
    int top;
    int right;
    int bottom;
} RECT;

#endif

/* 该宏用于消除变量未使用的警告 */
#define DO_USE_VAR(var)  do { var = var; } while (0)

#endif


