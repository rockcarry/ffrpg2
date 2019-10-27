#ifndef _TEST_

/* 包含头文件 */
#include "screen.h"
#include "log.h"

/* 全局变量 */
static HINSTANCE FFRPG_APP_INSTANCE = NULL;

/*
  GDISCREEN 实现
 */

/* 常量定义 */
#define FFRPG_WND_CLASS TEXT("FFRPGWndClass")
#define FFRPG_WND_NAME  TEXT("FFRPG")

/* 类型定义 */
typedef struct {
    HWND    hwnd;
    DWORD   style;
    WNDPROC wndproc;
    HBITMAP hbmp;
    HDC     hdc;
} BMP_EXTRA_WIN;

/* 内部函数声明 */
static BOOL _windrv_createbmp (void *pb);
static void _windrv_destroybmp(void *pb);

static BMP_EXTRA_WIN WINSCREEN_EXTRA =
{
    NULL,
    WS_SYSMENU|WS_MINIMIZEBOX,
    DEF_SCREEN_WNDPROC,
};

BMP SCREEN = {
    0, 0, NULL, &WINSCREEN_EXTRA, {0},
    _windrv_createbmp,
    _windrv_destroybmp,
};

/* 内部函数实现 */
static BOOL _windrv_createbmp(void *pb)
{
    BMP           *pbmp   = (BMP*)pb;
    BMP_EXTRA_WIN *pextra = (BMP_EXTRA_WIN*)pbmp->pextra;
    WNDCLASS wc  = {0};
    RECT cltrect = {0};
    int  x, y, w, h;

    BYTE        buffer[sizeof(BITMAPINFOHEADER)] = {0};
    BITMAPINFO *bmpinfo = (BITMAPINFO*)buffer;

    wc.lpfnWndProc   = pextra->wndproc;
    wc.hInstance     = FFRPG_APP_INSTANCE;
    wc.hIcon         = LoadIcon  (NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wc.lpszClassName = FFRPG_WND_CLASS;
    if (!RegisterClass(&wc)) {
        log_printf("_windrv_createbmp: failed to register window class !\n");
        return FALSE;
    }

    bmpinfo->bmiHeader.biSize        =  sizeof(BITMAPINFOHEADER);
    bmpinfo->bmiHeader.biWidth       =  pbmp->width;
    bmpinfo->bmiHeader.biHeight      = -pbmp->height;
    bmpinfo->bmiHeader.biPlanes      =  1;
    bmpinfo->bmiHeader.biBitCount    =  32;
    bmpinfo->bmiHeader.biCompression =  BI_RGB;

    pextra->hdc  = CreateCompatibleDC(NULL);
    pextra->hbmp = CreateDIBSection(pextra->hdc, bmpinfo, DIB_RGB_COLORS, (void**)&(pbmp->pdata), NULL, 0);
    if (!pextra->hdc || !pextra->hbmp || !pbmp->pdata) {
        log_printf("_windrv_createbmp: failed to create hdc or hbmp !\n");
        return FALSE;
    } else {
        SelectObject(pextra->hdc, pextra->hbmp);
    }

    pextra->hwnd = CreateWindow(FFRPG_WND_CLASS, FFRPG_WND_NAME, pextra->style,
        CW_USEDEFAULT, CW_USEDEFAULT, pbmp->width, pbmp->height,
        NULL, NULL, FFRPG_APP_INSTANCE, NULL);
    if (!pextra->hwnd) {
        log_printf("_windrv_createbmp: failed to create window !\n");
        return FALSE;
    }

    GetClientRect(pextra->hwnd, &cltrect);
    w = pbmp->width  + (pbmp->width  - cltrect.right);
    h = pbmp->height + (pbmp->height - cltrect.bottom);
    x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;
    x = x > 0 ? x : 0;
    y = y > 0 ? y : 0;

    MoveWindow(pextra->hwnd, x, y, w, h, FALSE);
    ShowWindow(pextra->hwnd, SW_SHOW);
    UpdateWindow(pextra->hwnd);
    return TRUE;
}

static void _windrv_destroybmp(void *pb)
{
    BMP           *pbmp   = (BMP*)pb;
    BMP_EXTRA_WIN *pextra = (BMP_EXTRA_WIN*)pbmp->pextra;
    DestroyWindow(pextra->hwnd);

    if (pextra->hdc ) DeleteDC(pextra->hdc);
    if (pextra->hbmp) DeleteObject(pextra->hbmp);

    pextra->hwnd = NULL;
    pextra->hdc  = NULL;
    pextra->hbmp = NULL;
    pbmp->pdata  = NULL;
}

/* 函数实现 */
void FFRPG_WIN_INIT(HINSTANCE hInst, WNDPROC wndproc)
{
    FFRPG_APP_INSTANCE      = hInst;
    WINSCREEN_EXTRA.wndproc = wndproc ? wndproc : DEF_SCREEN_WNDPROC;
}

HINSTANCE GET_APP_INSTANCE(void)
{
    return FFRPG_APP_INSTANCE;
}

void FFRPG_MSG_LOOP(void)
{
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage (&msg);
    }
}

HWND GET_SCREEN_HWND(void)
{
    BMP_EXTRA_WIN *pextra = (BMP_EXTRA_WIN*)SCREEN.pextra;
    return pextra->hwnd;
}

void UPDATE_SCREEN(BMP *pb, int x, int y, int w, int h, BOOL flag)
{
    BMP_EXTRA_WIN *pextra = (BMP_EXTRA_WIN*)pb->pextra;
    RECT rect = { x, y, w ? x + w : pb->width, h ? y + h : pb->height };
    if (flag) {
        HDC hdc = GetDC(pextra->hwnd);
        BitBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
               pextra->hdc, rect.left, rect.top, SRCCOPY);
        ReleaseDC(pextra->hwnd, hdc);
    } else {
        InvalidateRect(pextra->hwnd, x || y || h || w ? &rect : NULL, FALSE);
    }
}

LRESULT CALLBACK DEF_SCREEN_WNDPROC(
    HWND hwnd,      /* handle to window */
    UINT uMsg,      /* message identifier */
    WPARAM wParam,  /* first message parameter */
    LPARAM lParam   /* second message parameter */
)
{
    PAINTSTRUCT ps = {0};
    HDC        hdc = NULL;

    switch (uMsg) {
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        BitBlt(hdc,
            ps.rcPaint.left,
            ps.rcPaint.top,
            ps.rcPaint.right  - ps.rcPaint.left,
            ps.rcPaint.bottom - ps.rcPaint.top,
            ((BMP_EXTRA_WIN*)SCREEN.pextra)->hdc,
            ps.rcPaint.left,
            ps.rcPaint.top,
            SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

#else
#include "screen.h"
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    int  i, j;

    FFRPG_WIN_INIT(hInst, NULL);
    createbmp(&SCREEN);

    for (i=0; i<SCREEN.height; i++) {
        for (j=0; j<SCREEN.width; j++) {
            *(SCREEN.pdata + i * SCREEN.width + j) = RGB(i, j, i);
            if (j == 0 || j == 639 || i == 0 || i == 479) *(SCREEN.pdata + i * SCREEN.width + j) = RGB(0, 255, 0);
        }
    }
    UPDATE_SCREEN(&SCREEN, 0, 0, 640, 480, FALSE);

    FFRPG_MSG_LOOP();
    destroybmp(&SCREEN);
    return 0;
}
#endif


