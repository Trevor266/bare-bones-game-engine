#ifndef WIN32_H
#define WIN32_H

#include <windows.h>
#include <stdbool.h>
#include "keyboard.h"
#include "window.h"

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
static              HICON CreateHICONFromPixelBuffer(PixelBuffer *pixelBuffer, BOOL isIcon, int hotspotX, int hotspotY);
HCURSOR             CreateCursorFromPixelBuffer(PixelBuffer *pixelBuffer, int hotspotX, int hotspotY);
HICON               CreateIconFromPixelBuffer(PixelBuffer *pixelBuffer);
void                GenerateRandomLevel(void);
#endif