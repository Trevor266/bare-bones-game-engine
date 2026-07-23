#ifndef _TILE_H
#define _TILE_H

typedef struct GameTile {
    int     Layer;                                      // Rendered in ascending order (layer 0 - layerMax)
    int     LevelColumn;                                // What row and column in the level itself to place this tile sprite.
    int     LevelRow;
    char    ParentSheet[MAX_OS_DIRECTORY_LENGTH];       // The name of the sprite sheet, no path required - assumes the file is present in the level folder's asset folder.  
    int     ParentSheetColumn;
    int     ParentSheetRow;                             // What row and what column in the parent asset file to pull this tile sprite from.
} GameTile;

#endif