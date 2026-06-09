#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <stdint.h>
#include <stdbool.h>

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
*/