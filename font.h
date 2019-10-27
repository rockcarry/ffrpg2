#ifndef __FONT_H__
#define __FONT_H__

/* 包含头文件 */
#include <stdio.h>
#include "bmp.h"

/* 字体类型定义 */
typedef struct {
    BYTE  hzk_width;     /* 点阵汉字的宽度 */
    BYTE  hzk_height;    /* 点阵汉字的高度 */
    BYTE  asc_width;     /* 点阵英文字的宽度 */
    BYTE  asc_height;    /* 点阵英文字的高度 */
    char *hzk_file;      /* 汉字库文件名 */
    char *asc_file;      /* 英文字库文件名 */
    BYTE *hzk_data;
    BYTE *asc_data;
    int  _hzk_buf_size; /* 汉字字模大小 */
    int  _asc_buf_size; /* 英文字模大小 */
} FONT;

/* 文本框类型定义 */
typedef struct {
    FONT *font;
    int   color;
    int   xpos;       /* 文本框的位置 x 坐标 */
    int   ypos;       /* 文本框的位置 y 坐标 */
    int   width;      /* 文本框的宽度 */
    int   height;     /* 文本框的高度 */
    int   colspace;   /* 文本的列间距 */
    int   rowspace;   /* 文本的行间距 */
    char *text;       /* 文本框的文本 */
    int   cursor;     /* 文本游标 */
    int  _cur_xpos;   /* 当前的 x 坐标 */
    int  _cur_ypos;   /* 当前的 y 坐标 */
} TEXTBOX;

/* 预定义字体 */
extern FONT FONT12;
extern FONT FONT16;
extern FONT FONT24;

BOOL loadfont(FONT *pf);
void freefont(FONT *pf);

/* TEXTBOX 相关函数 */
void resettextbox(TEXTBOX *ptb);
int  printtextbox(BMP *pb, TEXTBOX *ptb);

/* 文本输出函数 */
void draw_asc (BMP *pb, int x, int y, int c, FONT *pf, char asc  );
void draw_hz  (BMP *pb, int x, int y, int c, FONT *pf, char hz[2]);
int  outtextxy(BMP *pb, int x, int y, int c, FONT *pf, char *txt);

#endif

