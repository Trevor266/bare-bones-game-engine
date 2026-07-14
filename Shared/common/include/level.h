#ifndef LEVEL_H
#define LEVEL_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include "bitmap.h"
#include "tile.h"
#include "file.h"

#define LEVEL_FILE_SIGNATURE            0x454C564C // ELVL
#define MAX_SPRITESHEET_FILE_NAME       256        // Seems good enough
#define LEVEL_BASE_PATH                 "../assets/resources/levels/"

// TODO: These may get #if'd for platform specific path sizes later - Shared ugly windows const for now.
#define LEVEL_NAME_MAX                  MAX_PATH                                     
#define LEVEL_FOLDER_PATH_MAX           MAX_PATH
#define LEVEL_FILE_PATH_MAX             MAX_PATH

/*
    level.h

    This file defines the structure of a level to be used by the engine and toolkit for running and creating levels.
    A level is very simply a folder which contains the following:

        * Level file - Essentially a manifest with level data additionally, like tiles.
        * Assets folder - A directory which the toolkit and engine load the various level assets from.
    
    There's really nothing more to it than that for now. The level file contains a tile list that represents data about what and where that tile should go. 
    For example, for a level that has 5 layers, there may be up to 5 different tiles placed on each tile position, the engine and toolkit both render these
    in ascending layer order - this is essentially a z coordinate - for more information about tiles, see tile.h.
*/
typedef struct
{
    uint32_t    signature;
    uint16_t    levelWidth;     // level size, in tiles
    uint16_t    levelHeight;
    uint8_t     tileWidth;      // tile size, in pixels
    uint8_t     tileHeight;
    uint8_t     layerCount;
    uint8_t     sheetCount;
    uint32_t    tileCount;
} LevelFileHeader;

typedef struct LevelSpriteSheetMetadata {
    char Name[MAX_SPRITESHEET_FILE_NAME]; // filename in assets folder, e.g. "terrain.png"
} LevelSpriteSheetMetadata;

typedef struct Level {
    uint16_t                    levelWidth;
    uint16_t                    levelHeight;
    uint16_t                    tileWidth;
    uint16_t                    tileHeight;
    uint8_t                     layerCount;
    uint8_t                     sheetCount;
    uint32_t                    tileCount;
    Bitmap                      **sheets;
    LevelSpriteSheetMetadata    *sheetMetaData;  // sheetCount entries
    GameTile                    *tiles;          // tileCount entries
} Level;

Level *ReadLevel(const char *filePath);
void PrintLevelProperties(const Level *level);
void FreeLevel(Level *level);
bool LevelExists(char *levelName);
Level *LoadLevel(const char *path);
Level *CreateLevel(const char *name, uint8_t layerCount, uint16_t tileWidth, uint16_t tileHeight, uint16_t levelWidth, uint16_t levelHeight);

#endif