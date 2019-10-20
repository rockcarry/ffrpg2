#ifndef _TEST_

/* 包含头文件 */
#include "bitblt.h"

/* 内部函数实现 */
/* 位块传送的区域裁剪函数 */
static BOOL bltclip(BMP *dstpb, int *dstx, int *dsty,
                    BMP *srcpb, int *srcx, int *srcy,
                    int *w, int *h)
{
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
    if (*dstx >= dstpb->width || *dsty >= dstpb->height) return FALSE;
    if (*dstx < 0) {
        *w    += *dstx;
        *srcx -= *dstx;
        *dstx  = 0;
    }
    if (*dsty < 0) {
        *h    += *dsty;
        *srcy -= *dsty;
        *dsty  = 0;
    }

    if (*w < 0 || *h < 0) return FALSE;
    if (*dstx + *w > dstpb->width ) *w = dstpb->width  - *dstx;
    if (*dsty + *h > dstpb->height) *h = dstpb->height - *dsty;
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
    do {
        memcpy(dst, src, srcw * sizeof(DWORD));
        src += srcpb->width;
        dst += dstpb->width;
    } while (--srch);
}

#else
/* 包含头文件 */
#include "screen.h"
#include "bitblt.h"
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    BMP mybmp = {0};

    if (*lpszCmdLine == 0) {
        lpszCmdLine = "res/boy.bmp";
    }

    FFRPG_WIN_INIT(hInst);
    createbmp(&SCREEN);
    loadbmp(&mybmp, lpszCmdLine);
    putbmp (&SCREEN, (SCREEN.width  - mybmp.width ) / 2, (SCREEN.height - mybmp.height) / 2, &mybmp);
    UPDATE_SCREEN(&SCREEN, 0, 0, 640, 480);

    FFRPG_MSG_LOOP();
    destroybmp(&mybmp );
    destroybmp(&SCREEN);
    return 0;
}
#endif



