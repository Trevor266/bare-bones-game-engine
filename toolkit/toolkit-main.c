#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <WinUser.h>
#include <stdio.h>
#include <math.h>
#include "include/resource.h"
#include "include/colors.h"
#include "include/gdifont.h"
#include "include/buffer.h"
#include "include/primitivetypes.h"
#include "include/button.h"
#include <stdint.h>
#include <stdbool.h>

#define internal static 
#define local_persist static 
#define global_variable static

#define TILE_SIZE       16
#define LEVEL_COLS      16
#define LEVEL_ROWS      16
#define LEVEL_LAYERS    32
#define ASSET_PANEL_W   160
#define LAYER_PANEL_W   32
#define WINDOW_CLASS    "BareBonesLevelEditorWindow"
#define WINDOW_TITLE    "Bare-Bones Level Editor"

// Defines a simple RECT-like structure for storing window dimensions.
typedef struct Dimensions
{
    int height;
    int width;
    int x;
    int y;
} Dimensions;

static HWND         windowHandle;
static HANDLE       hCascadiaFontResource = NULL;
static HFONT        cascasiaRegularFontHandle = NULL;

void            InitializeSystem();
void            RegisterWindowClass(HINSTANCE hInst);
Dimensions      GetWindowDimensions(HWND windowHandle);
void            UpdateApplicationWindow(HDC devicecontext, Dimensions clientRect, OffscreenBuffer buffer);
void            ResizeDIBSection(OffscreenBuffer *buffer, int width, int height);
void            DrawHomeScreen(void);
static          BITMAPINFO BitmapInfo;
static          void *BitmapMemory;
static          HBITMAP BitmapHandle;
static          HDC BitmapDeviceContext;

int BackBufferWidth = 0;
int BackBufferHeight = 0;

global_variable bool ApplicationRunning;
global_variable OffscreenBuffer WindowBackBuffer;

LRESULT CALLBACK WndProc(HWND windowHandle, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            Dimensions Dimension = GetWindowDimensions(windowHandle);
            HDC dc = GetDC(windowHandle);
            ResizeDIBSection(&WindowBackBuffer, Dimension.width, Dimension.height);
            ReleaseDC(windowHandle, dc);

            UpdateApplicationWindow(dc, Dimension, WindowBackBuffer); 
            return 0;
        }

        case WM_SIZE:
        {
            HDC deviceContextHandle = GetDC(windowHandle);
            Dimensions Dimension = GetWindowDimensions(windowHandle);
            UpdateApplicationWindow(deviceContextHandle, Dimension, WindowBackBuffer);

            ResizeDIBSection(&WindowBackBuffer, Dimension.width, Dimension.height);
            DrawHomeScreen();
            ReleaseDC(windowHandle, deviceContextHandle);

            UpdateApplicationWindow(deviceContextHandle, Dimension, WindowBackBuffer);

            InvalidateRect(windowHandle, NULL, TRUE);
            return 0;
        }

        case WM_ERASEBKGND:
        {
            // Eliminates screen flicker
            return 1;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC deviceContextHandle = BeginPaint(windowHandle, &ps);

            Dimensions Dimension = GetWindowDimensions(windowHandle);
            UpdateApplicationWindow(deviceContextHandle, Dimension, WindowBackBuffer);

            EndPaint(windowHandle, &ps);
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            return 0;
        }

        case WM_COMMAND:
        {
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

        case WM_CLOSE:
        {
            ApplicationRunning = false;
        }
    }

    return DefWindowProcA(windowHandle, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
    WNDCLASS WindowClass = {0};

    ResizeDIBSection(&WindowBackBuffer, 1920, 1080);
    
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = WndProc;
    WindowClass.hInstance = hInst;
    // WindowClass.hIcon; TODO: Set if the mood ever strikes.
    WindowClass.lpszClassName = WINDOW_CLASS;

    if (RegisterClassA(&WindowClass))
    {
        windowHandle =
            CreateWindowExA(
                0,
                WindowClass.lpszClassName,
                WINDOW_TITLE,
                WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                0,
                0,
                hInst,
                0);

        if (windowHandle)
        {
            HDC DeviceContext = GetDC(windowHandle);

            int XOffset = 0;
            int YOffset = 0;

            ApplicationRunning = true;
            while(ApplicationRunning)
            {
                MSG Message;

                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if(Message.message == WM_QUIT)
                    {
                        ApplicationRunning = false;
                    }
                    
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }

                DrawHomeScreen();

                Dimensions Dimension = GetWindowDimensions(windowHandle);
                UpdateApplicationWindow(DeviceContext, Dimension, WindowBackBuffer);
            }
        }
    }
    
    return(0);
}

