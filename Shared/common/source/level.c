
#include "../include/level.h"

Level *ReadLevel(const char *levelFilePathBuffer)
{
    FILE *levelFile = fopen(levelFilePathBuffer, "rb");
    if (!levelFile)
    {
        #if DEBUG
        fprintf(stderr, "ReadLevel: failed to open '%s'\n", levelFilePathBuffer);
        #endif
        return NULL;
    }

    LevelFileHeader header;
    if (fread(&header, sizeof(LevelFileHeader), 1, levelFile) != 1)
    {
        #if DEBUG
        fprintf(stderr, "ReadLevel: failed to read header from '%s'\n", levelFilePathBuffer);
        #endif
        fclose(levelFile);
        return NULL;
    }

    if (header.signature != LEVEL_FILE_SIGNATURE)
    {
        #if DEBUG
        fprintf(stderr, "ReadLevel: bad signature 0x%08X in '%s'\n", header.signature, levelFilePathBuffer);
        #endif
        fclose(levelFile);
        return NULL;
    }

    Level *level = malloc(sizeof(Level));
    if (!level)
    {
        fclose(levelFile);
        return NULL;
    }

    level->sourcePath                   = _strdup(levelFilePathBuffer);
    level->levelWidth                   = header.levelWidth;
    level->levelHeight                  = header.levelHeight;
    level->tileWidth                    = header.tileWidth;
    level->tileHeight                   = header.tileHeight;
    level->layerCount                   = header.layerCount;
    level->spriteSheetCount             = header.spriteSheetCount;
    level->tileCount                    = header.tileCount;
    level->spriteSheetsBuffer           = NULL;
    level->spriteSheetMetaDataBuffer    = NULL;
    level->tiles                        = NULL;

    if (level->spriteSheetCount > 0)
    {
        level->spriteSheetMetaDataBuffer = malloc(sizeof(LevelSpriteSheetMetadata) * level->spriteSheetCount);
        level->spriteSheetsBuffer        = malloc(sizeof(Bitmap*) * level->spriteSheetCount);

        // A level contains spritesheets, stored as bitmap memory, this bitmap data is read into memory in full and used to pull tile data.
        // The sheets are stored within a sheets folder present in the root of the level.
        // To start off, get the parent folder of the level so we know what directory we are working with.
        char sheetBasePathBuffer[LEVEL_FILE_PATH_MAX];
        GetFilesContainingFolder(levelFilePathBuffer, sheetBasePathBuffer);
        AppendFilePath(sheetBasePathBuffer, sizeof(sheetBasePathBuffer), sheetBasePathBuffer, "sheets/");

        // Now we have the full path to the level's sheets folder, use this to read the sheet meta data from the file, sheet meta data includes things like the physical file name of the asset.
        if (fread(level->spriteSheetMetaDataBuffer, sizeof(LevelSpriteSheetMetadata), level->spriteSheetCount, levelFile) != level->spriteSheetCount)
        {
            #if DEBUG
            fprintf(stderr, "ReadLevel: failed to read sheet metadata from '%s'\n", levelFilePathBuffer);
            #endif
            FreeLevel(level);
            fclose(levelFile);
            return NULL;
        }

        for (uint8_t i = 0; i < level->spriteSheetCount; i++)
        {
            // For each sheet, load the physical bitmaps for each tilesheet into memory.
            char sheetPathBuffer[LEVEL_FILE_PATH_MAX] = "";
            AppendFilePath(sheetPathBuffer, sizeof(sheetPathBuffer), sheetBasePathBuffer, level->spriteSheetMetaDataBuffer[i].Name);

            level->spriteSheetsBuffer[i] = ReadBitmapFromFile(sheetPathBuffer);

            if (!level->spriteSheetsBuffer[i])
            {
                #if DEBUG
                fprintf(stderr, "ReadLevel: failed to load sheet '%s'\n", sheetPathBuffer);
                #endif
                FreeLevel(level);
                fclose(levelFile);
                return NULL;
            }
        }
    }

    if (level->tileCount > 0)
    {
        // TODO: Impelemnt tile data
    }

    fclose(levelFile);
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
    printf("----Sheet count        : %u\n", level->spriteSheetCount);
    printf("----Tile count         : %u\n", level->tileCount);
}

void FreeLevel(Level *level)
{
    if (!level) 
    {
        return;
    }
    
    free(level->tiles);
    free(level->spriteSheetMetaDataBuffer);
    free(level->spriteSheetsBuffer);
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
    char levelsDirectoryPathBuffer[MAX_OS_DIRECTORY_LENGTH];
    GetRelativePathFromExecutableDirectory(levelsDirectoryPathBuffer, sizeof(levelsDirectoryPathBuffer), LEVEL_BASE_PATH);

    char newLevelFolderPathBuffer[MAX_OS_DIRECTORY_LENGTH];
    snprintf(newLevelFolderPathBuffer, sizeof(newLevelFolderPathBuffer), "%s%s", levelsDirectoryPathBuffer, name);

    // If we weren't able to create the folder, this means there was either a canceled user action, naming conflict, or some 
    // other error. In any case we can go no further.
    if (CreateNewLevelFolder(newLevelFolderPathBuffer) == 0)
    {
        return NULL;
    }

    char sheetFolderPathBuffer[MAX_OS_DIRECTORY_LENGTH];
    snprintf(sheetFolderPathBuffer, sizeof(sheetFolderPathBuffer), "%s%s", newLevelFolderPathBuffer, "/sheets");

    // Create a new asset folder for the level that will house sprite sheets and various resources.
    if (!CreateNewAssetFolder(sheetFolderPathBuffer))
    {
        return FALSE;
    }

    // Tiles by default will use a transparent pixel buffer sized at the level's tile width and height. This creates that tile "sheet".
    if (!CreateTransparentAlphaSheet(sheetFolderPathBuffer, tileWidth, tileHeight))
    {
        return FALSE;
    }

    char levelFilePathBuffer[MAX_OS_DIRECTORY_LENGTH];
    snprintf(levelFilePathBuffer, sizeof(levelFilePathBuffer), "%s/%s.bbl", newLevelFolderPathBuffer, name);

    LevelFileHeader header = {
        .signature   = LEVEL_FILE_SIGNATURE,
        .levelWidth  = levelWidth,
        .levelHeight = levelHeight,
        .tileWidth   = (uint8_t)tileWidth,
        .tileHeight  = (uint8_t)tileHeight,
        .layerCount  = layerCount,
        .spriteSheetCount  = 1, // Initialize with 1 for the blank alpha tile "sheet" - Update as new default sheets are needed.
        .tileCount   = ((levelWidth / tileWidth) * (levelHeight / tileHeight)) * layerCount
    };

    FILE *levelFile = fopen(levelFilePathBuffer, "wb");
    fwrite(&header, sizeof(LevelFileHeader), 1, levelFile);

    LevelSpriteSheetMetadata alphaSheetMetadata = {0};
    strncpy_s(alphaSheetMetadata.Name, sizeof(alphaSheetMetadata.Name), DEFAULT_ALPHA_SHEET_FILE_NAME, _TRUNCATE);
    fwrite(&alphaSheetMetadata, sizeof(LevelSpriteSheetMetadata), 1, levelFile);

    fclose(levelFile);

    Level *level = ReadLevel(levelFilePathBuffer);

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