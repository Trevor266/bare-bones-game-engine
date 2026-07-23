#ifndef _EDITORTILE_H
#define _EDITORTILE_H

#include "tile.h"
#include "level.h"
// The editor fundamentally works with a tile at any given point. That tile needs to be renderable in the editor to visualize the current state 
// of the level itself. 

/*

    The level itself is a file which defines a tilemap - The editor needs to provide a way to visualize 2 things:

    1 - The asset which is in use on that tile.
    2 - The asset which is in use on that layer.

    A tile may have up to (levelLayerCount) layers rendered, each with it's own tile, not that you would see this in practice.
    The idea right now is the editor allows you to select your active layer -> When it does this, the renderer will only render 
    up to the layer you are on, and will only consider edits to the tile to apply to that given layer, allowing visualization of 
    the layer and its lower layer parts.
*/

typedef struct EditorTile {
    GameTile *tiles;        // The given tiles to be rendered by the editor.

} EditorTile;

// Gotta read a level.
void LoadLevelToEditor(char *filePath)
{
    Level *level = ReadLevel(filePath);

    free(level);
}
/*

    EditorTile tile = EditorTile {
        .layerData
    }
    foreach(tile in editorTile)
    {

    }

*/
#endif