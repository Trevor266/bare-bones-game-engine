
#include "../include/level.h"

Level *ReadLevel(const char *filePath)
{
    FILE *file = fopen(filePath, "rb");
    if (!file)
    {
        #if DEBUG
        fprintf(stderr, "ReadLevel: failed to open '%s'\n", filePath);
        #endif
        return NULL;
    }

    LevelFileHeader header;
    if (fread(&header, sizeof(LevelFileHeader), 1, file) != 1)
    {
        #if DEBUG
        fprintf(stderr, "ReadLevel: failed to read header from '%s'\n", filePath);
        #endif
        fclose(file);
        return NULL;
    }

    if (header.signature != LEVEL_FILE_SIGNATURE)
    {
        #if DEBUG
        fprintf(stderr, "ReadLevel: bad signature 0x%08X in '%s'\n", header.signature, filePath);
        #endif
        fclose(file);
        return NULL;
    }

    Level *level = malloc(sizeof(Level));
    if (!level)
    {
        fclose(file);
        return NULL;
    }

    level->sourcePath = _strdup(filePath);
    level->levelWidth  = header.levelWidth;
    level->levelHeight = header.levelHeight;
    level->tileWidth   = header.tileWidth;
    level->tileHeight  = header.tileHeight;
    level->layerCount  = header.layerCount;
    level->sheetCount  = header.sheetCount;
    level->tileCount   = header.tileCount;
    level->sheets        = NULL;
    level->sheetMetaData = NULL;
    level->tiles          = NULL;

    if (level->sheetCount > 0)
    {
        // TODO: Implement tilesheets
    }

    if (level->tileCount > 0)
    {
        // TODO: Impelemnt tile data
    }

    fclose(file);
    return level;
}

void PrintLevelProperties(const Level *level)
{
    if (!level)
    {
        printf("PrintLevelProperties: level is NULL\n");
        return;
    }

    printf("Level properties:\n");
    printf("----Level size (tiles) : %u x %u\n", level->levelWidth, level->levelHeight);
    printf("----Tile size (px)     : %u x %u\n", level->tileWidth, level->tileHeight);
    printf("----Layer count        : %u\n", level->layerCount);
    printf("----Sheet count        : %u\n", level->sheetCount);
    printf("----Tile count         : %u\n", level->tileCount);
}

void FreeLevel(Level *level)
{
    if (!level) 
    {
        return;
    }
    
    free(level->tiles);
    free(level->sheetMetaData);
    free(level->sheets);
    free(level);
}

// Create a new level. A level is a directory which contains a .bbl file containing metadata relating to the level, 
// along with assets relating to the level. The folder and .bbl file are both named after the name parameter.
Level *CreateLevel
(
    const char *name,
    uint8_t layerCount,
    uint16_t tileWidth,
    uint16_t tileHeight,
    uint16_t levelWidth,
    uint16_t levelHeight
)
{
    // Levels are written to a shared asset folder that's copied into the toolkit and engine builds at runtime. 
    // Here we are getting the exectuable directory and then navigating back to the shared asset folder with a relative path.
    char basePath[LEVEL_FOLDER_PATH_MAX];
    GetExecutableWorkingDirectory(basePath, sizeof(basePath), LEVEL_BASE_PATH);

    char levelFolderPath[LEVEL_FOLDER_PATH_MAX];
    snprintf(levelFolderPath, sizeof(levelFolderPath), "%s%s", basePath, name);

    // If we weren't able to create the folder, this means there was either a canceled user action, naming conflict, or some 
    // other error. In any case we can go no further.
    if (CreateNewLevelFolder(levelFolderPath) == 0)
    {
        return NULL;
    }

    char bblFilePath[LEVEL_FILE_PATH_MAX];
    snprintf(bblFilePath, sizeof(bblFilePath), "%s/%s.bbl", levelFolderPath, name);

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

    FILE *file = fopen(bblFilePath, "wb");
    fwrite(&header, sizeof(LevelFileHeader), 1, file);
    fclose(file);

    Level *level = ReadLevel(bblFilePath);

    #if DEBUG
    if (level)
    {
        PrintLevelProperties(level);
    }
    #endif

    return level;
}

bool LevelExists(char *levelName)
{
    char basePath[MAX_PATH];
    GetExecutableWorkingDirectory(basePath, sizeof(basePath), LEVEL_BASE_PATH);
    strcat_s(basePath, sizeof(basePath), levelName);

    DWORD dwAttrib = GetFileAttributes((LPCSTR)basePath);

    return 
    (
        dwAttrib != INVALID_FILE_ATTRIBUTES 
        && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
    );
}