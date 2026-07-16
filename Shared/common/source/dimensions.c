#include "../include/dimensions.h"

#ifdef _WIN32
#include <windows.h>

Dimensions GetWin32WindowDimensions()
{
    Dimensions windowDimensions = {0};
    RECT rect;

    if (GetClientRect(PlatformWindowInstance.window, &rect))
    {
        windowDimensions.width  = rect.right - rect.left;
        windowDimensions.height = rect.bottom - rect.top;
    }

    POINT topLeft = { 0, 0 };
    if (ClientToScreen(PlatformWindowInstance.window, &topLeft))
    {
        HWND parent = GetParent(PlatformWindowInstance.window);
        if (parent)
        {
            ScreenToClient(parent, &topLeft);
        }
        windowDimensions.x = topLeft.x;
        windowDimensions.y = topLeft.y;
    }

    return windowDimensions;
}
#endif