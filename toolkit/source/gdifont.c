#include "../include/gdifont.h"

/*
    Measures a given string of text in raw pixels. Assumes pixels being provided back from Win32.
*/
SIZE MeasureGdiText(HWND windowHandle, const char *text, HFONT fontHandle)
{
    HDC hdc = GetDC(windowHandle);
    HFONT previousFont = (HFONT)SelectObject(hdc, fontHandle);

    SIZE size = {0};
    GetTextExtentPoint32A(hdc, text, strlen(text), &size);

    SelectObject(hdc, previousFont);
    ReleaseDC(windowHandle, hdc);

    return size;
}

HFONT LoadCustomFont(int pointSize, char *fontName, HANDLE *pFontResourceHandle, int fontResourceId)
{
    // Find the specified resource in the exectuable
    HRSRC fontResourceHandle = FindResource(NULL, MAKEINTRESOURCE(fontResourceId), RT_RCDATA);
    if (!fontResourceHandle)
    {
        return NULL;
    }

    // Load resource into memory
    HGLOBAL hGlobal = LoadResource(NULL, fontResourceHandle);
    if (!hGlobal)
    {
        return NULL;
    }

    void  *pData = LockResource(hGlobal);
    DWORD  size  = SizeofResource(NULL, fontResourceHandle);

    // Register the font with gdi
    DWORD nFonts = 0;
    pFontResourceHandle = AddFontMemResourceEx(pData, size, NULL, &nFonts);

    if (!pFontResourceHandle || nFonts == 0)
    {
        return NULL;
    } 

    // Create a logical font
    // Negative height = point size (GDI convention)
    return CreateFontA(
        -pointSize,          // height in points (negative = points)
        0,                   // width (0 = auto)
        0,                   // escapement
        0,                   // orientation
        FW_NORMAL,           // weight
        FALSE,               // italic
        FALSE,               // underline
        FALSE,               // strikeout
        DEFAULT_CHARSET,
        OUT_TT_PRECIS,       // prefer TrueType
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        fontName
    );
}