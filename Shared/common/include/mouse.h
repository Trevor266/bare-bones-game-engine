#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

// Define generic mouse button enum, this will be used at the game level for keying into mouse button state.
typedef enum MouseButton 
{
    MOUSEBUTTON_INVALID     = 0,
    MOUSEBUTTON_ONE         = 1,
    MOUSEBUTTON_TWO         = 2,
    MOUSEBUTTON_THREE       = 3,
    MOUSEBUTTON_FOUR        = 4,
    MOUSEBUTTON_FIVE        = 5,
    MOUSEBUTTON_COUNT       = 6,
} MouseButton;

// Define mouse button states
// Define 4 bitfields that represent the state of a mouse click. Doing this packs our data into a consistent size 
// independent of the compiler, os, processor, etc. 
#define MOUSEBUTTON_STATE_DOWN     (1 << 0)        // [00000001]
#define MOUSEBUTTON_STATE_PRESSED  (1 << 1)        // [00000010]
#define MOUSEBUTTON_STATE_RELEASED (1 << 2)        // [00000100]
#define MOUSEBUTTON_STATE_HELD     (1 << 3)        // [00001000]

// Defines function like macros for ease of use when using the above bitmasks, this gives the performance of bitmasks and compiler inlining with
// a simpler function-like syntax and reduced repeated comparison code.
#define MOUSEBUTTON_IS_DOWN(mouseButtonEvent)       ((mouseButtonEvent).buttonState & MOUSEBUTTON_STATE_DOWN)
#define MOUSEBUTTON_IS_PRESSED(mouseButtonEvent)    ((mouseButtonEvent).buttonState & MOUSEBUTTON_STATE_PRESSED)
#define MOUSEBUTTON_IS_RELEASED(mouseButtonEvent)   ((mouseButtonEvent).buttonState & MOUSEBUTTON_STATE_RELEASED)
#define MOUSEBUTTON_IS_HELD(mouseButtonEvent)       ((mouseButtonEvent).buttonState & MOUSEBUTTON_STATE_HELD)

typedef struct {
    uint8_t         buttonState;            // A set of 4 flags [00001111] representing the state of the button (See MouseButton States). 
    MouseButton     mouseButton;            // Abstract mouse button field used to map to the platform specific mouse buttons to generic ones.
    int16_t         xCoordinate;           // X coordinate of the window click.
    int16_t         yCoordinate;           // Y coordinate of the window click.
} MouseButtonEvent;

extern MouseButtonEvent MouseButtonEventState[MOUSEBUTTON_COUNT];

void SetMouseUpState(MouseButton button, int x, int y);
void SetMouseDownState(MouseButton button, int x, int y);

#endif