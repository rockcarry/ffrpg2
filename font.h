#ifndef __FONT_H__
#define __FONT_H__

/* ����ͷ�ļ� */
#include <stdio.h>
#include "bmp.h"

/* �������Ͷ��� */
typedef struct {
    BYTE  hzk_width;     /* �����ֵĿ�� */
    BYTE  hzk_height;    /* �����ֵĸ߶� */
    BYTE  asc_width;     /* ����Ӣ���ֵĿ�� */
    BYTE  asc_height;    /* ����Ӣ���ֵĸ߶� */
    char *hzk_file;      /* ���ֿ��ļ��� */
    char *asc_file;      /* Ӣ���ֿ��ļ��� */
    BYTE *hzk_data;
    BYTE *asc_data;
    int  _hzk_buf_size; /* ������ģ��С */
    int  _asc_buf_size; /* Ӣ����ģ��С */
} FONT;

/* �ı������Ͷ��� */
typedef struct {
    FONT *font;
    int   color;
    int   xpos;       /* �ı����λ�� x ���� */
    int   ypos;       /* �ı����λ�� y ���� */
    int   width;      /* �ı���Ŀ�� */
    int   height;     /* �ı���ĸ߶� */
    int   colspace;   /* �ı����м�� */
    int   rowspace;   /* �ı����м�� */
    char *text;       /* �ı�����ı� */
    int   cursor;     /* �ı��α� */
    int  _cur_xpos;   /* ��ǰ�� x ���� */
    int  _cur_ypos;   /* ��ǰ�� y ���� */
} TEXTBOX;

/* Ԥ�������� */
extern FONT FONT12;
extern FONT FONT16;
extern FONT FONT24;

BOOL loadfont(FONT *pf);
void freefont(FONT *pf);

/* TEXTBOX ��غ��� */
void resettextbox(TEXTBOX *ptb);
int  printtextbox(BMP *pb, TEXTBOX *ptb);

/* �ı�������� */
void draw_asc (BMP *pb, int x, int y, int c, FONT *pf, char asc  );
void draw_hz  (BMP *pb, int x, int y, int c, FONT *pf, char hz[2]);
int  outtextxy(BMP *pb, int x, int y, int c, FONT *pf, char *txt);

#endif

