#include "keyboard.h"
#include "mouse.h"
#include "win32.h"

#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

// Lookup for checking a win32 virtual key's mapping in the abstraction layer.
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

// This isn't the most efficient way to store this, this is a deliberate quality of life choice to keep usage consistent between abstraction layer level mouse buttons and key inputs while maintaining separation between the two. 
// This creates two 256 byte lookups for ease of use with using win32 preprocessors as indexes in the looksup, VK_Keys are kept in the same lookup table in win32, this separates them into two lookups which technically doubles this to 2kb instead of 1kb to represent the same 
// things.
MouseButton Win32_VirtualKey_MouseButton_Lookup[256] = 
{
	[VK_LBUTTON] 	= MOUSEBUTTON_ONE,
	[VK_RBUTTON] 	= MOUSEBUTTON_TWO,
	[VK_MBUTTON] 	= MOUSEBUTTON_THREE,
	[VK_XBUTTON1] 	= MOUSEBUTTON_FOUR,
	[VK_XBUTTON2]	= MOUSEBUTTON_FIVE,
};

static HWND Win32_Window = NULL;

static LRESULT CALLBACK Win32_WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
			AllocConsole();
			freopen("CONOUT$", "w", stdout);

			KeyCode code        = Win32_TranslateKeyCode(wParam);
			bool    isRepeat    = (lParam & (1 << 30)) != 0;

			KeyboardEventState[code].keyCode          = code;
			KeyboardEventState[code].unicodeCodepoint = (uint32_t)wParam;
			KeyboardEventState[code].keyPressState    = KEYPRESS_STATE_DOWN;

			if (!isRepeat)
			{
				KeyboardEventState[code].keyPressState |= KEYPRESS_STATE_PRESSED;
			}

			if (isRepeat)
			{
				KeyboardEventState[code].keyPressState |= KEYPRESS_STATE_HELD;
			}

			return 0;
        }

        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            KeyCode code = Win32_TranslateKeyCode(wParam);
			KeyboardEventState[code].keyPressState  = KEYPRESS_STATE_RELEASED;

			return 0;
        }
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

void Win32_Start(void) 
{
	WNDCLASSEXA wc      = {0};
		wc.cbSize           = sizeof(WNDCLASSEXA);
		wc.style            = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc      = Win32_WindowProc;
		wc.hInstance        = GetModuleHandleA(NULL);
		wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
		wc.lpszClassName    = "Window";

    RegisterClassExA(&wc);

    Win32_Window = CreateWindowExA(
        0,
        "Window",
        "TestWindow",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1280, 720,
        NULL, NULL,
        GetModuleHandleA(NULL),
        NULL
    );

    ShowWindow(Win32_Window, SW_SHOW);
}

bool Win32_PeekMessages(void)
{
    MSG msg;

    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
		{
			return false;
		} 

        TranslateMessage(&msg);
        DispatchMessageA(&msg);
		PrintActiveKeyState();
    }

    return true;
}

inline KeyCode Win32_TranslateKeyCode(WPARAM wParam) 
{
    if (wParam > 255)
    {
		return KEYCODE_INVALID;
    }
    
    return Win32_VirtualKey_KeyCode_Lookup[wParam];
}

void PrintActiveKeyboardState(void)
{
    for (int i = 0; i < KEYCODE_COUNT; i++)
    {
        if (KeyboardEventState[i].keyPressState & KEYPRESS_STATE_DOWN)
        {
            printf("KeyCode=%d  VK=0x%02X\n", 
                   KeyboardEventState[i].keyCode,
                   KeyboardEventState[i].unicodeCodepoint);
        }
    }
}
