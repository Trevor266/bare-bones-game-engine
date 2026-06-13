#ifndef _WINDOWH
#define _WINDOWH

#include "pixelbuffer.h"

// A structure representing shared parameters relevant to window creation that either Linux or Win32 would 
// care about when requesting an os window.
typedef struct WindowCreationParameters {
    PixelBuffer*    cursorImage;
    PixelBuffer*    iconImage;
    char*           title;
} WindowCreationParameters;

// A structure representing the opaque platform specific window. For both Linux and Win32, a pointer of a platform specific type is 
// provided back during window creation and is used to call into platform specific functions relevant to working with the window.
// This platform type is abstracted to a void pointer - It can either be manually cast back to a platform type, or via provided convinience macros.
typedef struct PlatformWindow {
    void* window;
} PlatformWindow;

extern PlatformWindow PlatformWindowInstance;

// Convinience macros
#define GET_PLATFORMWINDOW_HWND() ((HWND)PlatformWindowInstance.window)

#endif