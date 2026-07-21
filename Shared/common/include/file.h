#ifndef _FILEH
#define _FILEH
#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <windows.h>
#include <shlobj.h>

#ifdef _WIN32
#define MAX_OS_DIRECTORY_LENGTH MAX_PATH
#else
#define MAX_OS_DIRECTORY_LENGTH 260
#endif

void                AppendFilePath(char *dest, size_t destSize, const char *basePath, const char *appendedPath);
void                GetFilesContainingFolder(const char *filePathBuffer, char *containingFolderPathBuffer);
char*               ReadTextFile(const char *path);
static  FILE*       GetFileHandle(const char *fileHandle);
static  long        GetFileBufferSize(FILE *fileHandle);
int                 WriteBinaryFile(FILE *file, const char *contents);
void                GetRelativePathFromExecutableDirectory(char *outPath, size_t outSize, const char *relativePath);
BOOL                GetCanonicalizedExecutableWorkingDirectory(wchar_t *outPath, size_t outSize, const char *relativePath);
BOOL                BuildPIDLISTFromPath(wchar_t *directoryPath, PIDLIST_ABSOLUTE *pidlAbsolute);

#endif