#ifndef _HOMESCREEN_H
#define _HOMESCREEN_H
#include <windows.h>
#include "../../Shared/common/include/buffer.h"
#include "../../Shared/common/include/font.h"

void DrawHomeScreen(HWND windowHandle, OffscreenBuffer WindowBackBuffer, Font font);

#endif