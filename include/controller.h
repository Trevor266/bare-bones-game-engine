#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <stdint.h>
#include <stdbool.h>

/*
VK_GAMEPAD_A                        0xC3    Gamepad A button
VK_GAMEPAD_B                        0xC4    Gamepad B button
VK_GAMEPAD_X                        0xC5    Gamepad X button
VK_GAMEPAD_Y                        0xC6    Gamepad Y button
VK_GAMEPAD_RIGHT_SHOULDER           0xC7    Gamepad Right Shoulder button
VK_GAMEPAD_LEFT_SHOULDER            0xC8    Gamepad Left Shoulder button
VK_GAMEPAD_LEFT_TRIGGER             0xC9    Gamepad Left Trigger button
VK_GAMEPAD_RIGHT_TRIGGER            0xCA    Gamepad Right Trigger button
VK_GAMEPAD_DPAD_UP                  0xCB    Gamepad D-pad Up button
VK_GAMEPAD_DPAD_DOWN                0xCC    Gamepad D-pad Down button
VK_GAMEPAD_DPAD_LEFT                0xCD    Gamepad D-pad Left button
VK_GAMEPAD_DPAD_RIGHT               0xCE    Gamepad D-pad Right button
VK_GAMEPAD_MENU                     0xCF    Gamepad Menu/Start button
VK_GAMEPAD_VIEW                     0xD0    Gamepad View/Back button
VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON   0xD1    Gamepad Left Thumbstick button
VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON  0xD2    Gamepad Right Thumbstick button
VK_GAMEPAD_LEFT_THUMBSTICK_UP       0xD3    Gamepad Left Thumbstick up
VK_GAMEPAD_LEFT_THUMBSTICK_DOWN     0xD4    Gamepad Left Thumbstick down
VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT    0xD5    Gamepad Left Thumbstick right
VK_GAMEPAD_LEFT_THUMBSTICK_LEFT     0xD6    Gamepad Left Thumbstick left
VK_GAMEPAD_RIGHT_THUMBSTICK_UP      0xD7    Gamepad Right Thumbstick up
VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN    0xD8    Gamepad Right Thumbstick down
VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT   0xD9    Gamepad Right Thumbstick right
VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT    0xDA    Gamepad Right Thumbstick left
*/


/*

    IMPLEMENTATION NOTES:

    Seems like anything beyond xbox controllers is going to be a pain. For the windows side, you have a few apis you can use to get 
    controller input but only one of them is a one size fits all, and it's the raw input api. This basically requires you to 
    query for a header of bytes that identify the hardware you're working with, then some other data that gives you all the 
    data related to controllers, and that's only for button presses, not haptics. You essentially sign up to maintain an evolving 
    controller input library for life. I commend those brave soldiers who fight that battle.

    So this engine will only be supporting xbox controllers or xinput compatible controllers. For example, steam implements an input
    abstraction that allows play station controllers over bluetooth or wire to get mapped to xinput. 

    As for how to do this on linux, there are xinput equivalent libraries but I don't know enough to say more than that, so 
    it makes sense to go ahead and design the abstraction layer around xinput and win32 and shoehorn linux in after.
    This should cover all steam users, other xinput abstract-er,s and all xbox controllers - good enough.

    XInput notes:
    You can support up to 4 controllers with xinput. While multiplayer is not a design consideration of this engine, 
    I see no harm in tracking 4 controller input states at this layer in case that changes.

    MICROSOFTS DOC ON XINPUT MULTIPLE CONTROLLERS:
    "
        The XInput functions all require a dwUserIndex parameter that is passed in to identify the controller being set or queried. 
        This ID will be in the range of 0-3 and is set automatically by XInput. The number corresponds to the port that the controller is 
        plugged into, and is not modifiable.

        Each controller displays which ID it is using by lighting up a quadrant on the "ring of light" in the center of the controller. 
        A dwUserIndex value of 0 corresponds to the top-left quadrant; the numbering proceeds 
        around the ring in clockwise order.
    "

    typedef struct _XINPUT_STATE {
        DWORD          dwPacketNumber;  Flag showing whether the controller state has changed - sequential duplicates 
                                        of this value indicate no change, any other value is a change.
        XINPUT_GAMEPAD Gamepad;         Controller State
    } XINPUT_STATE, *PXINPUT_STATE;

    typedef struct _XINPUT_GAMEPAD {
        WORD  wButtons;             // Bitmask representing the buttons below - 0 = unpressed, 1 = pressed.
        BYTE  bLeftTrigger;         // 0-255 value representing the amount the trigger is pressed  
        BYTE  bRightTrigger;        ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        SHORT sThumbLX;             A signed integer value between -32768 and 32767 representing horizontal tilt of the stick
        SHORT sThumbLY;             A signed integer value between -32768 and 32767 representing vertical tilt of the stick
        SHORT sThumbRX;             Same as LX
        SHORT sThumbRY;             Same as LY
    } XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

    Device button	                Bitmask
    XINPUT_GAMEPAD_DPAD_UP	        \n0x0001
    XINPUT_GAMEPAD_DPAD_DOWN	    \n0x0002
    XINPUT_GAMEPAD_DPAD_LEFT	    \n0x0004
    XINPUT_GAMEPAD_DPAD_RIGHT	    \n0x0008
    XINPUT_GAMEPAD_START	        \n0x0010
    XINPUT_GAMEPAD_BACK	            \n0x0020
    XINPUT_GAMEPAD_LEFT_SHOULDER    \n0x0100
    XINPUT_GAMEPAD_RIGHT_SHOULDER   \n0x0200
    XINPUT_GAMEPAD_A	            \n0x1000
    XINPUT_GAMEPAD_B	            \n0x2000
    XINPUT_GAMEPAD_X	            \n0x4000
    XINPUT_GAMEPAD_Y	            \n0x8000

    XINPUT_GAMEPAD_LEFT_THUMB	    \n0x0040
    XINPUT_GAMEPAD_RIGHT_THUMB	    \n0x0080

    TODO: 
        * Make a win32 mapping array for the controller button enum to key into controller state.
        * Build the rest of state.
        * 
    
    The constant XINPUT_GAMEPAD_TRIGGER_THRESHOLD may be used as the value which bLeftTrigger and bRightTrigger must be 
    greater than to register as pressed. This is optional, but often desirable. Controller buttons do not manifest 
    crosstalk.
*/

// Represents the state of a single controller
typedef struct ControllerState { 
    bool Connected;
    bool DPAD_UP;
    bool DPAD_DOWN;
    bool DPAD_LEFT;
    bool DPAD_RIGHT;
    bool START;
    bool BACK;
    bool LEFT_SHOULDER;
    bool RIGHT_SHOULDER;
    bool A;
    bool B;
    bool X;
    bool Y;
    bool L3;
    bool R3;
    uint8_t LEFT_TRIGGER;
    uint8_t RIGHT_TRIGGER;
    signed short int ThumbLX;
    signed short int ThumbLY;
    signed short int ThumbRX;
    signed short int ThumbRY;
} ControllerState;

// Represents the state of the 4 supported controllers
extern ControllerState ControllerStates[4]; 

#endif