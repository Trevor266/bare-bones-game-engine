#ifndef LEVEL_H
#define LEVEL_H
#include "bitmap.h"
#include <stdint.h>

#define LEVEL_FILE_SIGNATURE 0x454C564C // ELVL

/*
NOTE: HUGE WIP HERE, MUCH OF THIS HAS ALREADY CHANGED. WILL UPDATE.
    This file contains functions and types related to loading a level file from memory, parsing it, and loading it into the 
    engine as an active level struct.

    The level itself is a file which defines all the relevant data that the engine is going to need to know about so that it can 
    properly load the correct portions of the game into memory/provide the right bitmaps to the renderer in the right order.

    We start with a header which defines the file signature for validating the file format.

    The level needs to provide the engine with every single asset it plans on using, so that the engine can read them from disk 
    and utilize them correctly. 

    Additionally, we provide animation data, spawn points, item locations, tiles that trigger events, etc. None of this has been defined.
*/
typedef struct
{
    uint32_t signature;
    uint8_t  layerCount;
    uint8_t  sheetCount;
} LevelFileHeader;

typedef struct
{
    uint8_t sheetId;
    uint8_t row;
    uint8_t column;
    uint8_t layer;
} TileSheetTile;

typedef struct Level 
{
    uint8_t layerCount;
    uint8_t sheetCount;
    Bitmap  **sheets;
    TileSheetTile  *tiles;
} Level;

Level *LoadLevel(const char *path);

#endif