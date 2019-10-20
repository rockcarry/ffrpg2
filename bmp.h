#ifndef __BMP_H__
#define __BMP_H__

/* 包含头文件 */
#include "stdefine.h"

/* 类型定义 */
/* BMP 对象的类型定义 */
typedef struct {
    int    width;      /* 宽度 */
    int    height;     /* 高度 */
    DWORD *pdata;      /* 数据 */
    void  *pextra;

    BOOL (*createbmp )(void *pb);
    void (*destroybmp)(void *pb);
} BMP;

/* 函数声明 */
/* 创建与销毁 */
BOOL createbmp (BMP *pb);
void destroybmp(BMP *pb);

BOOL loadbmp(BMP *pb, char *file);
BOOL savebmp(BMP *pb, char *file);

#endif


