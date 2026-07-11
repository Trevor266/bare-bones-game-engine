#include "../include/dimensions.h"

#ifdef _WIN32
#include <windows.h>

Dimensions GetWin32WindowDimensions()
{
    Dimensions windowDimensions = {0};
    RECT rect;

    // When we get a window rect, it's x/y are not based on the client area of the window, they are based on the raw pixel position 
    // of the monitor, screen space based, we must convert to a client screen space for the caller, we do this by checking if the window is a parent window, 
    // or if it is a child of a parent. If it's a child window, that means there is a parent window, and we can use ScreenToClient to convert
    // the parent screen coordinate into a coordinate relative to the parent's client coordinate origin (0,0). 
    // If it's a parent window, we can just return the dimensions as-is, as the screen coordinates are the best we can calculate.
    if (GetWindowRect(PlatformWindowInstance.window, &rect))
    {
        HWND parent = GetParent(PlatformWindowInstance.window);
        POINT topLeft = { rect.left, rect.top };

        if (parent)
        {
            // Get the top left point of the screen, we need this to tell windows what we want to convert to client space.
            // We check this call here because technically it could fail. To be honest I'm not really sure how it would.
            if (!ScreenToClient(parent, &topLeft)) 
            {
                // If we're unable to convert screen to client dimensions, then the remaining calculations 
                // can't be trusted so return what we have.
                return windowDimensions;
            };

            windowDimensions.x = topLeft.x;
            windowDimensions.y = topLeft.y;
        }
        else
        {
            windowDimensions.x = rect.left;
            windowDimensions.y = rect.top;
        }

        windowDimensions.width  = rect.right - rect.left;
        windowDimensions.height = rect.bottom - rect.top;
    }

    return windowDimensions;
}
#endif