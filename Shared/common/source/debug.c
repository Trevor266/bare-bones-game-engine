#ifndef _DEBUG_H
#define _DEBUG_H
#include "../include/mouse.h"

#include <stdio.h>

void PrintMouseButtonState(MouseButton button)
{
    const char *state;

    if (MOUSEBUTTON_IS_RELEASED(MouseButtonEventState.buttonState[button]))
    {
        state = "released";
    }
    else if (MOUSEBUTTON_IS_HELD(MouseButtonEventState.buttonState[button]))
    {
        state = "held";
    }
    else if (MOUSEBUTTON_IS_PRESSED(MouseButtonEventState.buttonState[button]))
    {
        state = "pressed";
    }
    else
    {
        state = "down";
    }

    printf("Button %d %s at %d, %d\n", button, state, 
           MouseButtonEventState.buttonState[button].xCoordinate, 
           MouseButtonEventState.buttonState[button].yCoordinate);
}

#endif