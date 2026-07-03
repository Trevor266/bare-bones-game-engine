#ifndef _GDIBUTTON_H
#define _GDIBUTTON_H
#include "windows.h"
#include "gdifont.h"

void CreateDyanmicGdiButton(HWND *buttonHandle, char *buttonText, HWND *parentWindowHandle, LPARAM lp, int x, int y, int leftRightPadding, int topBottomPadding, HFONT buttonFont)
{
    *buttonHandle = CreateWindowA(
        "BUTTON",
        buttonText,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        x,
        y,
        0,
        0,
        *parentWindowHandle,
        (HMENU)(INT_PTR)1,
        ((LPCREATESTRUCT)lp)->hInstance,
        NULL
    );

    // To make sure the font is what we expect when we measure, we need to manually set the font.
    SendMessage(*buttonHandle, WM_SETFONT, (WPARAM)buttonFont, FALSE);

    // Based on the measured text size, set the button position and size.
    SIZE textSize = MeasureGdiText(*buttonHandle, buttonText, buttonFont);
    SetWindowPos(*buttonHandle, NULL, x, y, textSize.cx + leftRightPadding, textSize.cy + topBottomPadding, SWP_NOZORDER);

    return;
}

#endif