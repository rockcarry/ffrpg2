/* 包含头文件 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* parse_params(const char *str, const char *key, char *val, int len)
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

char* read_file(const char *filename, int *filelen)
{
    FILE *fp = fopen(filename, "rb");
    int   len;
    char *data;

    if (!fp) {
        if (filelen) *filelen = 0;
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    data = malloc(len);
    fread(data, len, 1, fp);
    fclose(fp);
    if (filelen) *filelen = len;
    return data;
}
