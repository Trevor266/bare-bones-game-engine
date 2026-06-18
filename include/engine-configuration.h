/*
    The engine needs to have a certain amount of starting data to know how to handle very basic startup and runtime operations.
    This is done by reading a configuration file from disk at startup, and saving to it periodically as settings potentially change 
    (think an in game settings menu with global settings like volume). 
*/
// Define the file header signature for the engine config file (ECFG - Engine config)
#define ENGINE_CONFIG_FILE_SIGNATURE    0x45434647

#define GAME_TILE_WIDTH                 16
#define GAME_TILE_HEIGHT                16
#define LOGICAL_WIDTH                   640
#define LOGICAL_HEIGHT                  360
#define SCREEN_TILE_COLS                (LOGICAL_WIDTH / GAME_TILE_WIDTH)
#define SCREEN_TILE_ROWS                ((LOGICAL_HEIGHT + GAME_TILE_HEIGHT - 1) / GAME_TILE_HEIGHT)
#define SUPPORTED_SAVE_SLOTS            3

// Defines a set file format for the engine configuration file at a precisely packed size.
#pragma pack(push, 1)
typedef struct EngineConfigFile {
    uint32_t    fileSignature;
    int32_t     logicalGameHeight;
    int32_t     logicalGameWidth;
    int32_t     supportedSaveSlots;
    int32_t     tileHeight;
    int32_t     tileWidth;
} EngineConfigFile;
#pragma pack(pop)