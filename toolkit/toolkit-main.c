#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <WinUser.h>
#include <stdio.h>
#include <math.h>
#include "include/resource.h"
#include "include/colors.h"
#include "include/gdifont.h"
#include <stdint.h>

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

typedef struct
{
    RECT        rect;
    const char *label;
    int         id;
} EditorButton;

static HWND         windowHandle;
static EditorButton landingScreenButtons[3];
static int          landingScreenButtonCount = 3;
static HANDLE       hCascadiaFontResource = NULL;
static HFONT        cascasiaRegularFontHandle = NULL;

void            InitializeSystem();
void            RegisterWindowClass(HINSTANCE hInst);
Dimensions      GetWindowDimensions(HWND windowHandle);

// DEMO ONLY:
void            Demo(HDC windowDeviceContextHandle, HWND hwnd, int width, int height);
void            DemoTwo(void);
void            PaintWindow(HDC deviceContext, int windowWidth, int windowHeight);
static          BITMAPINFO BitmapInfo;
static          void *BitmapMemory;
static          HBITMAP BitmapHandle;
static          HDC BitmapDeviceContext;


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            RECT rect;
            GetClientRect(hwnd, &rect);
            HDC dc = GetDC(hwnd);
            Demo(dc, hwnd, rect.right - rect.left, rect.bottom - rect.top);
            ReleaseDC(hwnd, dc);

            DemoTwo(); 
            return 0;
        }

        case WM_SIZE:
        {
            RECT rect;
            GetClientRect(hwnd, &rect);
            HDC dc = GetDC(hwnd);
            Demo(dc, hwnd, rect.right - rect.left, rect.bottom - rect.top); // recreate DIB at new size
            ReleaseDC(hwnd, dc);

            DemoTwo();

            InvalidateRect(hwnd, NULL, TRUE);
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
            HDC deviceContextHandle = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);
            PaintWindow(deviceContextHandle, rect.right - rect.left, rect.bottom - rect.top);

            EndPaint(hwnd, &ps);
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
    }

    return DefWindowProcA(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
    InitializeSystem();
    RegisterWindowClass(hInst);

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

void RegisterWindowClass(HINSTANCE windowInstance)
{
    WNDCLASSA wc        = {0};
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
typedef uint8_t  u8;
typedef uint32_t u32;
typedef int32_t  s32;
typedef float    r32;
int BitmapWidth;
int BitmapHeight;

void Demo(HDC windowDeviceContextHandle, HWND hwnd, int width, int height)
{
    if (BitmapHandle)
    {
        DeleteObject(BitmapHandle);
    }
    else
    {
        BitmapDeviceContext = CreateCompatibleDC(0);
    }
 
    BitmapInfo.bmiHeader.biSize        = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth       = width;
    BitmapInfo.bmiHeader.biHeight      = -height; // top-down
    BitmapInfo.bmiHeader.biPlanes      = 1;
    BitmapInfo.bmiHeader.biBitCount    = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;
    BitmapInfo.bmiHeader.biSizeImage      = 0;
    BitmapInfo.bmiHeader.biXPelsPerMeter  = 0;
    BitmapInfo.bmiHeader.biYPelsPerMeter  = 0;
    BitmapInfo.bmiHeader.biClrUsed        = 0;
    BitmapInfo.bmiHeader.biClrImportant   = 0;
 
    BitmapHandle = CreateDIBSection(
        windowDeviceContextHandle,
        &BitmapInfo,
        DIB_RGB_COLORS,
        &BitmapMemory,
        0,
        0);
 
    BitmapWidth  = width;
    BitmapHeight = height;
}
 
static u32 PackColor(u8 r, u8 g, u8 b, u8 a)
{
    return ((u32)a << 24) | ((u32)r << 16) | ((u32)g << 8) | (u32)b;
}
 
static void PutPixel(int x, int y, u32 color)
{
    if (x < 0 || y < 0 || x >= BitmapWidth || y >= BitmapHeight) return;
    u32 *pixel = (u32 *)BitmapMemory + y * BitmapWidth + x;
    *pixel = color;
}
 
static void DrawBackground(void)
{
    u8 topR = 0x1A, topG = 0x1B, topB = 0x1E;
    u8 botR = 0x26, botG = 0x27, botB = 0x2C;
 
    for (int y = 0; y < BitmapHeight; ++y)
    {
        r32 t = (r32)y / (r32)(BitmapHeight - 1);
        u8 r = (u8)(topR + (botR - topR) * t);
        u8 g = (u8)(topG + (botG - topG) * t);
        u8 b = (u8)(topB + (botB - topB) * t);
        u32 rowColor = PackColor(r, g, b, 255);
        for (int x = 0; x < BitmapWidth; ++x)
            PutPixel(x, y, rowColor);
    }
}
 
void DemoTwo(void)
{
    if (!BitmapMemory) return;
 
    DrawBackground();
}

void PaintWindow(HDC deviceContext, int windowWidth, int windowHeight)
{
    StretchDIBits(
        deviceContext,
        0, 0, windowWidth, windowHeight,
        0, 0, BitmapWidth, BitmapHeight,
        BitmapMemory,
        &BitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY);
}