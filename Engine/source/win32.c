#include "../../Shared/common/include/mouse.h"
#include "../../Shared/common/include/debug.h"

#include "../include/keyboard.h"
#include "../include/win32.h"
#include "../include/controller.h"
#include "../../Shared/common/include/window.h"
#include "../../Shared/common/include/bitmap.h"
#include "../../Shared/common/include/file.h"
#include "../include/engine-configuration.h"

#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include <Xinput.h>
#include <time.h>

static uint32_t Backbuffer[LOGICAL_WIDTH * LOGICAL_HEIGHT];
static bool BackbufferReady = false;

// Lookup for checking a win32 virtual key's mapping in the abstraction layer.
KeyCode Win32_VirtualKey_KeyCode_Lookup[256];

void Win32_Start(WindowCreationParameters *windowCreationParams)
{
	Win32_InitializeWindow(windowCreationParams);

    ShowWindow(GET_PLATFORMWINDOW_HWND(), SW_SHOW);
}

// Windows are handled by the operating system, we are only given an opaque look at the window itself through a pointer (Handle in Win32 land).
// Because of this, we need to make a request to the operating system to make us a window and give us the pointer to it, which we then use to call
// system functions that allow us to work with the window indirectly. This is handled the same on Linux, so we can rely on a pointer 
// of a platform specific type to be handed back to us. We store this pointer in a globally provided PlatformWindowInstance.window
// void pointer to allow the abstraction layer to not have to worry about the typing of this pointer.  
void Win32_InitializeWindow(WindowCreationParameters *windowCreationParams)
{
	// Setup window utilizing abstraction level window creation parameters.
	WNDCLASSEXA windowClass      	= {0};
	windowClass.cbSize           	= sizeof(WNDCLASSEXA);
	windowClass.style            	= CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc      	= Win32_WindowProc;
	windowClass.hInstance        	= GetModuleHandleA(NULL);
	windowClass.hIcon				= windowCreationParams->iconImage ? CreateIconFromPixelBuffer(windowCreationParams->iconImage) : LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor          	= windowCreationParams->cursorImage ? CreateCursorFromPixelBuffer(windowCreationParams->cursorImage, 0, 0) : LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName    	= "Window";
	windowClass.hbrBackground		= CreateSolidBrush(RGB(235, 215, 185));

    RegisterClassExA(&windowClass);

	RECT rect = { 0, 0, 1280, 720 };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    PlatformWindowInstance.window = CreateWindowExA(
        0,
        "Window",
        windowCreationParams->title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left,   // adjusted width
		rect.bottom - rect.top,   // adjusted height
        NULL, NULL,
        GetModuleHandleA(NULL),
        NULL
    );
}

bool Win32_PeekMessages(void)
{
    MSG msg;

	ClearReleasedKeysFromKeyboardState();
	ClearReleasedMouseButtonsFromMouseButtonState();

    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
		{
			return false;
		} 

        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
	
    return true;
}

