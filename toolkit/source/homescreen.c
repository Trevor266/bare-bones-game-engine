#include "../include/homescreen.h"

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
                // Create a new level
                case NEW_LEVEL_BUTTON_ID:
                {
                    // Standup a new level params struct for storing inputs from the new level dialog.
                    NewLevelParams newLevelParameters = { 0 };

                    if(BuildNewLevelDialogPromptAndPrompt(&newLevelParameters) == false)
                    {
                        return;
                    }

                    // If a level with that name already exists, we should ask if it should be overwritten - this will wipe
                    // the current level folder if they do so and generate a blank .bbl file.
                    if (newLevelParameters.buffer[0] == '\0')
                    {
                        // TODO: Add actual validation to more than just the file name - right now the file name is being validated as it causes 
                        // actual directory creation and file generation, the other params are inside the file, which isn't even being used
                        // yet.
                        MessageBoxA(NULL, "Please enter a valid level name", "No Name", MB_OK);
                    }
                    else if (LevelExists(newLevelParameters.buffer))
                    {
                        HandleLevelNamingConflict(&newLevelParameters);
                    }
                    else
                    {
                        CreateLevel
                        (
                            newLevelParameters.buffer,
                            newLevelParameters.layerCount,
                            newLevelParameters.tileWidth,
                            newLevelParameters.tileHeight,
                            newLevelParameters.levelWidth,
                            newLevelParameters.levelHeight
                        );
                    }
                    
                    break;
                }
                case LOAD_LEVEL_BUTTON_ID:
                {
                    MessageBoxA(NULL, "load level!", "test", MB_OK);
                    break;
                }
                case SETTINGS_BUTTON_ID:
                {
                    MessageBoxA(NULL, "settings!", "test", MB_OK);
                    break;
                }
            }
        }
    }
}

// Returns true when the level data was filled out successfully, false for cancellation or failure.
bool BuildNewLevelDialogPromptAndPrompt(NewLevelParams *params)
{
    // Create new level form from resource.rc resource.
    INT_PTR levelCreationResult = DialogBoxParamA
    (
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDD_NEW_LEVEL_NAME),
        PlatformWindowInstance.window,
        NewLevelNameDialogProc,
        (LPARAM)params
    );

    if (levelCreationResult != IDOK)
    {
        // TODO: Abstract this very simple ok dialog into a platform agnostic one that takes two args for strings.
        MessageBoxW
        (
            NULL,
            L"Level Creation Cancelled",
            L"Cancelled",
            MB_ICONEXCLAMATION | MB_OK
        );

        return false;
    }

    return true;
}

INT_PTR CALLBACK NewLevelNameDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static NewLevelParams* params;

    switch (message)
    {
        case WM_INITDIALOG:
            params = (NewLevelParams*)lParam;
            SetWindowLongPtr(hwndDlg, DWLP_USER, (LONG_PTR)params);

            SetDlgItemInt(hwndDlg, IDC_LAYER_COUNT_EDIT, 32, FALSE);
            SetDlgItemInt(hwndDlg, IDC_TILE_WIDTH_EDIT, 16, FALSE);
            SetDlgItemInt(hwndDlg, IDC_TILE_HEIGHT_EDIT, 16, FALSE);
            SetDlgItemInt(hwndDlg, IDC_LEVEL_WIDTH_EDIT, 4096, FALSE);
            SetDlgItemInt(hwndDlg, IDC_LEVEL_HEIGHT_EDIT, 4096, FALSE);

            return TRUE;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDOK:
                {
                    params = (NewLevelParams*)GetWindowLongPtr(hwndDlg, DWLP_USER);

                    GetDlgItemTextA(hwndDlg, IDC_LEVEL_NAME_EDIT, params->buffer, sizeof(params->buffer));

                    BOOL ok;
                    params->layerCount  = (int)GetDlgItemInt(hwndDlg, IDC_LAYER_COUNT_EDIT, &ok, TRUE);
                    params->tileWidth   = (int)GetDlgItemInt(hwndDlg, IDC_TILE_WIDTH_EDIT, &ok, TRUE);
                    params->tileHeight  = (int)GetDlgItemInt(hwndDlg, IDC_TILE_HEIGHT_EDIT, &ok, TRUE);
                    params->levelWidth  = (int)GetDlgItemInt(hwndDlg, IDC_LEVEL_WIDTH_EDIT, &ok, TRUE);
                    params->levelHeight = (int)GetDlgItemInt(hwndDlg, IDC_LEVEL_HEIGHT_EDIT, &ok, TRUE);

                    EndDialog(hwndDlg, IDOK);
                    return TRUE;
                }
                case IDCANCEL:
                {
                    EndDialog(hwndDlg, IDCANCEL);
                    return TRUE;
                }
            }
            
            break;
    }

    return FALSE;
}

void HandleLevelNamingConflict(NewLevelParams *params)
{
    #define nameConflictBufferSize 512

    char message[nameConflictBufferSize];
    char caption[nameConflictBufferSize];
    snprintf(message, nameConflictBufferSize, "A level named %s already exists.\nDo you want to replace it? This cannot be undone.", params->buffer);
    snprintf(caption, nameConflictBufferSize, "Overwrite %s?", params->buffer);

    #undef nameConflictBufferSize
    
    int overwriteLevelResult = MessageBoxA
    (
        NULL,
        message,
        caption,
        MB_ICONEXCLAMATION | MB_YESNO
    );

    if (overwriteLevelResult == IDYES)
    {
        CreateLevel
        (
            params->buffer,
            params->layerCount,
            params->tileWidth,
            params->tileHeight,
            params->levelWidth,
            params->levelHeight
        );
    }
    else
    {
        MessageBoxA
        (
            NULL,
            "Level Creation Cancelled",
            "Cancelled",
            MB_ICONEXCLAMATION | MB_OK
        );
    }

    return;
}