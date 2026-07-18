#include "../include/utf8.h"

char* WideToUtf8(const wchar_t* wide)
{
    int size = WideCharToMultiByte(CP_UTF8, 0, wide, -1, NULL, 0, NULL, NULL);

    if (size == 0)
    {
        return NULL;   
    }

    char* result = malloc(size);
    WideCharToMultiByte(CP_UTF8, 0, wide, -1, result, size, NULL, NULL);

    return result;
}