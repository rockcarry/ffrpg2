#ifndef _TEST_

/* ����ͷ�ļ� */
#include <stdlib.h>
#include "draw2d.h"
#include "font.h"

/* Ԥ�������� */
FONT FONT12 = { 12, 12,  6, 12, "font/hzk1212.dat", "font/asc0612.dat" };
FONT FONT16 = { 16, 16,  8, 14, "font/hzk1616.dat", "font/asc0814.dat" };
FONT FONT24 = { 24, 24, 12, 22, "font/hzk2424.dat", "font/asc1222.dat" };

/* �ڲ�����ʵ�� */
/* draw raster font */
static void draw_raster_font(BMP *pb, int x, int y, int c, BYTE *data, int w, int h)
{
    int i, j;

    /* w ���뵽 8 */
    w = (w + 7) / 8 * 8;

    /* ������ģ */
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

/* ���һ�����ֵĺ��� */
void draw_hz(BMP *pb, int x, int y, int c, FONT *pf, char hz[2])
{
    int ch0 = (BYTE)hz[0] - 0xA0;
    int ch1 = (BYTE)hz[1] - 0xA0;
    long offset = pf->_hzk_buf_size * ((ch0 - 1L) * 94 + (ch1 - 1L));
    fseek(pf->_hzk_fp, offset, SEEK_SET);
    fread(pf->_font_buf, pf->_hzk_buf_size, 1, pf->_hzk_fp);
    draw_raster_font(pb, x, y, c, pf->_font_buf, pf->hzk_width, pf->hzk_height);
}

/* ���һ��Ӣ���ַ��ĺ��� */
void draw_asc(BMP *pb, int x, int y, int c, FONT *pf, char asc)
{
    long offset = (long)asc * pf->_asc_buf_size;
    fseek(pf->_asc_fp, offset, SEEK_SET);
    fread(pf->_font_buf, pf->_asc_buf_size, 1, pf->_asc_fp);
    draw_raster_font(pb, x, y, c, pf->_font_buf, pf->asc_width, pf->asc_height);
}

BOOL loadfont(FONT *pf)
{
    int bufsize;

    if (!pf) return FALSE;

    /* ������������Ҫ�Ļ�������С */
    pf->_hzk_buf_size = (pf->hzk_width + 7) / 8 * pf->hzk_height;
    pf->_asc_buf_size = (pf->asc_width + 7) / 8 * pf->asc_height;
    bufsize = max(pf->_hzk_buf_size, pf->_asc_buf_size);

    /* �������ֿ��ļ� */
    pf->_hzk_fp = fopen(pf->hzk_file, "rb");
    if (!pf->_hzk_fp) return FALSE;

    /* ��Ӣ���ֿ��ļ� */
    pf->_asc_fp = fopen(pf->asc_file, "rb");
    if (!pf->_asc_fp) return FALSE;

    /* ������������Ҫ�Ļ����� */
    pf->_font_buf = (BYTE*)malloc(bufsize);
    if (pf->_font_buf) return FALSE;

    return TRUE;
}

void freefont(FONT *pf)
{
    if (!pf) return;

    if (pf->_hzk_fp) {
        fclose(pf->_hzk_fp);
        pf->_hzk_fp = NULL;
    }

    if (pf->_asc_fp) {
        fclose(pf->_asc_fp);
        pf->_asc_fp = NULL;
    }

    if (pf->_font_buf) {
        free(pf->_font_buf);
        pf->_font_buf = NULL;
    }
}

/* TEXTBOX ��غ��� */
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

    /* �Բ�ͬ�����ֽ��зֱ��� */
    switch (ptb->text[ptb->cursor]) {
    case '\0':  /* �ַ��������� */
        ptb->cursor = 0;
        return 0; /* text box end */

    case '\n':  /* ���з� */
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

/* �ı�������� */
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
    outtextxy(&SCREEN, 10, 10, RGB(0, 255, 0), &FONT16, "Hello FFRPG !\n\n������ʾ\n");
    UPDATE_SCREEN(&SCREEN, 0, 0, 0, 0, FALSE);

    FFRPG_MSG_LOOP();
    destroybmp(&SCREEN);
    freefont(&FONT16);
    return 0;
}
#endif
