#ifndef __RGE_SCREEN_H__
#define __RGE_SCREEN_H__

/* ����ͷ�ļ� */
#include "bmp.h"

/* �������� */
void FFRPG_WIN_INIT(HINSTANCE hInst); /* ��ʼ�� */
void FFRPG_MSG_LOOP(void); /* ��Ϣѭ�� */
HINSTANCE GET_APP_INSTANCE(void); /* ��ȡ FFRPG APP ��ʵ����� */
HWND      GET_SCREEN_HWND (void); /* ��ȡ SCREEN �Ĵ��ھ�� */
void UPDATE_SCREEN(BMP *pb, int x, int y, int h, int w);

/* WINSCREEN Ĭ�ϵĴ�����Ϣ������ */
LRESULT CALLBACK DEF_SCREEN_WNDPROC(
    HWND hwnd,      /* handle to window */
    UINT uMsg,      /* message identifier */
    WPARAM wParam,  /* first message parameter */
    LPARAM lParam   /* second message parameter */
);

/* ȫ�ֱ������� */
extern BMP SCREEN;

#endif
