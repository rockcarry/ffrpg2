#ifndef __BMP_H__
#define __BMP_H__

/* ����ͷ�ļ� */
#include "stdefine.h"

/* ���Ͷ��� */
/* BMP ��������Ͷ��� */
typedef struct {
    int    width;      /* ��� */
    int    height;     /* �߶� */
    DWORD *pdata;      /* ���� */
    void  *pextra;

    BOOL (*createbmp )(void *pb);
    void (*destroybmp)(void *pb);
} BMP;

/* �������� */
/* ���������� */
BOOL createbmp (BMP *pb);
void destroybmp(BMP *pb);

BOOL loadbmp(BMP *pb, char *file);
BOOL savebmp(BMP *pb, char *file);

#endif


