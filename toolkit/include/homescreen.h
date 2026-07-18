#ifndef _HOMESCREEN_H
#define _HOMESCREEN_H
#include <windows.h>
#include <wchar.h>
#include <shlobj.h>
#include "../include/button.h"
#include "../include/resource.h"
#include "../../Shared/common/include/primitive_types.h"
#include "../../Shared/common/include/primitive_geometry.h"
#include "../../Shared/common/include/dimensions.h"
#include "../../Shared/common/include/buffer.h"
#include "../../Shared/common/include/font.h"
#include "../../Shared/common/include/mouse.h"
#include "../../Shared/common/include/level.h"
#include "../../Shared/common/include/window.h"
#include "../../Shared/common/include/file.h"
#include "../../Shared/common/include/utf8.h"

#define HOMESCREEN_BUTTON_COUNT 3
#define NEW_LEVEL_BUTTON_ID     0
#define LOAD_LEVEL_BUTTON_ID    1
#define SETTINGS_BUTTON_ID      2

extern Button homescreenButtons[HOMESCREEN_BUTTON_COUNT];

typedef enum HomescreenAction 
{
    CREATED_LEVEL = 0,
    LOADED_LEVEL = 1,
    LOADED_SETTINGS = 2
} HomescreenAction;

// Provides an output to main that can be used to determine what was done and how to proceed, this keeps implementation of 
// other parts of the toolkit separated while allowing them to react to one another in a coordinated way.
typedef struct HomescreenResult 
{
    HomescreenAction    action;
    Level*              level;
} HomescreenResult;

typedef struct NewLevelParams
{
    char buffer[LEVEL_NAME_MAX];
    int  layerCount;
    int  tileWidth;
    int  tileHeight;
    int  levelWidth;
    int  levelHeight;
} NewLevelParams;

typedef void (*CloseHomescreenCallback)(HomescreenResult result);

void                InitializeHomescreen(CloseHomescreenCallback onClose);
void                DrawHomeScreen(OffscreenBuffer WindowBackBuffer, Font font);
void                CheckHomescreenClickEvents(int hitX, int hitY);
bool                BuildNewLevelDialogPromptAndPrompt(NewLevelParams *params);
INT_PTR CALLBACK    NewLevelNameDialogProc(HWND dialogHandle, UINT message, WPARAM wParam, LPARAM lParam);
Level*              HandleLevelNamingConflict(NewLevelParams *params);
void                Close(HomescreenAction action, Level* level);
void                HandleLoadLevelClick();
void                HandleNewLevelClick();

#endif