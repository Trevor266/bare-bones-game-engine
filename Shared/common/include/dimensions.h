typedef struct Dimensions
{
    int height;
    int width;
    int x;
    int y;
} Dimensions;

// TODO: This is just a stab at abstracting some shared functionality that has platform specific implementations, 
// This is a quick and dirty way of doing this - If it's win32, include the relevant headers and function stubs.
#if _WIN32
#include <windows.h>

extern Dimensions GetWin32WindowDimensions(HWND windowHandle);

#endif