#include "../../Shared/common/include/primitivetypes.h"
#include "../../Shared/common/include/dimensions.h"
#include "../include/buffer.h"
#include "../include/button.h"
#include "../include/homescreen.h"
#include <windows.h>



void DrawHomeScreen(HWND windowHandle)
{
    // FIRST - Move window back buffer into a globally available variable in buffer.c, then all future buffers can just be pulled from here.
    ClearBufferColor(&WindowBackBuffer, 0x00FBD2CB);
    Dimensions windowDimensions = GetWin32WindowDimensions(windowHandle);

    int buttonCount = 3;

    // Use the smaller dimension as the scaling reference, so buttons maintain 
    // their intended aspect ratio independent of the client aspect ratio.
    int referenceDimension = (windowDimensions.width < windowDimensions.height)
        ? windowDimensions.width
        : windowDimensions.height;

    int buttonWidth  = (int)(referenceDimension * 0.30f);
    int buttonHeight = (int)(referenceDimension * 0.08f);

    int buttonSpacing = buttonHeight;

    int buttonStackHeight = (buttonHeight * buttonCount) + ((buttonCount + 1) * buttonSpacing);

    /*
        Centering the button stack visualized:
        window = 34 dashes wide
        button = 14 dashes wide
        left/right margin = 10 dashes wide
        -----------------|-----------------
        **********-------|-------**********             
        Here, the window is 34 wide, to center the button which is 14 wide, we need 
        the halfway point of the window, subtracted by half the width of the button to determine the margin for both sides, 
        which in this case corresponds to the x position as wel
        To get the centered Y position is the same thing but we need to (height of buttons * count of buttons) / 2 for the y position.
    */
    int buttonStackX = (windowDimensions.width  / 2) - (buttonWidth / 2);
    int buttonStackY = (windowDimensions.height / 2) - (buttonStackHeight / 2);

    DrawClientSpaceBox(&WindowBackBuffer, buttonStackX, buttonStackY, buttonWidth, buttonStackHeight, 0x00BDA9A4);

    for (int i = 0; i < buttonCount; ++i)
    {
        int buttonY = buttonStackY + buttonSpacing + (i * (buttonHeight + buttonSpacing));
        DrawClientSpaceBox(&WindowBackBuffer, buttonStackX, buttonY, buttonWidth, buttonHeight, 0x00F27B66);
    }
}