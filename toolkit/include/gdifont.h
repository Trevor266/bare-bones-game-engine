#ifndef _GDIFONT_H
#define _GDIFONT_H
#include <windows.h>
#include <WinUser.h>

// Defines utilities related to working with and building out fonts in GDI. 

/*
    Measures a given string of text in raw pixels. Assumes pixels being provided back from Win32.
*/
SIZE MeasureGdiText(HWND windowHandle, const char *text, HFONT fontHandle);

#endif