#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinUser.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include "include/homescreen.h"
#include "include/resource.h"
#include "include/colors.h"
#include "include/button.h"
#include "../Shared/common/include/dimensions.h"
#include "../Shared/common/include/buffer.h"
#include "../Shared/common/include/primitive_types.h"
#include "../Shared/common/include/font.h"
#include "../Shared/common/include/mouse.h"
#include "../Shared/common/include/debug.h"
#include "../Shared/common/include/level.h"
#include "../Shared/common/include/file.h"
#include "../Shared/common/include/window.h"


#define TILE_SIZE       16
#define LEVEL_COLS      16
#define LEVEL_ROWS      16
#define LEVEL_LAYERS    32
#define ASSET_PANEL_W   160
#define LAYER_PANEL_W   32
#define WINDOW_CLASS    "BareBonesLevelEditorWindow"
#define WINDOW_TITLE    "Bare-Bones Level Editor"

static HANDLE           hCascadiaFontResource = NULL;
static HFONT            cascasiaRegularFontHandle = NULL;
private_global_variable OffscreenBuffer WindowBackBuffer;

void            InitializeSystem();
void            UpdateApplicationWindow(HDC devicecontext, Dimensions clientRect, OffscreenBuffer buffer);
void            ResizeDIBSection(OffscreenBuffer *buffer, int width, int height);
void            OnHomescreenClose(HomescreenResult result);
static          BITMAPINFO BitmapInfo;
static          void *BitmapMemory;
static          HBITMAP BitmapHandle;
static          HDC BitmapDeviceContext;

int BackBufferWidth = 0;
int BackBufferHeight = 0;

typedef enum ApplicationScreens {
    HOME = 0,
    EDITOR = 1
} ApplicationScreens;

private_global_variable bool ApplicationRunning;
private_global_variable int CurrentApplicationScreen = HOME;

static Font CascadiaFont;

