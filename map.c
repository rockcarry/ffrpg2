#ifndef _TEST_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "map.h"
#include "draw2d.h"

static char* parse_params(const char *str, const char *key, char *val, int len)
{
    char *p = (char*)strstr(str, key);
    int   i;

    if (!p) return NULL;
    p += strlen(key);
    if (*p == '\0') return NULL;

    while (1) {
        if (*p != ' ' && *p != '=' && *p != ':') break;
        else p++;
    }

    for (i=0; i<len; i++) {
        if (*p == '\\') {
            p++;
        } else if (*p == ';' || *p == '\n' || *p == '\0') {
            break;
        }
        val[i] = *p++;
    }
    val[i] = val[len-1] = '\0';
    return val;
}

BOOL loadmap(MAP *map, char *name)
{
    char  path[MAX_PATH], strval[32], *buf;
    int   len, i;
    FILE *fp;

    if (!map || !name) {
        log_printf("loadmap: invalid map or name %p, %p !\n", map, name);
        return FALSE;
    }

    snprintf(path, sizeof(path), "%s/map.ini", name);
    fp = fopen(path, "rb");
    if (!fp) {
        log_printf("loadmap: failed to open file %s !\n", path);
        return FALSE;
    }

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buf = malloc(len);
    if (buf) {
        fread(buf, len, 1, fp);
        parse_params(buf, "cols" , strval, sizeof(strval)); map->cols  = atoi(strval);
        parse_params(buf, "rows" , strval, sizeof(strval)); map->rows  = atoi(strval);
        parse_params(buf, "tilew", strval, sizeof(strval)); map->tilew = atoi(strval);
        parse_params(buf, "tileh", strval, sizeof(strval)); map->tileh = atoi(strval);
        parse_params(buf, "tileb", strval, sizeof(strval));
        snprintf(path, sizeof(path), "%s/%s", name, strval);
        loadbmp(&map->tileb, path);
        free(buf);
    }
    fclose(fp);

    snprintf(path, sizeof(path), "%s/layer0.dat", name);
    fp = fopen(path, "rb");if (!fp) {
        log_printf("loadmap: failed to open file %s !\n", path);
        return FALSE;
    }
    map->layer0 = calloc(1, map->cols * map->rows);
    if (map->layer0) {
        for (i=0; i<map->rows; i++) {
            fgets(map->layer0 + i * map->cols, map->cols, fp);
        }
    }
    fclose(fp);
    return TRUE;
}

void freemap(MAP *map)
{
    if (map) {
        destroybmp(&map->tileb);
        free(map->layer0);
    }
}

static void puttile(BMP *pb, int x, int y, RECT *clipper, MAP *pm, int n)
{
    if (n < 0) {
        bar(pb, x, y, pm->tilew, pm->tileh, 0);
    } else {
        int cols, tx, ty;
        cols = pm->tileb.width / pm->tilew;
        tx = n % cols * pm->tilew;
        ty = n / cols * pm->tileh;
        bitblt(pb, x, y, &pm->tileb, tx, ty, pm->tilew, pm->tileh);
    }
}

void drawmap(BMP *pb, int dstx, int dsty, MAP *pm, int srcx, int srcy, int w, int h)
{
    RECT rect = { dstx, dsty, dstx + w - 1, dsty + h - 1 };
    int rows, cols, toffx, toffy, r, c, i, j, n;

    rows = h / pm->tileh + (srcy % pm->tileh || h % pm->tileh);
    cols = w / pm->tileh + (srcx % pm->tilew || w % pm->tilew);
    toffx= srcx % pm->tilew ? -(srcx % pm->tilew) : 0;
    toffy= srcy % pm->tileh ? -(srcy % pm->tileh) : 0;

    if (toffx > 0) bar(pb, dstx, dsty, dstx + toffx, dsty + h, 0);
    if (toffy > 0) bar(pb, dstx, dsty, dstx + w, dsty + toffy, 0);
    for (i=0; i<rows; i++) {
        for (j=0; j<cols; j++) {
            r = srcy / pm->tileh + i;
            c = srcx / pm->tilew + j;
            n = r >= 0 && c >= 0 && r < pm->rows && c < pm->cols ? pm->layer0[r * pm->cols + c] - ' ' : -1;
            puttile(pb, dstx + toffx + j * pm->tilew, dsty + toffy + i * pm->tileh, &rect, pm, n);
        }
    }
}

