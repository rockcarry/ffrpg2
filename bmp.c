#ifndef _TEST_

/* 包含头文件 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bmp.h"
#include "log.h"

/* 常量定义 */
/* 默认位图像素位深度 */
#define DEF_BMP_WIDTH   640
#define DEF_BMP_HEIGHT  480

/* 函数实现 */
/* 创建一个 BMP 对象的函数 */
BOOL createbmp(BMP *pb)
{
    if (!pb) return FALSE;
    if (pb->width  == 0) pb->width  = DEF_BMP_WIDTH;
    if (pb->height == 0) pb->height = DEF_BMP_HEIGHT;
    if (pb->createbmp) {
        return pb->createbmp(pb);
    } else {
        pb->pdata = malloc(pb->width * pb->height * sizeof(DWORD));
        return pb->pdata ? TRUE : FALSE;
    }
}

/* 销毁一个 BMP 对象 */
void destroybmp(BMP *pb)
{
    if (!pb) return;
    if (pb->destroybmp) {
        pb->destroybmp(pb);
    }
    if (pb->pdata) {
        free(pb->pdata);
        pb->pdata = NULL;
    }
}

/* 内部类型定义 */
/* BMP 文件结构类型定义 */
#pragma pack(1)
typedef struct {
    WORD   filetype;
    DWORD  filesize;
    DWORD  reserved;
    DWORD  dataoffset;
    DWORD  infosize;
    LONG   imagewidth;
    LONG   imageheight;
    WORD   planes;
    WORD   bitsperpixel;
    DWORD  compression;
    DWORD  imagesize;
    LONG   xpixelpermeter;
    LONG   ypixelpermeter;
    DWORD  colorused;
    DWORD  colorimportant;
} BMPFILE;
#pragma pack()

/* 函数实现 */
/* 装载 BMP 文件到 BMP 对象 */
BOOL loadbmp(BMP *pb, char *file)
{
    BMPFILE bmpfile;
    FILE   *fp;
    DWORD  *pdst;
    DWORD   pal[256];
    int     i, j, n;

    if (!pb) return FALSE;

    /* 打开文件 */
    fp = fopen(file, "rb");
    if (!fp) {
        log_printf("loadbmp: failed to open file %s  !\n", file);
        return FALSE;
    }

    /* 读取文件头 */
    fread(&bmpfile, sizeof(bmpfile), 1, fp);
    if (bmpfile.filetype != (('B' << 0) | ('M' << 8))) {
        log_printf("loadbmp: file is not bmp file !\n");
        return FALSE;
    }
    pb->width  = (int)bmpfile.imagewidth;
    pb->height = (int)bmpfile.imageheight;

    /* 目前只支持 256 色、16/24/32bit 色的 BMP 图像 */
    if (  bmpfile.bitsperpixel != 8
       && bmpfile.bitsperpixel != 16
       && bmpfile.bitsperpixel != 24
       && bmpfile.bitsperpixel != 32)
    {
        log_printf("loadbmp: only 8/16/24/32 bit bmp files are supported !\n");
        return FALSE;
    }

    /* 创建 BMP 对象 */
    if (!createbmp(pb)) {
        log_printf("loadbmp: failed to createbmp !\n");
        return FALSE;
    }

    if (bmpfile.bitsperpixel == 8) {
        fseek(fp, bmpfile.dataoffset - 256 * 4, SEEK_SET);
        fread(pal, sizeof(pal), 1, fp);
    }

    fseek(fp, bmpfile.dataoffset, SEEK_SET);
    pdst = pb->pdata + pb->width * (pb->height - 1);
    switch (bmpfile.bitsperpixel) {
    case 8:
        for (i=0; i<pb->height; i++) {
            for (j=0; j<pb->width; j++) {
                BYTE c = fgetc(fp);
               *pdst++ = pal[c];
            }
            n = (4 - pb->width % 4) % 4;
            for (j=0; j<n; j++) fgetc(fp);
            pdst -= 2 * pb->width;
        }
        break;
    case 16:
        for (i=0; i<pb->height; i++) {
            for (j=0; j<pb->width; j++) {
                WORD color;
                BYTE r, g, b;
                color  = fgetc(fp) << 0;
                color |= fgetc(fp) << 8;
                r = (color >> 11) & 0x1F;
                g = (color >> 5 ) & 0x3F;
                b = (color >> 0 ) & 0x1F;
                r = (r << 3) | (r >> 2);
                g = (g << 2) | (g >> 4);
                b = (b << 3) | (b >> 2);
               *pdst++ = (r << 16) | (g << 8) | (b << 0);
            }
            n = (4 - (pb->width * 2) % 4) % 4;
            for (j=0; j<n; j++) fgetc(fp);
            pdst -= 2 * pb->width;
        }
        break;
    case 24:
        for (i=0; i<pb->height; i++) {
            for (j=0; j<pb->width; j++) {
                BYTE r, g, b;
                b = fgetc(fp);
                g = fgetc(fp);
                r = fgetc(fp);
               *pdst++ = (r << 16) | (g << 8) | (b << 0);
            }
            n = (4 - (pb->width * 3) % 4) % 4;
            for (j=0; j<n; j++) fgetc(fp);
            pdst -= 2 * pb->width;
        }
        break;
    case 32:
        for (i=0; i<pb->height; i++) {
            fread(pdst, pb->width * sizeof(DWORD), 1, fp);
            pdst -= pb->width;
        }
        break;
    }

    fclose(fp); /* 关闭文件 */
    return TRUE;
}

