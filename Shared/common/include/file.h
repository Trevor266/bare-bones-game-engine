#ifndef _FILEH
#define _FILEH
#include <stdio.h>
#include "bitmap.h"

char*               ReadTextFile(const char *path);
Bitmap*             ReadBitmapFromFile(const char *path);
static  FILE*       GetFileHandle(const char *fileHandle);
static  long        GetFileBufferSize(FILE *fileHandle);
int                 WriteBinaryFile(FILE *file, const char *contents);
int                 CreateNewLevelFolder(const char *directoryRootPath);
void                GetExecutableWorkingDirectory(char *outPath, size_t outSize, const char *relativePath);
#endif