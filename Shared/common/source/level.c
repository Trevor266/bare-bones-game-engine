#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "../include/level.h"
#include "../include/file.h"

Level *CreateLevel(
    const char *name,
    uint8_t layerCount,
    uint16_t tileWidth,
    uint16_t tileHeight,
    uint16_t levelWidth,
    uint16_t levelHeight
)
{
    char basePath[MAX_PATH];
    GetExecutableWorkingDirectory(basePath, sizeof(basePath), LEVEL_BASE_PATH);

    char folderPath[512];
    snprintf(folderPath, sizeof(folderPath), "%s%s", basePath, name);

    if (CreateNewLevelFolder(folderPath) == 0)
    {
        return NULL;
    }

    char filePath[600];
    snprintf(filePath, sizeof(filePath), "%s/%s.bbl", folderPath, name);

    LevelFileHeader header = {
        .signature   = LEVEL_FILE_SIGNATURE,
        .levelWidth  = levelWidth,
        .levelHeight = levelHeight,
        .tileWidth   = (uint8_t)tileWidth,
        .tileHeight  = (uint8_t)tileHeight,
        .layerCount  = layerCount,
        .sheetCount  = 0,
        .tileCount   = 0
    };

    FILE *file = fopen(filePath, "wb");
    fwrite(&header, sizeof(LevelFileHeader), 1, file);
    fclose(file);

    Level *level = malloc(sizeof(Level));
    level->levelWidth    = levelWidth;
    level->levelHeight   = levelHeight;
    level->tileWidth     = tileWidth;
    level->tileHeight    = tileHeight;
    level->layerCount    = layerCount;
    level->sheetCount    = 0;
    level->tileCount     = 0;
    level->sheets        = NULL;
    level->sheetMetaData = NULL;
    level->tiles         = NULL;

    return level;
}

bool LevelExists(char *levelName)
{
    char basePath[MAX_PATH];
    GetExecutableWorkingDirectory(basePath, sizeof(basePath), LEVEL_BASE_PATH);
    strcat_s(basePath, sizeof(basePath), levelName);

    DWORD dwAttrib = GetFileAttributes((LPCSTR)basePath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}