// Windows message loop callback
LRESULT CALLBACK Win32_WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_DESTROY:
		{
            PostQuitMessage(0);
            return 0;
		}
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {	
			ProcessKeyDown(wParam, lParam);

			return 0;
        }
		case WM_CHAR:
		{
			ProcessCharacter(wParam, lParam);

			return 0;
		}
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
			ProcessKeyUp(wParam);

			return 0;
        }
		case WM_LBUTTONDOWN:
		{
			SetMouseDownState(MOUSEBUTTON_ONE, (int16_t)(short)LOWORD(lParam), (int16_t)(short)HIWORD(lParam));

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
		case WM_KILLFOCUS:
		{
			// Some keys will break window focus, and it can cause a keyup to be missed and get stuck in a pressed
			// state, so when the window loses focus, clear keyboard state to avoid this.
			memset(KeyboardEventState, 0, sizeof(KeyboardEventState));
			return 0;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT paintStruct;
			HDC hdc = BeginPaint(hwnd, &paintStruct);

			if (BackbufferReady)
			{
				RECT clientRect;
				GetClientRect(hwnd, &clientRect);
				int clientWidth = clientRect.right  - clientRect.left;
				int clientHeight = clientRect.bottom - clientRect.top;

				BITMAPINFO info              = {0};
				info.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
				info.bmiHeader.biWidth       = LOGICAL_WIDTH;
				info.bmiHeader.biHeight      = -LOGICAL_HEIGHT;
				info.bmiHeader.biPlanes      = 1;
				info.bmiHeader.biBitCount    = 32;
				info.bmiHeader.biCompression = BI_RGB;

				StretchDIBits(hdc, 0, 0, clientWidth, clientHeight, 0, 0, LOGICAL_WIDTH, LOGICAL_HEIGHT, Backbuffer, &info, DIB_RGB_COLORS, SRCCOPY);
			}

			EndPaint(hwnd, &paintStruct);
			return 0;
		}
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

void Win32PollControllers(void)
{
	DWORD dwResult;    

	// NOTE: This is being hardcoded to a size of 4 because xinput has a limit of 4 controllers. 
	// This would need to be changed if the input backend were changed to support a broader range of controllers, 
	// but given this is an xinput specific loop, it seems fine to hardcode this.
	for (DWORD controllerIndex = 0; controllerIndex < 4; controllerIndex++ )
	{
		XINPUT_STATE state;
		memset(&state, 0, sizeof(XINPUT_STATE));

		// Simply get the state of the controller from XInput.
		dwResult = XInputGetState(controllerIndex, &state);

		if( dwResult == ERROR_SUCCESS )
		{
			// Controller is connected, map xinput state to controller state and toggle connected.
			ControllerStates[controllerIndex].Connected = true;
			ControllerStates[controllerIndex].DPAD_UP = state.Gamepad.wButtons& XINPUT_GAMEPAD_DPAD_UP;
			ControllerStates[controllerIndex].DPAD_DOWN = state.Gamepad.wButtons& XINPUT_GAMEPAD_DPAD_DOWN;
			ControllerStates[controllerIndex].DPAD_LEFT = state.Gamepad.wButtons& XINPUT_GAMEPAD_DPAD_LEFT;
			ControllerStates[controllerIndex].DPAD_RIGHT = state.Gamepad.wButtons& XINPUT_GAMEPAD_DPAD_RIGHT;
			ControllerStates[controllerIndex].START = state.Gamepad.wButtons& XINPUT_GAMEPAD_START;
			ControllerStates[controllerIndex].BACK = state.Gamepad.wButtons& XINPUT_GAMEPAD_BACK;
			ControllerStates[controllerIndex].LEFT_SHOULDER = state.Gamepad.wButtons& XINPUT_GAMEPAD_LEFT_SHOULDER;
			ControllerStates[controllerIndex].RIGHT_SHOULDER = state.Gamepad.wButtons& XINPUT_GAMEPAD_RIGHT_SHOULDER;
			ControllerStates[controllerIndex].A = state.Gamepad.wButtons& XINPUT_GAMEPAD_A;
			ControllerStates[controllerIndex].B = state.Gamepad.wButtons& XINPUT_GAMEPAD_B;
			ControllerStates[controllerIndex].X = state.Gamepad.wButtons& XINPUT_GAMEPAD_X;
			ControllerStates[controllerIndex].Y = state.Gamepad.wButtons& XINPUT_GAMEPAD_Y;
			ControllerStates[controllerIndex].LEFT_TRIGGER = state.Gamepad.bLeftTrigger;
			ControllerStates[controllerIndex].RIGHT_TRIGGER = state.Gamepad.bRightTrigger;
			ControllerStates[controllerIndex].ThumbLX =	state.Gamepad.sThumbLX;
			ControllerStates[controllerIndex].ThumbLY =	state.Gamepad.sThumbLY;
			ControllerStates[controllerIndex].ThumbRX =	state.Gamepad.sThumbRX;
			ControllerStates[controllerIndex].ThumbRY =	state.Gamepad.sThumbRY;
		}
		else
		{
			// Controller is disconnected, 0 out controller state.
			ControllerStates[controllerIndex].Connected = false;
			ControllerStates[controllerIndex].DPAD_UP = false;
			ControllerStates[controllerIndex].DPAD_DOWN = false;
			ControllerStates[controllerIndex].DPAD_LEFT = false;
			ControllerStates[controllerIndex].DPAD_RIGHT = false;
			ControllerStates[controllerIndex].START = false;
			ControllerStates[controllerIndex].BACK = false;
			ControllerStates[controllerIndex].LEFT_SHOULDER = false;
			ControllerStates[controllerIndex].RIGHT_SHOULDER = false;
			ControllerStates[controllerIndex].A = false;
			ControllerStates[controllerIndex].B = false;
			ControllerStates[controllerIndex].X = false;
			ControllerStates[controllerIndex].Y = false;
			ControllerStates[controllerIndex].LEFT_TRIGGER = 0;
			ControllerStates[controllerIndex].RIGHT_TRIGGER = 0;
			ControllerStates[controllerIndex].ThumbLX =	0;
			ControllerStates[controllerIndex].ThumbLY =	0;
			ControllerStates[controllerIndex].ThumbRX =	0;
			ControllerStates[controllerIndex].ThumbRY =	0;
		}
	}
}

void ClearReleasedKeysFromKeyboardState()
{
	for (int i = 0; i < KEYCODE_COUNT; i++)
    {
        if (KeyboardEventState[i].keyPressState == KEYPRESS_STATE_RELEASED)
        {
			KeyboardEventState[i] = (KeyEvent){0};
		}
	}
}

void ClearReleasedMouseButtonsFromMouseButtonState()
{
	for (int i = 0; i < MOUSEBUTTON_COUNT; i++)
    {
        if (MouseButtonEventState.buttonState[i].buttonState == MOUSEBUTTON_STATE_RELEASED)
        {
			MouseButtonEventState.buttonState[i].buttonState = 0;
		}
	}
}

void ProcessCharacter(WPARAM wParam, LPARAM lParam)
{
	// Get the scancode lparam
	UINT scanCode   = (lParam >> 16) & 0xFF;
	// Get the virtual key from windows
	UINT virtualKey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK);
	// Get the keyboard state lookup key from the virtual key.
	KeyCode code = Win32_VirtualKey_KeyCode_Lookup[virtualKey];

	KeyboardEventState[code].unicodeCharacter = (uint32_t)wParam;

	#if DEBUG
	PrintActiveKeyboardState();
	#endif
}

