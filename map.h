#ifndef __MAP_H__
#define __MAP_H__

#include "bmp.h"

typedef struct {
    int   cols;
    int   rows;
    int   tilew;
    int   tileh;
    BMP   tileb;
    char *layer0;
} MAP;

BOOL loadmap(MAP *map, char *name);
void freemap(MAP *map);
void drawmap(BMP *pb, int dstx, int dsty, MAP *pm, int srcx, int srcy, int w, int h);

#endif

