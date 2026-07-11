#include "../../Shared/common/include/primitive_types.h"
#include "../../Shared/common/include/primitive_geometry.h"
#include "../../Shared/common/include/dimensions.h"
#include "../../Shared/common/include/buffer.h"
#include "../../Shared/common/include/font.h"
#include "../../Shared/common/include/mouse.h"
#include "../../Shared/common/include/level.h"
#include "../../Shared/common/include/window.h"
#include "../include/button.h"
#include "../include/homescreen.h"
#include <windows.h>
#include "../include/resource.h"

typedef struct {
    char *buffer;
    size_t bufferSize;
} NewLevelNameParams;

Button homescreenButtons[HOMESCREEN_BUTTON_COUNT] = 
{
    {
        .text            = "New Level",
        .textColor       = 0x00202020,
        .backgroundColor = 0x00F8AE85,
        .borderWidth     = 2,
        .borderColor     = 0x009C6D53,
        .borderRadius    = 8,
        .hoverColor      = 0x009C6D53,
        .hovered         = false,
        .dimensions      = {0},
    },
    {
        .text            = "Load Level",
        .textColor       = 0x00202020,
        .backgroundColor = 0x00F89785,
        .borderWidth     = 2,
        .borderColor     = 0x00AD695D,
        .borderRadius    = 8,
        .hoverColor      = 0x00AD695D,
        .hovered         = false,
        .dimensions      = {0},
    },
    {
        .text            = "Settings",
        .textColor       = 0x00202020,
        .backgroundColor = 0x00F27B66,
        .borderWidth     = 2,
        .borderColor     = 0x00A95647,
        .borderRadius    = 8,
        .hoverColor      = 0x00A95647,
        .hovered         = false,
        .dimensions      = {0},
    }
};

void DrawHomeScreen(OffscreenBuffer WindowBackBuffer, Font font)
{
    // FIRST - Move window back buffer into a globally available variable in buffer.c, then all future buffers can just be pulled from here.
    ClearBufferColor(&WindowBackBuffer, 0x00FBD2CB);
    Dimensions windowDimensions = GetWin32WindowDimensions();

    // Use the smaller dimension as the scaling reference, so buttons maintain 
    // their intended aspect ratio independent of the client aspect ratio.
    int referenceDimension = (windowDimensions.width < windowDimensions.height)
        ? windowDimensions.width
        : windowDimensions.height;

    int buttonStackLeftRightPadding = 40;

    int buttonWidth  = (int)(referenceDimension * 0.30f);
    int buttonHeight = (int)(referenceDimension * 0.08f);

    int buttonSpacing = 40;

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
        which in this case corresponds to the x position as well
        To get the centered Y position is the same thing but we need to (height of buttons * count of buttons) / 2 for the y position.
    */
    int buttonStackX = (windowDimensions.width  / 2) - (buttonWidth / 2) - buttonStackLeftRightPadding;
    int buttonStackWidth = buttonWidth + (buttonStackLeftRightPadding * 2);
    int buttonStackY = (windowDimensions.height / 2) - (buttonStackHeight / 2);
    int buttonX = buttonStackX + buttonStackLeftRightPadding;

    RenderQuad(WindowBackBuffer.Memory, WindowBackBuffer.Width, WindowBackBuffer.Height, WindowBackBuffer.Pitch, buttonStackX, buttonStackY, buttonStackWidth, buttonStackHeight, 8, 0x00FFE5E0);

    for (int i = 0; i < HOMESCREEN_BUTTON_COUNT; ++i)
    {
        int buttonY = buttonStackY + buttonSpacing + (i * (buttonHeight + buttonSpacing));

        // Update button dimensions.
        homescreenButtons[i].dimensions.y           = buttonY;
        homescreenButtons[i].dimensions.x           = buttonX;
        homescreenButtons[i].dimensions.width       = buttonWidth;
        homescreenButtons[i].dimensions.height      = buttonHeight;
        homescreenButtons[i].hovered                = ButtonHitTest(homescreenButtons[i], MouseButtonEventState.lastX, MouseButtonEventState.lastY);

        RenderButton(homescreenButtons[i], WindowBackBuffer.Memory, WindowBackBuffer.Width, WindowBackBuffer.Height, WindowBackBuffer.Pitch, font);
    }
    
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
                    char levelName[LEVEL_NAME_MAX] = {0};
                    NewLevelNameParams params = { .buffer = levelName, .bufferSize = sizeof(levelName) };

                    INT_PTR result = DialogBoxParamA(
                        GetModuleHandle(NULL),
                        MAKEINTRESOURCE(IDD_NEW_LEVEL_NAME),
                        PlatformWindowInstance.window,
                        NewLevelNameDialogProc,
                        (LPARAM)&params
                    );

                    CreateLevel(params.buffer, 32, 16, 16, 4096, 4096);
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

INT_PTR CALLBACK NewLevelNameDialogProc(HWND dialogHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            SetWindowLongPtrA(dialogHandle, GWLP_USERDATA, (LONG_PTR)lParam);
            SetFocus(GetDlgItem(dialogHandle, IDC_LEVEL_NAME_EDIT));
            return (INT_PTR)FALSE;
        }
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDOK:
                {
                    NewLevelNameParams *params =(NewLevelNameParams *)GetWindowLongPtrA(dialogHandle, GWLP_USERDATA);

                    if (params != NULL)
                    {
                        GetDlgItemTextA(dialogHandle, IDC_LEVEL_NAME_EDIT, params->buffer, (int)params->bufferSize);
                    }

                    EndDialog(dialogHandle, (INT_PTR)IDOK);
                    return (INT_PTR)TRUE;
                }
                case IDCANCEL:
                {
                    EndDialog(dialogHandle, (INT_PTR)IDCANCEL);
                    return (INT_PTR)TRUE;
                }
            }
            break;
        }
    }
    return (INT_PTR)FALSE;
}