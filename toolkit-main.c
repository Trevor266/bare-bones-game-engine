#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h> 
#include <stdio.h>

#define TILE_SIZE       16
#define LEVEL_COLS      16
#define LEVEL_ROWS      16
#define LEVEL_LAYERS    32
#define ASSET_PANEL_W   160
#define LAYER_PANEL_W   32
#define STATUS_H        20
#define WINDOW_CLASS    "BareBonesLevelEditorWindow"
#define WINDOW_TITLE    "Bare-Bones level editor"

// Defines a simple RECT-like structure for storing window dimensions.
typedef struct Dimensions
{
    int height;
    int width;
    int x;
    int y;
} Dimensions;

void InitializeSystem();
void RegisterWindow(HINSTANCE windowInstance);
SIZE MeasureText(HWND windowHandle, const char *text);
void CreateDyanmicButton(HWND *buttonHandle, char *buttonText, HWND *parentWindowHandle, LPARAM lp, int x, int y, int leftRightPadding, int topBottomPadding);
Dimensions GetWindowDimensions(HWND windowHandle);
void LayoutHomeScreenButtonStack(HWND *parentWindow, LPARAM lp);

static HWND     windowHandle;
static HWND     newLevelButtonHandle;
static HWND     loadLevelButtonHandle;
static HWND     settingsButtonHandle;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
        case WM_CREATE:
        {
            // This lays out the buttons for the home screen.
            LayoutHomeScreenButtonStack(&hwnd, lp);

            return 0;
        }

        case WM_COMMAND:
        {
            if (LOWORD(wp) == 1)
                MessageBoxA(hwnd, "Button was clicked!", "Hello", MB_OK);
            return 0;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_MOUSEWHEEL:
        {
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            return 0;
        }

        case WM_RBUTTONUP:
        {
            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            return 0;
        }

        case WM_SIZE:
        {
            return 0;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    }

    //
    return DefWindowProcA(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
    InitializeSystem();
    RegisterWindow(hInst);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return (int)msg.wParam;
}

void InitializeSystem()
{
    // This tells Windows that we don't want it doing dpi conversions for us, we want to be told real pixel sizes and 
    // work off that. This is limited to Windows 10+.
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

void RegisterWindow(HINSTANCE windowInstance)
{
    WNDCLASSA wc = {0};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = windowInstance;
    wc.lpszClassName = WINDOW_CLASS;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClassA(&wc);

    windowHandle = CreateWindowA(
        WINDOW_CLASS,
        WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1920,
        1080,
        NULL,
        NULL,
        windowInstance,
        NULL);
}

// Random unsorted utilities
/*
    Generates a dynamically sized button at the given handle.
*/
void CreateDyanmicButton(HWND *buttonHandle, char *buttonText, HWND *parentWindowHandle, LPARAM lp, int x, int y, int leftRightPadding, int topBottomPadding)
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
    HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    SendMessage(*buttonHandle, WM_SETFONT, (WPARAM)hFont, FALSE);

    // Based on the measured text size, set the button position and size.
    SIZE textSize = MeasureText(*buttonHandle, buttonText);
    SetWindowPos(*buttonHandle, NULL, x, y, textSize.cx + leftRightPadding, textSize.cy + topBottomPadding, SWP_NOZORDER);

    return;
}

/*
    Measures a given string of text in raw pixels. Assumes pixels being provided back from Win32.
*/
SIZE MeasureText(HWND windowHandle, const char *text)
{
    HDC deviceContextHandle = GetDC(windowHandle);
    HFONT hFont = (HFONT)SendMessage(windowHandle, WM_GETFONT, 0, 0);

    if (!hFont)
    {
        // If there isn't a custom font set on this window, get the system font.
        hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    }

    // We now need to put the font we pulled from the window into the device context. We need to track the current font that was 
    // in device context before we swapped ours in so we can put it back when we're done.
    HFONT previousDeviceContextFont = (HFONT)SelectObject(deviceContextHandle, hFont);

    SIZE size = {0};
    GetTextExtentPoint32A(deviceContextHandle, text, strlen(text), &size);

    SelectObject(deviceContextHandle, previousDeviceContextFont);
    ReleaseDC(windowHandle, deviceContextHandle);

    return size;
}

Dimensions GetWindowDimensions(HWND windowHandle)
{
    Dimensions windowDimensions = {0};
    RECT rect;

    // When we get a window rect, it's x/y are not based on the client area of the window, they are based on the raw pixel position 
    // of the monitor, screen space based, we must convert to a client screen space for the caller, we do this by checking if the window is a parent window, 
    // or if it is a child of a parent. If it's a child window, that means there is a parent window, and we can use ScreenToClient to convert
    // the parent screen coordinate into a coordinate relative to the parent's client coordinate origin (0,0). 
    // If it's a parent window, we can just return the dimensions as-is, as the screen coordinates are the best we can calculate.
    if (GetWindowRect(windowHandle, &rect))
    {
        HWND parent = GetParent(windowHandle);
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

void LayoutHomeScreenButtonStack(HWND *parentWindow, LPARAM lp)
{
    // Get the parent window size in pixels for the height and width.
    RECT clientRect;
    GetClientRect(*parentWindow, &clientRect);
    int clientWidth = clientRect.right;
    int clientHeight = clientRect.bottom;

    // For the home screen, we have 3 buttons - new, load, settings. These should be centered on the screen and sized appropriately.
    
    // First, generate the 3 buttons we need. We are doing two things here - We are sizing each button with dynamic padding and width/height, 
    // and then we get the dimensions we just generated, and use them to space the next button relative to the first buttons positions. We do this 
    // for the second button as well so we can position the third button relatively, and so on.
    CreateDyanmicButton
    (
        &newLevelButtonHandle,
        "New Level",
        parentWindow,
        lp,
        0,
        57,
        60,
        20
    );
    Dimensions newLevelButtonDimensions = GetWindowDimensions(newLevelButtonHandle);

    CreateDyanmicButton
    (
        &loadLevelButtonHandle,
        "Load Level",
        parentWindow,
        lp,
        0,
        newLevelButtonDimensions.y + newLevelButtonDimensions.height + 5,
        60,
        20
    );
    Dimensions loadLevelButtonDimensions = GetWindowDimensions(loadLevelButtonHandle);

    CreateDyanmicButton(
        &settingsButtonHandle,
        "Settings",
        parentWindow,
        lp,
        0,
        loadLevelButtonDimensions.y + loadLevelButtonDimensions.height + 5,
        60,
        20
    );
    Dimensions settingsButtonDimensions = GetWindowDimensions(settingsButtonHandle);

    // First, calculate the screen height, plus calculate the top of the first button and the bottom of the last button, then divide the remainder by 2 to 
    // determine our new Y coordinate that we will reposition everything with.
    int centeredYPosition = (clientHeight - ((settingsButtonDimensions.y + settingsButtonDimensions.height) - newLevelButtonDimensions.y)) / 2;

    // The dynamic sizing pass for these buttons gave us the required widths to display properly, but to align these buttons nicely, we get the width of the 
    // largest button calculated, and size all other buttons to this size so they align evenly.
    int buttonCount = 3;
    int buttonWidths[] = { newLevelButtonDimensions.width, loadLevelButtonDimensions.width, settingsButtonDimensions.width };
    int widestButtonWidth = 0;

    for (int i = 0; i < buttonCount; i++)
    {
        if (buttonWidths[i] > widestButtonWidth)
        {
            widestButtonWidth = buttonWidths[i];
        }
    }

    int centeredXPosition = (clientWidth - ((settingsButtonDimensions.x + widestButtonWidth) - newLevelButtonDimensions.x)) / 2;
    SetWindowPos(newLevelButtonHandle, NULL, centeredXPosition, centeredYPosition, widestButtonWidth, newLevelButtonDimensions.height, SWP_NOZORDER);
    newLevelButtonDimensions = GetWindowDimensions(newLevelButtonHandle);

    SetWindowPos(loadLevelButtonHandle, NULL, centeredXPosition, newLevelButtonDimensions.y + newLevelButtonDimensions.height + 5, widestButtonWidth, loadLevelButtonDimensions.height, SWP_NOZORDER);
    loadLevelButtonDimensions = GetWindowDimensions(loadLevelButtonHandle);

    SetWindowPos(settingsButtonHandle, NULL, centeredXPosition, loadLevelButtonDimensions.y + loadLevelButtonDimensions.height + 5, widestButtonWidth, settingsButtonDimensions.height, SWP_NOZORDER);
}