void InitializeSystem()
{
    // This tells Windows that we don't want it doing dpi conversions for us, we want to be told real pixel sizes and 
    // work off that. This is limited to Windows 10+.
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

void RegisterWindowClass(HINSTANCE windowInstance)
{
    WNDCLASSA wc        = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = WndProc;
    wc.hInstance        = windowInstance;
    wc.lpszClassName    = WINDOW_CLASS;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;
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

/////////////////////////////////DEMO CODE/////////////////////////////////
/*
    After using gdi for button rendering, it's become painfully clear how slow it will be for doing all rendering for the editor, 
    This demo code is for testing out some of Casey Muratori's handmade hero renderer handles both converting fonts to bitmaps, and drawing 
    bitmaps to a raw pixel buffer and blitting to screen buffer directly, in theory this would mean the any amount of ui elements could be 
    added for relatively low cost (on their own anyway). This won't be ready for a while, so it lives here.
*/

void ResizeDIBSection(OffscreenBuffer *buffer, int width, int height)
{
    if (buffer->Memory)
    {
        VirtualFree(buffer->Memory, 0, MEM_RELEASE);
    }

    // Write into the buffer's OWN Info field — this is what
    // UpdateApplicationWindow actually passes to StretchDIBits.
    buffer->Info.bmiHeader.biSize        = sizeof(buffer->Info.bmiHeader);
    buffer->Info.bmiHeader.biWidth       = width;
    buffer->Info.bmiHeader.biHeight      = -height; // top-down
    buffer->Info.bmiHeader.biPlanes      = 1;
    buffer->Info.bmiHeader.biBitCount    = 32;
    buffer->Info.bmiHeader.biCompression = BI_RGB;

    buffer->Width  = width;
    buffer->Height = height;

    BackBufferWidth  = width;
    BackBufferHeight = height;

    int BytesPerPixel = 4;
    int BitmapMemorySize = width * height * BytesPerPixel;
    buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    buffer->Pitch  = width * BytesPerPixel;
}

void UpdateApplicationWindow(HDC deviceContextHandle, Dimensions clientRect, OffscreenBuffer buffer)
{
    StretchDIBits(
        deviceContextHandle,
        0, 0, 
        clientRect.width,
        clientRect.height,
        0, 0, BackBufferWidth, BackBufferHeight,
        buffer.Memory,
        &buffer.Info,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

void ClearBufferColor(OffscreenBuffer *Buffer, uint32 Color)
{
    // Cast our buffer to a byte sized buffer so we can do pointer arithmetic on it byte by byte.
    // We will use this to initialize a pointer we do this arithmetic on in the inner for loop, and 
    // we reinitialize it from this value per row, this is so we are 
    uint8 *bufferBytes = (uint8 *)Buffer->Memory;
    
    // Go through each row of the buffer and get each pixel, then set it's color to the provided clear color.
    for (int i = 0; i < Buffer->Height; ++i)
    {
        uint32 *bufferPixels = (uint32 *)bufferBytes;

        for (int j = 0; j < Buffer->Width; ++j)
        {
            // Set the color value, then advance the pointer to the next pixel in the row.
            *bufferPixels = Color;
            bufferPixels++;
        }

        // Advance the buffer byte array to the next row
        bufferBytes += Buffer->Pitch;
    }
}

void DrawHomeScreen(void)
{
    ClearBufferColor(&WindowBackBuffer, 0x00FBD2CB);
    Dimensions windowDimensions = GetWindowDimensions(windowHandle);

    int buttonCount = 3;

    // Use the smaller dimension as the scaling reference, so buttons stay
    // proportionate whether the window is wide, tall, or square.
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