void ProcessKeyDown(WPARAM wParam, LPARAM lParam)
{
	KeyCode code        = Win32_VirtualKey_KeyCode_Lookup[wParam];
	bool    isRepeat    = (lParam & (1 << 30)) != 0;

	KeyboardEventState[code].keyCode          = code;
	KeyboardEventState[code].keyPressState    = KEYPRESS_STATE_DOWN;

	if (!isRepeat)
	{
		KeyboardEventState[code].keyPressState |= KEYPRESS_STATE_PRESSED;
	}
	else
	{
		KeyboardEventState[code].keyPressState |= KEYPRESS_STATE_HELD;
	}
	
	#if DEBUG
	if (MapVirtualKey(wParam, MAPVK_VK_TO_CHAR) == 0)
	{
		PrintActiveKeyboardState();
	}

	#endif
}

void ProcessKeyUp(WPARAM wParam)
{
	KeyCode code = Win32_VirtualKey_KeyCode_Lookup[wParam];
	KeyboardEventState[code].keyPressState = KEYPRESS_STATE_RELEASED;

	#if DEBUG
		PrintActiveKeyboardState();
	#endif
}



KeyCode Win32_VirtualKey_KeyCode_Lookup[256] = 
{
	['A'] 					= KEY_A,
	['B'] 					= KEY_B,
	['C'] 					= KEY_C,
	['D'] 					= KEY_D,
	['E'] 					= KEY_E,
	['F'] 					= KEY_F,
	['G'] 					= KEY_G,
	['H'] 					= KEY_H,
	['I'] 					= KEY_I,
	['J'] 					= KEY_J,
	['K'] 					= KEY_K,
	['L'] 					= KEY_L,
	['M'] 					= KEY_M,
	['N'] 					= KEY_N,
	['O'] 					= KEY_O,
	['P'] 					= KEY_P,
	['Q'] 					= KEY_Q,
	['R'] 					= KEY_R,
	['S'] 					= KEY_S,
	['T'] 					= KEY_T,
	['U'] 					= KEY_U,
	['V'] 					= KEY_V,
	['W'] 					= KEY_W,
	['X'] 					= KEY_X,
	['Y'] 					= KEY_Y,
	['Z'] 					= KEY_Z,
	['0'] 					= KEY_0,
	['1'] 					= KEY_1,
	['2'] 					= KEY_2,
	['3'] 					= KEY_3,
	['4'] 					= KEY_4,
	['5'] 					= KEY_5,
	['6'] 					= KEY_6,
	['7'] 					= KEY_7,
	['8'] 					= KEY_8,
	['9'] 					= KEY_9,
	[VK_OEM_1] 				= KEY_SEMICOLON,
	[VK_NUMPAD0] 			= KEY_NUMPAD_0,
	[VK_NUMPAD1] 			= KEY_NUMPAD_1,
	[VK_NUMPAD2] 			= KEY_NUMPAD_2,
	[VK_NUMPAD3] 			= KEY_NUMPAD_3,
	[VK_NUMPAD4] 			= KEY_NUMPAD_4,
	[VK_NUMPAD5] 			= KEY_NUMPAD_5,
	[VK_NUMPAD6] 			= KEY_NUMPAD_6,
	[VK_NUMPAD7] 			= KEY_NUMPAD_7,
	[VK_NUMPAD8] 			= KEY_NUMPAD_8,
	[VK_NUMPAD9] 			= KEY_NUMPAD_9,
	[VK_F1] 				= KEY_F1,
	[VK_F2] 				= KEY_F2,
	[VK_F3] 				= KEY_F3,
	[VK_F4] 				= KEY_F4,
	[VK_F5] 				= KEY_F5,
	[VK_F6] 				= KEY_F6,
	[VK_F7] 				= KEY_F7,
	[VK_F8] 				= KEY_F8,
	[VK_F9] 				= KEY_F9,
	[VK_F10] 				= KEY_F10,
	[VK_F11] 				= KEY_F11,
	[VK_F12] 				= KEY_F12,
	[VK_F13] 				= KEY_F13,
	[VK_F14] 				= KEY_F14,
	[VK_F15] 				= KEY_F15,
	[VK_F16] 				= KEY_F16,
	[VK_F17] 				= KEY_F17,
	[VK_F18] 				= KEY_F18,
	[VK_F19] 				= KEY_F19,
	[VK_F20] 				= KEY_F20,
	[VK_F21] 				= KEY_F21,
	[VK_F22] 				= KEY_F22,
	[VK_F23] 				= KEY_F23,
	[VK_F24] 				= KEY_F24,
	[VK_UP] 				= KEY_ARROW_UP,
	[VK_DOWN] 				= KEY_ARROW_DOWN,
	[VK_LEFT] 				= KEY_ARROW_LEFT,
	[VK_RIGHT] 				= KEY_ARROW_RIGHT,
	[VK_PRIOR] 				= KEY_PAGE_UP,
	[VK_NEXT] 				= KEY_PAGE_DOWN,
	[VK_END] 				= KEY_END,
	[VK_HOME] 				= KEY_HOME,
	[VK_SELECT]		 		= KEY_SELECT,
	[VK_NUMLOCK] 			= KEY_NUMLOCK,
	[VK_SCROLL] 			= KEY_SCROLL,
	[VK_PRINT] 				= KEY_PRINT,
	[VK_MULTIPLY] 			= KEY_MULTIPLY,
	[VK_ADD] 				= KEY_ADD,
	[VK_SEPARATOR] 			= KEY_SEPARATOR,
	[VK_SUBTRACT] 			= KEY_SUBTRACT,
	[VK_DECIMAL] 			= KEY_DECIMAL,
	[VK_DIVIDE] 			= KEY_DIVIDE,
	[VK_OEM_PLUS] 			= KEY_EQUALS_PLUS,
	[VK_LSHIFT] 			= KEY_SHIFT_LEFT, 
	[VK_RSHIFT] 			= KEY_SHIFT_RIGHT,
	[VK_LCONTROL] 			= KEY_CTRL_LEFT,  
	[VK_RCONTROL] 			= KEY_CTRL_RIGHT, 
	[VK_LMENU] 				= KEY_ALT_LEFT,   
	[VK_RMENU] 				= KEY_ALT_RIGHT,  
	[VK_CAPITAL] 			= KEY_CAPS,       
	[VK_OEM_MINUS] 			= KEY_DASH,              
	[VK_OEM_PERIOD] 		= KEY_PERIOD_GT,         
	[VK_OEM_2] 				= KEY_SLASH_QUESTION,    
	[VK_OEM_3] 				= KEY_TILDE_GRAVE,       
	[VK_OEM_4] 				= KEY_LEFT_BRACE,        
	[VK_OEM_5] 				= KEY_BACKSLASH_PIPE_US, 
	[VK_OEM_6] 				= KEY_RIGHT_BRACE,       
	[VK_OEM_7] 				= KEY_APOSTROPHE_QUOTE,
	[VK_OEM_8] 				= KEY_CTRL_RIGHT,
	[VK_OEM_102] 			= KEY_BACKSLASH_PIPE_ISO,
	[VK_OEM_COMMA] 			= KEY_COMMA_LT,          
	[VK_ESCAPE] 			= KEY_ESCAPE,         
	[VK_EXECUTE] 			= KEY_EXECUTE_COMMAND,
	[VK_SNAPSHOT] 			= KEY_SNAPSHOT,       
	[VK_INSERT] 			= KEY_INSERT,         
	[VK_DELETE] 			= KEY_DELETE,         
	[VK_HELP] 				= KEY_HELP,           
	[VK_APPS] 				= KEY_CONTEXT_MENU,   
	[VK_SPACE] 				= KEY_SPACE,    
	[VK_RETURN] 			= KEY_ENTER,    
	[VK_TAB] 				= KEY_TAB,      
	[VK_BACK] 				= KEY_BACKSPACE,
	[VK_OEM_CLEAR] 			= KEY_CLEAR,
	[VK_CLEAR] 				= KEY_CLEAR, 
	[VK_LWIN] 				= KEY_LEFT_META, 
	[VK_RWIN] 				= KEY_RIGHT_META,
	[VK_PAUSE] 				= KEY_PAUSE,      
	[VK_VOLUME_MUTE] 		= KEY_VOLUME_MUTE,
	[VK_VOLUME_DOWN] 		= KEY_VOLUME_DOWN,
	[VK_VOLUME_UP] 			= KEY_VOLUME_UP,  
	[VK_MEDIA_NEXT_TRACK] 	= KEY_NEXT_TRACK, 
	[VK_MEDIA_PREV_TRACK] 	= KEY_PREV_TRACK, 
	[VK_MEDIA_STOP] 		= KEY_STOP,       
};

