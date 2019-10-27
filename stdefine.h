/* ��׼ͷ�ļ� */
#ifndef __STDEFINE_H__
#define __STDEFINE_H__

#if defined(WIN32) || defined(__MINGW32__)
#include <windows.h>

#if !defined(__MINGW32__)
#pragma warning(disable:4996)
#define snprintf _snprintf_c
#endif

#else

/* �������� */
#define TRUE   1
#define FALSE  0
#define __inline

/* ��׼�����Ͷ��� */
typedef int BOOL;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long     int   LONG;

/* �������Ͷ��� */
typedef struct {
    int left;
    int top;
    int right;
    int bottom;
} RECT;

#endif

/* �ú�������������δʹ�õľ��� */
#define DO_USE_VAR(var)  do { var = var; } while (0)

#endif


