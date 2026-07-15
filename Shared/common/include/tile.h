#ifndef _TILE_H
#define _TILE_H

typedef struct GameTile {
    char*   ParentSheet;         // The name of the sprite sheet, no path required - assumes the file is present in the level folder's asset folder.
    int     ParentSheetRow;      // What row and what column in the parent asset file to pull this tile sprite from.
    int     ParentSheetColumn;
    int     LevelColumn;         // What row and column in the level itself to place this tile sprite.
    int     LevelRow;
    int     Layer;               // Rendered in ascending order (layer 0 - layerMax)
} GameTile;

#endif