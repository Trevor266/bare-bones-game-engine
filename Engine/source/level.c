#include "../include/level.h"
#include "../include/file.h"

Level *LoadLevel(const char *path)
{
    // Load the specified level file.
    FILE *levelFile = fopen(path, "rb");
    if (!levelFile)
    {
         return NULL;
    }

    // Next validate the header signature and get level information from the header.
    LevelFileHeader levelFileHeader;
    fread(&levelFileHeader, sizeof(LevelFileHeader), 1, levelFile);

    if (levelFileHeader.signature != LEVEL_FILE_SIGNATURE) 
    {
        fclose(levelFile);
        return NULL;
    }

    // Allocate level memory, and set the header fields.
    Level *level = malloc(sizeof(Level));
    level->layerCount = levelFileHeader.layerCount;
    level->sheetCount = levelFileHeader.sheetCount;
    level->sheets     = malloc(levelFileHeader.sheetCount * sizeof(Bitmap *));

    for (int i = 0; i < levelFileHeader.sheetCount; i++)
    {
        uint16_t nameLength;
        fread(&nameLength, sizeof(uint16_t), 1, levelFile);

        char *filename = malloc(nameLength + 1);
        fread(filename, 1, nameLength, levelFile);
        filename[nameLength] = '\0';

        level->sheets[i] = ReadBitmapFromFile(filename);
        free(filename);
    }

    // Allocate memory for the level tiles.
    int levelTileCount = GameEngine.configuration->maxLevelHorizontalTiles * GameEngine.configuration->maxLevelVerticalTiles * levelFileHeader.layerCount;
    level->tiles  = malloc(levelTileCount * sizeof(TileSheetTile));


    fread(level->tiles, sizeof(TileSheetTile), levelTileCount, levelFile);

    fclose(levelFile);
    return level;
}