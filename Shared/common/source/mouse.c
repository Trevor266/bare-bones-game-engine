#include "stdbool.h"
#include "../include/mouse.h"

MouseState MouseButtonEventState = 
{
	.lastX = 0,
	.lastY = 0,
};

// Sets the mouse state's last x and y positions
void SetActiveMouseCoordinate(int16_t x, int16_t y)
{
	MouseButtonEventState.lastX      	= x;
	MouseButtonEventState.lastY      	= y;
}

void SetMouseDownState(MouseButton button, int16_t x, int16_t y)
{
	// TODO: Remove this held logic and state type, windows does not fire multiple down events when the mouse buttons are held, this has 
	// to be tracked by intuiting it from a pressed state per-frame (or something).
	bool isHeld = MOUSEBUTTON_IS_DOWN(MouseButtonEventState.buttonState[button]);

	MouseButtonEventState.buttonState[button].mouseButton      	= button;
	MouseButtonEventState.buttonState[button].buttonState 		= MOUSEBUTTON_STATE_DOWN;
	MouseButtonEventState.buttonState[button].xCoordinate      	= x;
	MouseButtonEventState.buttonState[button].yCoordinate      	= y;

	if (!isHeld)
	{
		MouseButtonEventState.buttonState[button].buttonState |= MOUSEBUTTON_STATE_PRESSED;
	}
	else
	{
		MouseButtonEventState.buttonState[button].buttonState |= MOUSEBUTTON_STATE_HELD;
	}
}

void SetMouseUpState(MouseButton button, int16_t x, int16_t y)
{
	MouseButtonEventState.buttonState[button].mouseButton      	= button;
	MouseButtonEventState.buttonState[button].buttonState 		= MOUSEBUTTON_STATE_RELEASED;
	MouseButtonEventState.buttonState[button].xCoordinate      	= x;
	MouseButtonEventState.buttonState[button].yCoordinate      	= y;
}