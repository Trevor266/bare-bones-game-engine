#ifndef _FILEH
#define _FILEH
#include "bitmap.h"

char*               ReadTextFile(const char *path);        
Bitmap*             ReadBitmapFromFile(const char *path);
static  FILE*       GetFileHandle(const char *fileHandle);
static  long        GetFileBufferSize(FILE *fileHandle);
#endif