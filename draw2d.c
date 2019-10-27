#ifndef _TEST_

/* 包含头文件 */
#include "draw2d.h"

/* 内部函数实现 */
/* 位块传送的区域裁剪函数 */
static BOOL bltclip(BMP *dstpb, int *dstx, int *dsty,
                    BMP *srcpb, int *srcx, int *srcy, int *w, int *h)
{
    int dst_clip_l = dstpb->clipper.left;
    int dst_clip_t = dstpb->clipper.top;
    int dst_clip_r = dstpb->clipper.right;
    int dst_clip_b = dstpb->clipper.bottom;
    if (dst_clip_l < 0) dst_clip_l = 0;
    if (dst_clip_t < 0) dst_clip_t = 0;
    if (dst_clip_r > dstpb->width ) dst_clip_r = dstpb->width - 1;
    if (dst_clip_b > dstpb->height) dst_clip_b = dstpb->height- 1;

    /* 对源图进行裁剪 */
    if (*srcx >= srcpb->width || *srcy >= srcpb->height) return FALSE;
    if (*srcx < 0) {
        *w    += *srcx;
        *dstx -= *srcx;
        *srcx  = 0;
    }
    if (*srcy < 0) {
        *h    += *srcy;
        *dsty -= *srcy;
        *srcy  = 0;
    }

    if (*srcx + *w > srcpb->width ) *w = srcpb->width  - *srcx;
    if (*srcy + *h > srcpb->height) *h = srcpb->height - *srcy;

    /* 对目的图进行裁剪 */
    if (*dstx > dst_clip_r || *dsty > dst_clip_b) return FALSE;
    if (*dstx < dst_clip_l) {
        *w    -= dst_clip_l - *dstx;
        *srcx += dst_clip_l - *dstx;
        *dstx  = dst_clip_l;
    }
    if (*dsty < dst_clip_t) {
        *h    -= dst_clip_t - *dsty;
        *srcy += dst_clip_t - *dsty;
        *dsty  = dst_clip_t;
    }

    if (*dstx + *w > dst_clip_r + 1) *w = dst_clip_r + 1 - *dstx;
    if (*dsty + *h > dst_clip_b + 1) *h = dst_clip_b + 1 - *dsty;
    if (*w < 0 || *h < 0) return FALSE;
    return TRUE;
}

/* 函数实现 */
void bitblt(BMP *dstpb, int dstx, int dsty, BMP *srcpb, int srcx, int srcy, int srcw, int srch)
{
    DWORD *dst, *src;
    if (srcw == -1) srcw = srcpb->width ;
    if (srch == -1) srch = srcpb->height;
    if (!bltclip(dstpb, &dstx, &dsty, srcpb, &srcx, &srcy, &srcw, &srch)) return;
    src = srcpb->pdata + srcy * srcpb->width + srcx;
    dst = dstpb->pdata + dsty * dstpb->width + dstx;
    while (srch--) {
        memcpy(dst, src, srcw * sizeof(DWORD));
        src += srcpb->width;
        dst += dstpb->width;
    }
}

void bar(BMP *pb, int x, int y, int w, int h, int color)
{
    DWORD *dst;
    int    i;
    if (w == -1) w = pb->width ;
    if (h == -1) h = pb->height;
    if (x > pb->clipper.right || y > pb->clipper.bottom) return;
    if (x < pb->clipper.left) {
        w-= pb->clipper.left - x;
        x = pb->clipper.left;
    }
    if (y < pb->clipper.top) {
        h-= pb->clipper.top - y;
        y = pb->clipper.top;
    }
    if (x + w > pb->clipper.right + 1) w = pb->clipper.right + 1 - x;
    if (y + h > pb->clipper.bottom+ 1) h = pb->clipper.bottom+ 1 - y;
    if (w < 0 || h < 0) return;
    dst = pb->pdata + y * pb->width + x;
    while (h--) {
        for (i=0; i<w; i++) {
            *dst++ = color;
        }
        dst += pb->width - w;
    }
}

#else
/* 包含头文件 */
#include "screen.h"
#include "draw2d.h"
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    BMP mybmp = {0};

    if (*lpszCmdLine == 0) {
        lpszCmdLine = "res/boy.bmp";
    }

    FFRPG_WIN_INIT(hInst, NULL);
    createbmp(&SCREEN);
    loadbmp(&mybmp, lpszCmdLine);
    putbmp (&SCREEN, (SCREEN.width  - mybmp.width ) / 2, (SCREEN.height - mybmp.height) / 2, &mybmp);
    UPDATE_SCREEN(&SCREEN, 0, 0, 0, 0);

    FFRPG_MSG_LOOP();
    destroybmp(&mybmp );
    destroybmp(&SCREEN);
    return 0;
}
#endif



