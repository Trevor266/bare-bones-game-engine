#ifndef ENGINE_H
#define ENGINE_H
#include "level.h"

typedef struct GameState {
    int foo;
} GameState;

typedef struct EngineConfiguration {
    int32_t     logicalGameHeight;
    int32_t     logicalGameWidth;
    int32_t     supportedSaveSlots;
    int32_t     tileHeight;
    int32_t     tileWidth;
    uint32_t    maxLevelHorizontalTiles;
    uint32_t    maxLevelVerticalTiles;
} EngineConfiguration;

typedef struct Menu {
    int foo;
} Menu;

typedef struct Engine {
    EngineConfiguration *configuration;

    // An data structure that encompasses all relevant game state data including input, io, save data, player state, etc.
    GameState *state;

    // The active level or game menu/intermediary screen currently being shown. The engine is only ever working with one or the other.

    // A level represents a structure containing all relevant data from a file related to the active level being shown. This file is defined 
    // in detail in level.h (TODO: Implement level.h). It contains everything from tile positions to layer data to tilesheets, potential npcs, animations, etc.
    // When the engine is working with a level, it is actively rendering in a game loop, reading level data, and processing inputs and running game logic on that 
    // level data.
    Level *level;

    // A menu is any screen that is not a level, which is to say an in game level the engine runs a full game loop one. 
    // The general idea is these are loading screens, splash screens, menus with widgets to select a point in the game, 
    // anything that is not that active in game level.
    // Conceptually, if you wanted to have some sort of gameplay with a menu, you could just create a level that handles the gameplay, while having
    // widgets or click regions that trigger events, so the menu concept exists to allow the game to pause, save off data, show a minimal ui with inputs, 
    // and load a new level once the old one is unloaded to keep memory usage low, essentially these are screens between gameplay, therefore they utilize a different
    // data structure.
    Menu *menu;
} Engine;

extern Engine GameEngine;

void                            InitializeEngine(void);
static int                      WriteEngineConfiguration(EngineConfiguration *config);
static EngineConfiguration*     BuildDefaultEngineConfiguration();
static EngineConfiguration*     BuildEngineConfiguration();
static EngineConfiguration*     ReadSavedEngineConfiguration();

#endif