
#include "../include/level.h"

Level *ReadLevel(const char *levelFilePath)
{
    FILE *file = fopen(levelFilePath, "rb");
    if (!file)
    {
        #if DEBUG
        fprintf(stderr, "ReadLevel: failed to open '%s'\n", levelFilePath);
        #endif
        return NULL;
    }

    LevelFileHeader header;
    if (fread(&header, sizeof(LevelFileHeader), 1, file) != 1)
    {
        #if DEBUG
        fprintf(stderr, "ReadLevel: failed to read header from '%s'\n", levelFilePath);
        #endif
        fclose(file);
        return NULL;
    }

    if (header.signature != LEVEL_FILE_SIGNATURE)
    {
        #if DEBUG
        fprintf(stderr, "ReadLevel: bad signature 0x%08X in '%s'\n", header.signature, levelFilePath);
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

    level->sourcePath = _strdup(levelFilePath);
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
        level->sheetMetaData = malloc(sizeof(LevelSpriteSheetMetadata) * level->sheetCount);
        level->sheets        = malloc(sizeof(Bitmap*) * level->sheetCount);

        // A level consists of a bitmap per spritesheet, this bitmap is read into memory in full and used to pull tile data.
        // The sheets are stored within a sheets folder present in the root of the level.
        // To start off, using the level passes to us, chop off the level file to get to the level folder, 
        // and append /sheets to the end.
        char sheetBasePath[LEVEL_FILE_PATH_MAX];
        strncpy_s(sheetBasePath, sizeof(sheetBasePath), levelFilePath, _TRUNCATE);
        GetFilesContainingFolder(sheetBasePath);
        AppendFilePath(sheetBasePath, sizeof(sheetBasePath), sheetBasePath, "sheets/");

        if (fread(level->sheetMetaData, sizeof(LevelSpriteSheetMetadata), level->sheetCount, file) != level->sheetCount)
        {
            #if DEBUG
            fprintf(stderr, "ReadLevel: failed to read sheet metadata from '%s'\n", levelFilePath);
            #endif
            FreeLevel(level);
            fclose(file);
            return NULL;
        }

        for (uint8_t i = 0; i < level->sheetCount; i++)
        {
            char sheetPath[LEVEL_FILE_PATH_MAX] = "";
            AppendFilePath(sheetPath, sizeof(sheetPath), sheetBasePath, level->sheetMetaData[i].Name);

            level->sheets[i] = ReadBitmapFromFile(sheetPath);

            if (!level->sheets[i])
            {
                #if DEBUG
                fprintf(stderr, "ReadLevel: failed to load sheet '%s'\n", sheetPath);
                #endif
                FreeLevel(level);
                fclose(file);
                return NULL;
            }
        }
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
    char basePath[MAX_OS_DIRECTORY_LENGTH];
    GetRelativePathFromExecutableDirectory(basePath, sizeof(basePath), LEVEL_BASE_PATH);

    char levelFolderPath[MAX_OS_DIRECTORY_LENGTH];
    snprintf(levelFolderPath, sizeof(levelFolderPath), "%s%s", basePath, name);

    // If we weren't able to create the folder, this means there was either a canceled user action, naming conflict, or some 
    // other error. In any case we can go no further.
    if (CreateNewLevelFolder(levelFolderPath) == 0)
    {
        return NULL;
    }

    char sheetFolderPath[MAX_OS_DIRECTORY_LENGTH];
    snprintf(sheetFolderPath, sizeof(sheetFolderPath), "%s%s", levelFolderPath, "/sheets");

    // Create a new asset folder for the level that will house sprite sheets and various resources.
    if (!CreateNewAssetFolder(sheetFolderPath))
    {
        return FALSE;
    }

    // Tiles by default will use a transparent pixel buffer sized at the level's tile width and height. This creates that tile "sheet".
    if (!CreateTransparentAlphaSheet(sheetFolderPath, tileWidth, tileHeight))
    {
        return FALSE;
    }

    char bblFilePath[MAX_OS_DIRECTORY_LENGTH];
    snprintf(bblFilePath, sizeof(bblFilePath), "%s/%s.bbl", levelFolderPath, name);

    LevelFileHeader header = {
        .signature   = LEVEL_FILE_SIGNATURE,
        .levelWidth  = levelWidth,
        .levelHeight = levelHeight,
        .tileWidth   = (uint8_t)tileWidth,
        .tileHeight  = (uint8_t)tileHeight,
        .layerCount  = layerCount,
        .sheetCount  = 1, // Initialize with 1 for the blank alpha tile "sheet"
        .tileCount   = ((levelWidth / tileWidth) * (levelHeight / tileHeight)) * layerCount
    };

    FILE *file = fopen(bblFilePath, "wb");
    fwrite(&header, sizeof(LevelFileHeader), 1, file);

    LevelSpriteSheetMetadata alphaSheetMeta = {0};
    // TODO: Get file name from somewhere else.
    strncpy_s(alphaSheetMeta.Name, sizeof(alphaSheetMeta.Name), "alpha.bmp", _TRUNCATE);
    fwrite(&alphaSheetMeta, sizeof(LevelSpriteSheetMetadata), 1, file);

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
    GetRelativePathFromExecutableDirectory(basePath, sizeof(basePath), LEVEL_BASE_PATH);
    strcat_s(basePath, sizeof(basePath), levelName);

    DWORD dwAttrib = GetFileAttributes((LPCSTR)basePath);

    return 
    (
        dwAttrib != INVALID_FILE_ATTRIBUTES 
        && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
    );
}

int CreateTransparentAlphaSheet(const char *assetFolderPath, int tileWidth, int tileHeight)
{
    if (tileWidth <= 0 || tileHeight <= 0)
    {
        return FALSE;
    }

    const int channels = BITMAP_CHANNEL_FORMAT_RGBA;

    Bitmap *bitmap = malloc(sizeof(Bitmap) + (size_t)(tileWidth * tileHeight * channels));
    if (!bitmap)
    {
        return FALSE;
    }

    bitmap->width    = tileWidth;
    bitmap->height   = tileHeight;
    bitmap->channels = BITMAP_CHANNEL_FORMAT_RGBA;
    bitmap->pixels   = (uint8_t *)(bitmap + 1);

    // All-zero RGBA: R=0, G=0, B=0, A=0 -> fully transparent.
    memset(bitmap->pixels, 0, (size_t)(tileWidth * tileHeight * channels));

    char alphaSheetPath[MAX_PATH];
    AppendFilePath(alphaSheetPath, sizeof(alphaSheetPath), assetFolderPath, "/alpha.bmp");

    int written = WriteBitmapToFile(alphaSheetPath, bitmap);

    free(bitmap);

    return written;
}

int CreateNewAssetFolder(const char *assetFolderPath)
{
    if (!CreateDirectoryA(assetFolderPath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
    {
        return FALSE;
    }

    // We allow overwriting an existing level folder from dialog prompt, so only fail if that was not the error we got.
    return TRUE;
}

int CreateNewLevelFolder(const char *directoryRootPath)
{
    char basePath[MAX_PATH];
    GetRelativePathFromExecutableDirectory(basePath, sizeof(basePath), LEVEL_BASE_PATH);

    // Ensure the shared base folder exists first - on a fresh checkout/build this may not exist yet.
    if (!CreateDirectoryA(basePath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
    {
        return FALSE;
    }

    if (!CreateDirectoryA(directoryRootPath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
    {
        return FALSE;
    }

    // We allow overwriting an existing level folder from dialog prompt, so only fail if that was not the error we got.
    return TRUE;
}