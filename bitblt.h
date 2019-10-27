#ifndef __BITBLT_H__
#define __BITBLT_H__

/* 包含头文件 */
#include "bmp.h"

#define putbmp(dstpb, x, y, srcpb)  bitblt(dstpb, x, y, NULL, srcpb, 0, 0, -1, -1)

/* 函数声明 */
void bitblt(BMP *dstpb, int dstx, int dsty, RECT *clipper, BMP *srcpb, int srcx, int srcy, int srcw, int srch);
            
#endif

