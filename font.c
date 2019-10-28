#ifndef _TEST_

/* 包含头文件 */
#include <stdlib.h>
#include "draw2d.h"
#include "font.h"
#include "utils.h"

/* 预定义字体 */
FONT FONT12 = { 12, 12,  6, 12, "font/hzk1212.dat", "font/asc0612.dat" };
FONT FONT16 = { 16, 16,  8, 14, "font/hzk1616.dat", "font/asc0814.dat" };
FONT FONT24 = { 24, 24, 12, 22, "font/hzk2424.dat", "font/asc1222.dat" };

/* 内部函数实现 */
/* draw raster font */
static void draw_raster_font(BMP *pb, int x, int y, int c, BYTE *data, int w, int h)
{
    int i, j;

    /* w 对齐到 8 */
    w = (w + 7) / 8 * 8;

    /* 绘制字模 */
    for (; h>0; h--,y++,x-=w) {
        for (i=0; i<w/8; i++,data++) {
            for (j=7; j>=0; j--,x++) {
                if (*data & (1 << j)) {
                    pixel(pb, x, y, c);
                }
            }
        }
    }
}

/* 输出一个汉字的函数 */
void draw_hz(BMP *pb, int x, int y, int c, FONT *pf, char hz[2])
{
    int ch0 = (BYTE)hz[0] - 0xA0;
    int ch1 = (BYTE)hz[1] - 0xA0;
    int offset = pf->_hzk_buf_size * ((ch0 - 1L) * 94 + (ch1 - 1L));
    draw_raster_font(pb, x, y, c, pf->hzk_data + offset, pf->hzk_width, pf->hzk_height);
}

/* 输出一个英文字符的函数 */
void draw_asc(BMP *pb, int x, int y, int c, FONT *pf, char asc)
{
    int offset = asc * pf->_asc_buf_size;
    draw_raster_font(pb, x, y, c, pf->asc_data + offset, pf->asc_width, pf->asc_height);
}

BOOL loadfont(FONT *pf)
{
    if (!pf) return FALSE;
    pf->_hzk_buf_size = (pf->hzk_width + 7) / 8 * pf->hzk_height;
    pf->_asc_buf_size = (pf->asc_width + 7) / 8 * pf->asc_height;
    pf->hzk_data = (BYTE*)read_file(pf->hzk_file, NULL);
    pf->asc_data = (BYTE*)read_file(pf->asc_file, NULL);
    return TRUE;
}

void freefont(FONT *pf)
{
    if (!pf) return;
    if (pf->hzk_data) {
        free(pf->hzk_data);
        pf->hzk_data = NULL;
    }
    if (pf->asc_data) {
        free(pf->asc_data);
        pf->asc_data = NULL;
    }
}

/* TEXTBOX 相关函数 */
void resettextbox(TEXTBOX *ptb)
{
    if (!ptb) return;
    ptb->_cur_xpos = ptb->xpos;
    ptb->_cur_ypos = ptb->ypos;
}

static int textbox_print_1step(BMP *pb, TEXTBOX *ptb)
{
    FONT *pf = ptb->font;
    int fhw, fhh, faw, fah;

    if (!pb || !ptb) return 1;
    fhw = pf->hzk_width;
    fhh = pf->hzk_height;
    faw = pf->asc_width;
    fah = pf->asc_height;

    /* 对不同的文字进行分别处理 */
    switch (ptb->text[ptb->cursor]) {
    case '\0':  /* 字符串结束符 */
        ptb->cursor = 0;
        return 0; /* text box end */

    case '\n':  /* 换行符 */
        ptb->_cur_xpos  = ptb->xpos;
        ptb->_cur_ypos += fhh + ptb->rowspace;
        ptb->cursor++;
        break;

    default:
        if (ptb->text[ptb->cursor] & 0x80) {
            draw_hz(pb, ptb->_cur_xpos, ptb->_cur_ypos, ptb->color, ptb->font, &(ptb->text[ptb->cursor]));
            ptb->_cur_xpos += fhw;
            ptb->cursor++;
            ptb->cursor++;
        } else {
            draw_asc(pb, ptb->_cur_xpos, ptb->_cur_ypos + fhh - fah, ptb->color, ptb->font, ptb->text[ptb->cursor]);
            ptb->_cur_xpos += faw;
            ptb->cursor++;
        }
        ptb->_cur_xpos += ptb->colspace;
        break;
    }

    if (  ptb->_cur_xpos + fhw > ptb->xpos + ptb->width
       && ptb->text[ptb->cursor] != '\n')
    {
        ptb->_cur_xpos  = ptb->xpos;
        ptb->_cur_ypos += fhh + ptb->rowspace;
    }

    if (ptb->_cur_ypos + fhh > ptb->ypos + ptb->height) {
        ptb->_cur_xpos = ptb->xpos;
        ptb->_cur_ypos = ptb->ypos;
        return -1; /* text box full */
    } else {
        return  1;
    }
}

int printtextbox(BMP *pb, TEXTBOX *ptb)
{
    int ret;
    do { ret = textbox_print_1step(pb, ptb); } while (ret > 0);
    return ret;
}

/* 文本输出函数 */
int outtextxy(BMP *pb, int x, int y, int c, FONT *pf, char *txt)
{
    TEXTBOX tb = { pf, c };
    if (!pb || !txt) return 1;
    tb.width     = pb->width;
    tb.height    = pb->height;
    tb.rowspace  = 1;
    tb.colspace  = 1;
    tb.text      = txt;
    tb.xpos      = x;
    tb.ypos      = y;
    tb._cur_xpos = x;
    tb._cur_ypos = y;
    return printtextbox(pb, &tb);
}

#else
#include "screen.h"
#include "font.h"

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    FFRPG_WIN_INIT(hInst, NULL);
    loadfont(&FONT16);
    createbmp(&SCREEN);
    outtextxy(&SCREEN, 10, 10, RGB(0, 255, 0), &FONT16, "Hello FFRPG !\n\n汉字显示\n");
    UPDATE_SCREEN(&SCREEN, 0, 0, 0, 0, FALSE);

    FFRPG_MSG_LOOP();
    destroybmp(&SCREEN);
    freefont(&FONT16);
    return 0;
}
#endif

