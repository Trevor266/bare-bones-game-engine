#ifndef WIN32_H
#define WIN32_H

#include <windows.h>
#include <stdbool.h>
#include "keyboard.h"
#include "mouse.h"

extern KeyCode     Win32_VirtualKey_KeyCode_Lookup[256];
extern MouseButton Win32_VirtualKey_MouseButton_Lookup[256];

bool    Win32_PeekMessages(void);
void    Win32_Start(void);
void    PrintActiveKeyboardState(void);
void    ClearReleasedKeysFromKeyboardState(void);

#endif