#ifndef __BITBLT_H__
#define __BITBLT_H__

/* ����ͷ�ļ� */
#include "bmp.h"

#define putbmp(dstpb, x, y, srcpb)  bitblt(dstpb, x, y, srcpb, 0, 0, -1, -1)

/* �������� */
void bitblt(BMP *dstpb, int dstx, int dsty, BMP *srcpb, int srcx, int srcy, int srcw, int srch);
            
#endif

