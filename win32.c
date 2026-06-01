#include "keyboard.h"
#include "mouse.h"
#include "win32.h"

static KeyCode Win32_VirtualKey_KeyCode_Lookup[256] = {
	// Alphabetics
	['A'] = KEY_A,
	['B'] = KEY_B,
	['C'] = KEY_C,
	['D'] = KEY_D,
	['E'] = KEY_E,
	['F'] = KEY_F,
	['G'] = KEY_G,
	['H'] = KEY_H,
	['I'] = KEY_I,
	['J'] = KEY_J,
	['K'] = KEY_K,
	['L'] = KEY_L,
	['M'] = KEY_M,
	['N'] = KEY_N,
	['O'] = KEY_O,
	['P'] = KEY_P,
	['Q'] = KEY_Q,
	['R'] = KEY_R,
	['S'] = KEY_S,
	['T'] = KEY_T,
	['U'] = KEY_U,
	['V'] = KEY_V,
	['W'] = KEY_W,
	['X'] = KEY_X,
	['Y'] = KEY_Y,
	['Z'] = KEY_Z,

	// Numerics
	['0'] = KEY_0,
	['1'] = KEY_1,
	['2'] = KEY_2,
	['3'] = KEY_3,
	['4'] = KEY_4,
	['5'] = KEY_5,
	['6'] = KEY_6,
	['7'] = KEY_7,
	['8'] = KEY_8,
	['9'] = KEY_9,
};

// This maybe isn't the most efficient way to do this. To keep mouse button lookup keys and keyboard input keys and lookups separate, this creates two 256 byte lookups for ease of use with using 
// win32 preprocessors as indexes in the looksup, VK_Keys are kept in the same lookup table in win32, this separates them into two lookups which technically doubles this to 2kb instead of 1kb to represent the same 
// things.
static MouseButton Win32_VirtualKey_MouseButton_Lookup[256] = {
	[VK_LBUTTON] 	= MOUSEBUTTON_ONE,
	[VK_RBUTTON] 	= MOUSEBUTTON_TWO,
	[VK_MBUTTON] 	= MOUSEBUTTON_THREE,
	[VK_XBUTTON1] 	= MOUSEBUTTON_FOUR,
	[VK_XBUTTON2]	= MOUSEBUTTON_FIVE,
};

void Win32_Start(void) {
    
}

static inline KeyCode Win32_TranslateKeyCode(WPARAM wParam) {
    if (wParam > 255)
    {
		return KEYCODE_INVALID;
    }
    
    return Win32_VirtualKey_KeyCode_Lookup[wParam];
}