static HICON CreateHICONFromPixelBuffer(PixelBuffer *pixelBuffer, BOOL isIcon, int hotspotX, int hotspotY)
{
	/*
		Windows uses an HICON for both cursors and window icons. An HICON is built from two bitmaps:
		a 32-bit color bitmap containing the pixel data, and a 1 bit mask(alpha) bitmap that serves as
		a transparency channel. When we provide a BITMAPV5HEADER, we are telling windows that the bitmap we 
		are provided has alpha encoding, however it still requires us to provide a fallback mask bitmap.
		This function serves to take a pixel buffer, and convert it into a valid color and mask bitmap so that windows will 
		provide us a custom HICON we can use for the window icon and cursor.
	*/
    if (!pixelBuffer || !pixelBuffer->pixelData)
	{
		return NULL;
	}

	// Using the pixel buffer, convert to a windows bitmap to create an icon, windows takes a bitmap v5 heading.
    BITMAPV5HEADER bitmapHeader     = {0};
    bitmapHeader.bV5Size            = sizeof(BITMAPV5HEADER);
    bitmapHeader.bV5Width           = pixelBuffer->width;
    bitmapHeader.bV5Height          = -pixelBuffer->height;
    bitmapHeader.bV5Planes          = 1;
    bitmapHeader.bV5BitCount        = 32;
    bitmapHeader.bV5Compression     = BI_BITFIELDS;
    bitmapHeader.bV5RedMask         = 0x000000FF;
    bitmapHeader.bV5GreenMask       = 0x0000FF00;
    bitmapHeader.bV5BlueMask        = 0x00FF0000;
    bitmapHeader.bV5AlphaMask       = 0xFF000000;

	// Next utilize the header and request windows to allocate us bitmap
    void *pixels = NULL;
    HDC deviceContextHandle = GetDC(NULL);

	// Create a bitmap to hold color data. This function will set the pixels pointer passed to it as the first byte in memory it allocated for us.
    HBITMAP colorBitmap = CreateDIBSection(deviceContextHandle, (BITMAPINFO *)&bitmapHeader, DIB_RGB_COLORS, &pixels, NULL, 0);
    ReleaseDC(NULL, deviceContextHandle);

    if (!colorBitmap)
	{
		return NULL;
	}

	// Copy our pixel buffer data into the allocated dib, this must be handled separately if we are using 3 vs. 4 channel bitmaps.
	uint8_t *pixelBufferData = (uint8_t *)pixelBuffer->pixelData;
	int pixelCount = pixelBuffer->width * pixelBuffer->height;

	if (pixelBuffer->channelCount == 4)
	{
		memcpy((uint8_t *)pixels, pixelBufferData, pixelCount * 4);
	}
	else if (pixelBuffer->channelCount == 3)
	{
		for (int i = 0; i < pixelCount; i++)
		{
			((uint8_t *)pixels)[i * 4 + 0] = pixelBufferData[i * 3 + 0]; 	// R
			((uint8_t *)pixels)[i * 4 + 1] = pixelBufferData[i * 3 + 1]; 	// G
			((uint8_t *)pixels)[i * 4 + 2] = pixelBufferData[i * 3 + 2]; 	// B
			((uint8_t *)pixels)[i * 4 + 3] = 255;             				// A - If we are seeing a 24 bitmap, it doesn't support alpha, full opaque.
		}
	}

	// Create a mask bitmap - if this fails, we could not convert the provided image, bail out and delete the color bitmap.
    HBITMAP maskBitmap = CreateBitmap(pixelBuffer->width, pixelBuffer->height, 1, 1, NULL);
    if (!maskBitmap)
    {
        DeleteObject(colorBitmap);
        return NULL;
    }

	// Setup icon info with our color and mask bitmaps, along with click region and isIcon, which determines what we get back.
    ICONINFO iconInfo   = {0};
    iconInfo.fIcon      = isIcon;
    iconInfo.xHotspot   = hotspotX;
    iconInfo.yHotspot   = hotspotY;
    iconInfo.hbmMask    = maskBitmap;
    iconInfo.hbmColor   = colorBitmap;

    HICON icon = CreateIconIndirect(&iconInfo);

	// Hygiene.
    DeleteObject(colorBitmap);
    DeleteObject(maskBitmap);

    return icon;
}

