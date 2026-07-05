#include "stdbool.h"
#include "../include/mouse.h"

MouseButtonEvent MouseButtonEventState[MOUSEBUTTON_COUNT] = {0};

void SetMouseDownState(MouseButton button, int x, int y)
{
	// TODO: Remove this held logic and state type, windows does not fire multiple down events when the mouse buttons are held, this has 
	// to be tracked by intuiting it from a pressed state per-frame (or something).
	bool isHeld = MOUSEBUTTON_IS_DOWN(MouseButtonEventState[button]);

	MouseButtonEventState[button].mouseButton      	= button;
	MouseButtonEventState[button].buttonState 		= MOUSEBUTTON_STATE_DOWN;
	MouseButtonEventState[button].xCoordinate      	= x;
	MouseButtonEventState[button].yCoordinate      	= y;

	if (!isHeld)
	{
		MouseButtonEventState[button].buttonState |= MOUSEBUTTON_STATE_PRESSED;
	}
	else
	{
		MouseButtonEventState[button].buttonState |= MOUSEBUTTON_STATE_HELD;
	}
}

void SetMouseUpState(MouseButton button, int x, int y)
{
	MouseButtonEventState[button].mouseButton      	= button;
	MouseButtonEventState[button].buttonState 		= MOUSEBUTTON_STATE_RELEASED;
	MouseButtonEventState[button].xCoordinate      	= x; // This avoids having to use windowsx.h, just gets the
	MouseButtonEventState[button].yCoordinate      	= y; // first and last 16 bits of the low 32 bits of lparam (64 bits).
}