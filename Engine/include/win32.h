#ifndef WIN32_H
#define WIN32_H


#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include <Xinput.h>
#include <time.h>

#include "../../Shared/common/include/mouse.h"
#include "../../Shared/common/include/debug.h"
#include "../../Shared/common/include/window.h"
#include "../../Shared/common/include/bitmap.h"
#include "../../Shared/common/include/file.h"
#include "keyboard.h"
#include "controller.h"
#include "engine-configuration.h"

extern KeyCode     Win32_VirtualKey_KeyCode_Lookup[256];

bool                Win32_PeekMessages(void);
void                Win32_Start(WindowCreationParameters *windowCreationParams);
LRESULT CALLBACK    Win32_WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void                Win32PollControllers(void);
void                Win32_InitializeWindow(WindowCreationParameters *windowCreationParams);
void                PrintActiveKeyboardState(void);
void                ClearReleasedKeysFromKeyboardState(void);
void                ClearReleasedMouseButtonsFromMouseButtonState(void);
void                PrintControllerState(void);
void                ProcessCharacter(WPARAM wParam, LPARAM lParam);
void                ProcessKeyDown(WPARAM wParam, LPARAM lParam);
void                ProcessKeyUp(WPARAM wParam);
static HICON        CreateHICONFromPixelBuffer(PixelBuffer *pixelBuffer, BOOL isIcon, int hotspotX, int hotspotY);
HCURSOR             CreateCursorFromPixelBuffer(PixelBuffer *pixelBuffer, int hotspotX, int hotspotY);
HICON               CreateIconFromPixelBuffer(PixelBuffer *pixelBuffer);
void                GenerateRandomLevel(void);
#endif