/* 保存 BMP 对象到 BMP 文件 */
BOOL savebmp(BMP *pb, char *file)
{
    BMPFILE bmpfile = {0};
    FILE   *fp;
    DWORD  *psrc;
    int     i;
    if (!pb || !pb->pdata) return FALSE;

    /* 填充 BMPFILE 结构 */
    bmpfile.filetype     = ('B' << 0) | ('M' << 8);
    bmpfile.dataoffset   = sizeof(bmpfile);
    bmpfile.imagesize    = pb->width * sizeof(DWORD) * pb->height;
    bmpfile.filesize     = bmpfile.dataoffset + bmpfile.imagesize;
    bmpfile.infosize     = 40;
    bmpfile.imagewidth   = pb->width;
    bmpfile.imageheight  = pb->height;
    bmpfile.planes       = 1;
    bmpfile.bitsperpixel = 32;

    /* 打开文件 */
    fp = fopen(file, "wb");
    if (!fp) {
        log_printf("savebmp: failed to open file %s  !\n", file);
        return FALSE;
    }

    /* 写入文件头 */
    fwrite(&bmpfile, sizeof(bmpfile), 1, fp);

    /* 初始化指针 */
    psrc = pb->pdata + pb->width * (pb->height - 1);

    /* 写入图像数据 */
    for (i=0; i<pb->height; i++) {
        fwrite(psrc, pb->width * sizeof(DWORD), 1, fp);
        psrc -= pb->width;
    }

    /* 关闭文件 */
    if (fp) fclose(fp);
    return TRUE;
}

#else
/* 包含头文件 */
#include "bmp.h"
int main(void)
{
    BMP mybmp332  = {};
    BMP mybmp565  = {};
    BMP mybmp888  = {};
    BMP mybmp8888 = {};

    loadbmp(&mybmp332 , "332.bmp" );
    loadbmp(&mybmp565 , "565.bmp" );
    loadbmp(&mybmp888 , "888.bmp" );
    loadbmp(&mybmp8888, "8888.bmp");

    savebmp(&mybmp332 , "332-32.bmp" );
    savebmp(&mybmp565 , "565-32.bmp" );
    savebmp(&mybmp888 , "888-32.bmp" );
    savebmp(&mybmp8888, "8888-32.bmp");

    destroybmp(&mybmp332 );
    destroybmp(&mybmp565 );
    destroybmp(&mybmp888 );
    destroybmp(&mybmp8888);
    return 0;
}
#endif
