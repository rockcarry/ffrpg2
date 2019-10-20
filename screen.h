#ifndef __RGE_SCREEN_H__
#define __RGE_SCREEN_H__

/* 包含头文件 */
#include "bmp.h"

/* 函数声明 */
void FFRPG_WIN_INIT(HINSTANCE hInst); /* 初始化 */
void FFRPG_MSG_LOOP(void); /* 消息循环 */
HINSTANCE GET_APP_INSTANCE(void); /* 获取 FFRPG APP 的实例句柄 */
HWND      GET_SCREEN_HWND (void); /* 获取 SCREEN 的窗口句柄 */
void UPDATE_SCREEN(BMP *pb, int x, int y, int h, int w);

/* WINSCREEN 默认的窗口消息处理函数 */
LRESULT CALLBACK DEF_SCREEN_WNDPROC(
    HWND hwnd,      /* handle to window */
    UINT uMsg,      /* message identifier */
    WPARAM wParam,  /* first message parameter */
    LPARAM lParam   /* second message parameter */
);

/* 全局变量声明 */
extern BMP SCREEN;

#endif
