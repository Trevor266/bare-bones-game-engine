#ifndef _HOMESCREEN_H
#define _HOMESCREEN_H
#include <windows.h>
#include "../../Shared/common/include/buffer.h"
#include "../../Shared/common/include/font.h"
#include "../include/button.h"

#define HOMESCREEN_BUTTON_COUNT 3
#define NEW_LEVEL_BUTTON_ID     0
#define LOAD_LEVEL_BUTTON_ID    1
#define SETTINGS_BUTTON_ID      2

extern Button homescreenButtons[HOMESCREEN_BUTTON_COUNT];

void DrawHomeScreen(OffscreenBuffer WindowBackBuffer, Font font);
void CheckHomescreenClickEvents(int hitX, int hitY);
INT_PTR CALLBACK NewLevelNameDialogProc(HWND dialogHandle, UINT message, WPARAM wParam, LPARAM lParam);

#endif