#include "../include/keyboard.h"
#include "../include/controller.h"
#include "../include/mouse.h"
#include <stdio.h>

void PrintMouseButtonState(MouseButton button)
{
    const char *state;

    if (MOUSEBUTTON_IS_RELEASED(MouseButtonEventState[button]))
    {
        state = "released";
    }
    else if (MOUSEBUTTON_IS_HELD(MouseButtonEventState[button]))
    {
        state = "held";
    }
    else if (MOUSEBUTTON_IS_PRESSED(MouseButtonEventState[button]))
    {
        state = "pressed";
    }
    else
    {
        state = "down";
    }

    printf("Button %d %s at %d, %d\n", button, state, 
           MouseButtonEventState[button].xCoordinate, 
           MouseButtonEventState[button].yCoordinate);
}

void PrintControllerState(void)
{
    for (int i = 0; i < 4; i++)
    {
        if (!ControllerStates[i].Connected)
		{
			break;
		}

        printf("Controller %d:\n", i);
        printf("  DPAD:         UP=%d DOWN=%d LEFT=%d RIGHT=%d\n",
            ControllerStates[i].DPAD_UP,
            ControllerStates[i].DPAD_DOWN,
            ControllerStates[i].DPAD_LEFT,
            ControllerStates[i].DPAD_RIGHT);
        printf("  Buttons:      A=%d B=%d X=%d Y=%d\n",
            ControllerStates[i].A,
            ControllerStates[i].B,
            ControllerStates[i].X,
            ControllerStates[i].Y);
        printf("  Shoulders:    L=%d R=%d\n",
            ControllerStates[i].LEFT_SHOULDER,
            ControllerStates[i].RIGHT_SHOULDER);
        printf("  Menu:         START=%d BACK=%d\n",
            ControllerStates[i].START,
            ControllerStates[i].BACK);
        printf("  Triggers:     L=%d R=%d\n",
            ControllerStates[i].LEFT_TRIGGER,
            ControllerStates[i].RIGHT_TRIGGER);
        printf("  Left Stick:   X=%d Y=%d\n",
            ControllerStates[i].ThumbLX,
            ControllerStates[i].ThumbLY);
        printf("  Right Stick:  X=%d Y=%d\n",
            ControllerStates[i].ThumbRX,
            ControllerStates[i].ThumbRY);
    }
}

void PrintActiveKeyboardState(void)
{
    for (int i = 0; i < KEYCODE_COUNT; i++)
    {
        if (KeyboardEventState[i].keyPressState != 0)
        {
            const char *state = "UNKNOWN";

            if (KeyboardEventState[i].keyPressState & KEYPRESS_STATE_PRESSED)
            {
                state = "PRESSED";
            }

            if (KeyboardEventState[i].keyPressState & KEYPRESS_STATE_HELD)
            {
                state = "HELD";
            }

            if (KeyboardEventState[i].keyPressState & KEYPRESS_STATE_RELEASED)
            {
                state = "RELEASED";
            }

            printf("KeyCode=%d  Unicode Character=%lc  State=%s\n",
                   KeyboardEventState[i].keyCode,
                   (wchar_t)KeyboardEventState[i].unicodeCharacter,
                   state);
        }
    }
}