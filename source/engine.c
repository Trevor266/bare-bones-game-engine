#include "engine.h"
#include "file.h"
#include "engine-configuration.h"

#include <stdlib.h>

Engine GameEngine = {0};

void InitializeEngine(void)
{
    // This engine is configurable, and therefore needs to load a configuration file 
    // to gather initial data related to setting itself up.
    GameEngine.configuration = BuildEngineConfiguration();
}

static EngineConfiguration* BuildEngineConfiguration()
{
    /* 
        The engine's configuration is at any given time in 3 states:

        Not Found       -       No engine.bin file exists containing the engine configuration, the engine is unusable in this state.
        Initialized     -       An engine.bin file didn't exist, so we created one, the values are all default values the engine hardcodes for initialization.
        Exists          -       An engine.bin file exists, so at some point we already ran initialization and should treat this config as custom (potentially non-default).
    */

    // We need to determine if the engine has a configuration file first.
    FILE *file = fopen("data/engine-configuration.bin", "rb");
    if (!file)
    {
        // When we do not find a configuration (state 1), we will generate a default configuration (state 2)
        return BuildDefaultEngineConfiguration();
    }

    // If there was a configuration found at this file, we should actually read the contents, allocate memory for the configuration, and 
    // allocate.

    return ReadSavedEngineConfiguration();
}

static EngineConfiguration* BuildDefaultEngineConfiguration()
{
    EngineConfiguration *defaultEngineConfiguration = malloc(sizeof(EngineConfiguration));

    if (!defaultEngineConfiguration)
    {
        return NULL;
    }

    defaultEngineConfiguration->logicalGameHeight       = LOGICAL_HEIGHT;
    defaultEngineConfiguration->logicalGameWidth        = LOGICAL_WIDTH;
    defaultEngineConfiguration->tileHeight              = GAME_TILE_HEIGHT;
    defaultEngineConfiguration->tileWidth               = GAME_TILE_WIDTH;
    defaultEngineConfiguration->supportedSaveSlots      = SUPPORTED_SAVE_SLOTS;
    defaultEngineConfiguration->maxLevelHorizontalTiles = MAX_LEVEL_HORIZONTAL_TILES;
    defaultEngineConfiguration->maxLevelVerticalTiles   = MAX_LEVEL_VERTICAL_TILES;

    int writeSuccess = WriteEngineConfiguration(defaultEngineConfiguration);

    return writeSuccess ? defaultEngineConfiguration : NULL;
}

// Returns 0 for successful write, 1 for failed.
static int WriteEngineConfiguration(EngineConfiguration *config)
{
    FILE *file = fopen("assets/data/engine-configuration.bin", "wb");

    if (!file)
    {
        return 1;
    }

    EngineConfigFile fileData;
    fileData.fileSignature              = ENGINE_CONFIG_FILE_SIGNATURE;
    fileData.logicalGameHeight          = config->logicalGameHeight;
    fileData.logicalGameWidth           = config->logicalGameWidth;
    fileData.supportedSaveSlots         = config->supportedSaveSlots;
    fileData.tileHeight                 = config->tileHeight;
    fileData.tileWidth                  = config->tileWidth;
    fileData.maxLevelHorizontalTiles    = config->maxLevelHorizontalTiles;
    fileData.maxLevelVerticalTiles      = config->maxLevelVerticalTiles;

    fwrite(&fileData, sizeof(EngineConfigFile), 1, file);
    fclose(file);

    return 0;
}

static EngineConfiguration* ReadSavedEngineConfiguration()
{
    FILE *file = fopen("assets/data/engine-configuration.bin", "rb");
    if (!file) return NULL;

    EngineConfigFile fileData;
    fread(&fileData, sizeof(EngineConfigFile), 1, file);
    fclose(file);

    if (fileData.fileSignature != ENGINE_CONFIG_FILE_SIGNATURE)
    {
        printf("Unable to read engine configuration, file signature does not match expected signature.\n");
        return NULL;
    }

    EngineConfiguration *config = malloc(sizeof(EngineConfiguration));
    if (!config) return NULL;

    config->logicalGameHeight  = fileData.logicalGameHeight;
    config->logicalGameWidth   = fileData.logicalGameWidth;
    config->supportedSaveSlots = fileData.supportedSaveSlots;
    config->tileHeight         = fileData.tileHeight;
    config->tileWidth          = fileData.tileWidth;

    return config;
}