HCURSOR CreateCursorFromPixelBuffer(PixelBuffer *pixelBuffer, int hotspotX, int hotspotY)
{
    return (HCURSOR)CreateHICONFromPixelBuffer(pixelBuffer, FALSE, hotspotX, hotspotY);
}

HICON CreateIconFromPixelBuffer(PixelBuffer *pixelBuffer)
{
    return CreateHICONFromPixelBuffer(pixelBuffer, TRUE, 0, 0);
}

void GenerateRandomLevel(void)
{
    srand((unsigned int)time(NULL));

    Bitmap *sheet = ReadBitmapFromFile("assets/resources/bitmaps/testsheet.bmp");
    if (!sheet)
    {
        OutputDebugStringA("GenerateRandomLevel: ReadBitmapFromFile FAILED\n");
        return;
    }
    OutputDebugStringA("GenerateRandomLevel: ReadBitmapFromFile OK\n");

    int sheetCols = sheet->width  / GAME_TILE_WIDTH;
    int sheetRows = sheet->height / GAME_TILE_HEIGHT;

    memset(Backbuffer, 0, sizeof(Backbuffer));

    for (int row = 0; row < SCREEN_TILE_ROWS; row++)
    {
        for (int col = 0; col < SCREEN_TILE_COLS; col++)
        {
            int tileCol = rand() % sheetCols;
            int tileRow = rand() % sheetRows;

            int srcOriginX = tileCol * GAME_TILE_WIDTH;
            int srcOriginY = tileRow * GAME_TILE_HEIGHT;
            int dstOriginX = col    * GAME_TILE_WIDTH;
            int dstOriginY = row    * GAME_TILE_HEIGHT;

            for (int y = 0; y < GAME_TILE_HEIGHT; y++)
            {
                for (int x = 0; x < GAME_TILE_WIDTH; x++)
                {
                    int srcIdx = ((srcOriginY + y) * sheet->width + (srcOriginX + x)) * sheet->channels;
                    uint8_t r = sheet->pixels[srcIdx + 0];
                    uint8_t g = sheet->pixels[srcIdx + 1];
                    uint8_t b = sheet->pixels[srcIdx + 2];

                    int dstIdx = (dstOriginY + y) * LOGICAL_WIDTH + (dstOriginX + x);
                    Backbuffer[dstIdx] = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
                }
            }
        }
    }

    free(sheet);

    BackbufferReady = true;
    InvalidateRect(GET_PLATFORMWINDOW_HWND(), NULL, FALSE);
}