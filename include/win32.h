#ifndef WIN32_H
#define WIN32_H

#include <windows.h>
#include <stdbool.h>
#include "keyboard.h"
#include "mouse.h"

extern KeyCode     Win32_VirtualKey_KeyCode_Lookup[256];

bool                Win32_PeekMessages(void);
void                Win32_Start(void);
LRESULT CALLBACK    Win32_WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void                Win32PollControllers(void);
void                PrintActiveKeyboardState(void);
void                ClearReleasedKeysFromKeyboardState(void);
void                ClearReleasedMouseButtonsFromMouseButtonState(void);
void                SetMouseDownState(MouseButton, LPARAM);
void                SetMouseUpState(MouseButton, LPARAM);
void                PrintControllerState(void);
void                PrintMouseButtonState(MouseButton);
void                ProcessCharacter(WPARAM wParam, LPARAM lParam);
void                ProcessKeyDown(WPARAM wParam, LPARAM lParam);
void                ProcessKeyUp(WPARAM wParam);
#endif