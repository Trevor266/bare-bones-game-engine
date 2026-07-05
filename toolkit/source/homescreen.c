#include "../../Shared/common/include/primitivetypes.h"
#include "../../Shared/common/include/dimensions.h"
#include "../../Shared/common/include/buffer.h"
#include "../../Shared/common/include/font.h"
#include "../include/button.h"
#include "../include/homescreen.h"
#include <windows.h>

extern Button homescreenButtons[HOMESCREEN_BUTTON_COUNT] = 
{
    {
        .text            = "New Level",
        .textColor       = 0x00202020,
        .backgroundColor = 0x00F27B66,
        .hoverColor      = 0,
        .dimensions      = {0},
    },
    {
        .text            = "Load Level",
        .textColor       = 0x00202020,
        .backgroundColor = 0x00F27B66,
        .hoverColor      = 0,
        .dimensions      = {0},
    },
    {
        .text            = "Settings",
        .textColor       = 0x00202020,
        .backgroundColor = 0x00F27B66,
        .hoverColor      = 0,
        .dimensions      = {0},
    }
};

void DrawHomeScreen(HWND windowHandle, OffscreenBuffer WindowBackBuffer, Font font)
{
    // FIRST - Move window back buffer into a globally available variable in buffer.c, then all future buffers can just be pulled from here.
    ClearBufferColor(&WindowBackBuffer, 0x00FBD2CB);
    Dimensions windowDimensions = GetWin32WindowDimensions(windowHandle);

    // Use the smaller dimension as the scaling reference, so buttons maintain 
    // their intended aspect ratio independent of the client aspect ratio.
    int referenceDimension = (windowDimensions.width < windowDimensions.height)
        ? windowDimensions.width
        : windowDimensions.height;

    int buttonWidth  = (int)(referenceDimension * 0.30f);
    int buttonHeight = (int)(referenceDimension * 0.08f);

    int buttonSpacing = buttonHeight / 3;

    int buttonStackHeight = (buttonHeight * HOMESCREEN_BUTTON_COUNT) + ((HOMESCREEN_BUTTON_COUNT + 1) * buttonSpacing);

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

    //TODO: Find some way to abstract out the font and client box rendering into a single button function call.
    for (int i = 0; i < HOMESCREEN_BUTTON_COUNT; ++i)
    {
        int buttonY = buttonStackY + buttonSpacing + (i * (buttonHeight + buttonSpacing));
        DrawClientSpaceBox(&WindowBackBuffer, buttonStackX, buttonY, buttonWidth, buttonHeight, homescreenButtons[i].backgroundColor);

        int textWidth = MeasureTextWidth(&font, homescreenButtons[i].text);
        int textX = buttonStackX + (buttonWidth / 2) - (textWidth / 2);
 
        int textY = buttonY + (buttonHeight / 2) + font.verticalCenterOffset;

        DrawCustomText(&WindowBackBuffer, &font, homescreenButtons[i].text, textX, textY, homescreenButtons[i].textColor);

        // Update button dimensions.
        homescreenButtons[i].dimensions.y           = buttonY;
        homescreenButtons[i].dimensions.x           = buttonStackX;
        homescreenButtons[i].dimensions.width       = buttonWidth;
        homescreenButtons[i].dimensions.height      = buttonHeight;
    }
    
    DrawCustomText(&WindowBackBuffer, &font, "Testing how good this rendering really is....", 30, 200, 0x00FF00FF);
    DrawCustomText(&WindowBackBuffer, &font, "here's a newline to really give it a whirl!", 30, 200 + font.fontSize, 0x0000FFFF);
}

void CheckHomescreenClickEvents(int hitX, int hitY)
{
    for (int buttonId = 0; buttonId < HOMESCREEN_BUTTON_COUNT; ++buttonId)
    {
        if (ButtonHitTest(homescreenButtons[buttonId], hitX, hitY))
        {
            switch(buttonId)
            {
                case NEW_LEVEL_BUTTON_ID:
                {
                    MessageBoxW(NULL, L"new level!", L"test", MB_OK);
                    break;
                }
                case LOAD_LEVEL_BUTTON_ID:
                {
                    MessageBoxW(NULL, L"load level!", L"test", MB_OK);
                    break;
                }
                case SETTINGS_BUTTON_ID:
                {
                    MessageBoxW(NULL, L"settings!", L"test", MB_OK);
                    break;
                }
            }
        }
    }
}