LRESULT CALLBACK WndProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            PlatformWindowInstance.window = windowHandle;
            Dimensions Dimension = GetWin32WindowDimensions();
            HDC dc = GetDC(windowHandle);

            ResizeDIBSection(&WindowBackBuffer, Dimension.width, Dimension.height);
            DrawHomeScreen(WindowBackBuffer, CascadiaFont);
            UpdateApplicationWindow(dc, Dimension, WindowBackBuffer);
            ReleaseDC(windowHandle, dc);
            return 0;
        }

        case WM_SIZE:
        {
            HDC deviceContextHandle = GetDC(windowHandle);
            Dimensions Dimension = GetWin32WindowDimensions();

            ResizeDIBSection(&WindowBackBuffer, Dimension.width, Dimension.height);
            DrawHomeScreen(WindowBackBuffer, CascadiaFont);
            UpdateApplicationWindow(deviceContextHandle, Dimension, WindowBackBuffer);

            ReleaseDC(windowHandle, deviceContextHandle);
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

            Dimensions Dimension = GetWin32WindowDimensions();
            UpdateApplicationWindow(deviceContextHandle, Dimension, WindowBackBuffer);

            EndPaint(windowHandle, &ps);
            return 0;
        }

        case WM_MOUSEMOVE:
        {            
            SetActiveMouseCoordinate((int16_t)(short)LOWORD(lParam), (int16_t)(short)HIWORD(lParam));

            return 0;
        }

		case WM_LBUTTONDOWN:
		{
			SetMouseDownState(MOUSEBUTTON_ONE, (int16_t)(short)LOWORD(lParam), (int16_t)(short)HIWORD(lParam));
            CheckHomescreenClickEvents((int16_t)(short)LOWORD(lParam), (int16_t)(short)HIWORD(lParam));

			#if DEBUG
				PrintMouseButtonState(MOUSEBUTTON_ONE);
			#endif

			return 0;
		}
		case WM_LBUTTONUP:
		{
			SetMouseUpState(MOUSEBUTTON_ONE, (int16_t)(short)LOWORD(lParam), (int16_t)(short)HIWORD(lParam));

			#if DEBUG
				PrintMouseButtonState(MOUSEBUTTON_ONE);
			#endif

			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			SetMouseDownState(MOUSEBUTTON_TWO, (int16_t)(short)LOWORD(lParam), (int16_t)(short)HIWORD(lParam));

			#if DEBUG
				PrintMouseButtonState(MOUSEBUTTON_TWO);
			#endif

			return 0;
		}
		case WM_RBUTTONUP:
		{
			SetMouseUpState(MOUSEBUTTON_TWO, (int16_t)(short)LOWORD(lParam), (int16_t)(short)HIWORD(lParam));

			#if DEBUG
				PrintMouseButtonState(MOUSEBUTTON_TWO);
			#endif

			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			SetMouseDownState(MOUSEBUTTON_THREE, (int16_t)(short)LOWORD(lParam), (int16_t)(short)HIWORD(lParam));

			#if DEBUG
				PrintMouseButtonState(MOUSEBUTTON_THREE);
			#endif

			return 0;
		}
		case WM_MBUTTONUP:
		{
			SetMouseUpState(MOUSEBUTTON_THREE, (int16_t)(short)LOWORD(lParam), (int16_t)(short)HIWORD(lParam));

			#if DEBUG
				PrintMouseButtonState(MOUSEBUTTON_THREE);
			#endif

			return 0;
		}
		case WM_XBUTTONDOWN:
		{
			int xButton = GET_XBUTTON_WPARAM(wParam); 

			if (xButton == XBUTTON1) 
			{
				SetMouseDownState(MOUSEBUTTON_FOUR, (int16_t)(short)LOWORD(lParam), (int16_t)(short)HIWORD(lParam));
			}
			else if (xButton == XBUTTON2)
			{
				SetMouseDownState(MOUSEBUTTON_FIVE, (int16_t)(short)LOWORD(lParam), (int16_t)(short)HIWORD(lParam));
			}

			#if DEBUG
				MouseButton xMouseButton = xButton == XBUTTON1 ? MOUSEBUTTON_FOUR : MOUSEBUTTON_FIVE;
				printf("Mouse %d click triggered, %d, %d\n", 
					xButton == XBUTTON1 ? 4 : 5,
					MouseButtonEventState.buttonState[xMouseButton].xCoordinate, 
					MouseButtonEventState.buttonState[xMouseButton].yCoordinate);
			#endif	

			return 0;
		}
		case WM_XBUTTONUP:
		{
			int xButton = GET_XBUTTON_WPARAM(wParam); 

			if (xButton == XBUTTON1) 
			{
				SetMouseUpState(MOUSEBUTTON_FOUR, (int16_t)(short)LOWORD(lParam), (int16_t)(short)HIWORD(lParam));
			}
			else if (xButton == XBUTTON2)
			{
				SetMouseUpState(MOUSEBUTTON_FIVE, (int16_t)(short)LOWORD(lParam), (int16_t)(short)HIWORD(lParam));
			}			

			#if DEBUG
				MouseButton xMouseButton = xButton == XBUTTON1 ? MOUSEBUTTON_FOUR : MOUSEBUTTON_FIVE;
				printf("Mouse %d click released, %d, %d\n", 
					xButton == XBUTTON1 ? 4 : 5,
					MouseButtonEventState.buttonState[xMouseButton].xCoordinate, 
					MouseButtonEventState.buttonState[xMouseButton].yCoordinate);
			#endif	

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

    return DefWindowProcA(windowHandle, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
    InitializeSystem();
    WNDCLASS WindowClass = {0};

    ResizeDIBSection(&WindowBackBuffer, 1920, 1080);
    
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = WndProc;
    WindowClass.hInstance = hInst;
    // WindowClass.hIcon; TODO: Set if the mood ever strikes.
    WindowClass.lpszClassName = WINDOW_CLASS;

    if (RegisterClassA(&WindowClass))
    {
        PlatformWindowInstance.window =
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

        if (PlatformWindowInstance.window)
        {
            HDC DeviceContext = GetDC(PlatformWindowInstance.window);

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

                switch (CurrentApplicationScreen)
                {
                    case HOME:
                        DrawHomeScreen(WindowBackBuffer, CascadiaFont);
                        break;
                    case EDITOR:
                        break;
                }
                

                Dimensions Dimension = GetWin32WindowDimensions();
                UpdateApplicationWindow(DeviceContext, Dimension, WindowBackBuffer);
            }
        }
    }
    
    return(0);
}

void InitializeSystem()
{
    #if DEBUG
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
	#endif

    // This tells Windows that we don't want it doing dpi conversions for us, we want to be told real pixel sizes and 
    // work off that. This is limited to Windows 10+.
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    CascadiaFont = LoadFont("assets/resources/fonts/Cascadia.ttf", 24.0f);
    InitializeHomescreen(OnHomescreenClose);
}

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

void OnHomescreenClose(HomescreenResult result)
{
    int i = 0; // TODO: Here for debugging - This works - This establishes an architecture for handling results from each screen of the toolkit.
}