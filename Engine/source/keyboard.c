#include "../include/keyboard.h"

KeyEvent KeyboardEventState[KEYCODE_COUNT] = {0};

void Initialize_Keyboard_Subsystem()
{
	#ifdef _WIN32
	// TODO: Windows support.
	#endif
	// TODO: Linux support.
}