#else
/* 包含头文件 */
#include "screen.h"
#include "font.h"
#include "map.h"

static MAP g_my_map = {0};
static int g_map_x  =  0;
static int g_map_y  =  0;
static int g_keys   =  0;
static int g_exit   =  0;
LRESULT CALLBACK MyWndProc(
    HWND hwnd,      /* handle to window */
    UINT uMsg,      /* message identifier */
    WPARAM wParam,  /* first message parameter */
    LPARAM lParam   /* second message parameter */
)
{
    switch (uMsg) {
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_UP   : g_keys |= (1 << 0); break;
        case VK_DOWN : g_keys |= (1 << 1); break;
        case VK_LEFT : g_keys |= (1 << 2); break;
        case VK_RIGHT: g_keys |= (1 << 3); break;
        }
        return 0;
    case WM_KEYUP:
        switch (wParam) {
        case VK_UP   : g_keys &=~(1 << 0); break;
        case VK_DOWN : g_keys &=~(1 << 1); break;
        case VK_LEFT : g_keys &=~(1 << 2); break;
        case VK_RIGHT: g_keys &=~(1 << 3); break;
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        g_exit = 1;
        return 0;
    }
    return DEF_SCREEN_WNDPROC(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI GameLoopThreadProc(LPVOID lpParam)
{
    DWORD tick_cur      = GetTickCount();
    DWORD tick_fps      = tick_cur;
    DWORD tick_2s       = tick_cur;
    int   tick_sleep    = 0;
    int   frame_counter = 0;
    int   frame_rate    = 60;
    char  text[256];

    while (!g_exit) {
        if (g_keys & (1 << 0)) g_map_y-=1;
        if (g_keys & (1 << 1)) g_map_y+=1;
        if (g_keys & (1 << 2)) g_map_x-=1;
        if (g_keys & (1 << 3)) g_map_x+=1;
        drawmap(&SCREEN, 0, 0, &g_my_map, g_map_x, g_map_y, SCREEN.width, SCREEN.height);
        snprintf(text, sizeof(text), "帧率: %d fps", frame_rate);
        outtextxy(&SCREEN, 10, 10, RGB(0, 255, 0), &FONT16, text);
        UPDATE_SCREEN(&SCREEN, 0, 0, 0, 0, TRUE);
        tick_cur = GetTickCount();
        frame_counter++;
        if (tick_cur - tick_2s > 2000) {
            tick_2s += 2000;
            frame_rate = frame_counter / 2;
            frame_counter = 0;
        }
        tick_fps  += 16;
        tick_sleep = tick_fps - tick_cur;
        if (tick_sleep > 0) Sleep(tick_sleep);
    }
    return 0;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    HANDLE hThread = NULL;

    FFRPG_WIN_INIT(hInst, MyWndProc);
    loadfont (&FONT16);
    createbmp(&SCREEN);
    loadmap(&g_my_map, "home");
    drawmap(&SCREEN, 0, 0, &g_my_map, g_map_x, g_map_y, SCREEN.width, SCREEN.height);
    UPDATE_SCREEN(&SCREEN, 0, 0, 0, 0, TRUE);

    hThread = CreateThread(NULL, 0, GameLoopThreadProc, NULL, 0, NULL);
    FFRPG_MSG_LOOP();
    WaitForSingleObject(hThread, -1);

    freemap(&g_my_map);
    destroybmp(&SCREEN);
    freefont(&FONT16);
    return 0;
}
#endif
