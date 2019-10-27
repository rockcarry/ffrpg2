#ifndef __DRAW2D_H__
#define __DRAW2D_H__

/* ����ͷ�ļ� */
#include "bmp.h"

#define putbmp(dstpb, x, y, srcpb)  bitblt(dstpb, x, y, srcpb, 0, 0, -1, -1)

/* �������� */
void bitblt(BMP *dstpb, int dstx, int dsty, BMP *srcpb, int srcx, int srcy, int srcw, int srch);
void bar(BMP *pb, int x, int y, int w, int h, int color);
void pixel(BMP *pb, int x, int y, int c);

#endif

