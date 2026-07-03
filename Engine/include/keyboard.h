/*
    This file contains types and definitions related to abstraction layer keyboard keystate.

    Keystate is a 128 byte lookup table of keyevents key'd off the KeyCode enum. This enum abstracts platform 
    specific physical keypresses to shared abstraction layer values and allows us to capture a snapshot of the 
    overall keystate that the operating system is reporting.

    Each KeyEvent consists of 
        * The KeyCode of the physical key input.
        * The unicode chatacter produced by the keypress.
        * The overall state of the keypress, represented by the KEYPRESS_ bitfields.
*/
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "keycodes.h"
#include <stdint.h>

// Define Keypress states
// Define 4 bitfields that represent the state of a keypress. Doing this packs our data into a consistent size 
// independent of the compiler, os, processor, etc. 
#define KEYPRESS_STATE_DOWN     (1 << 0)        // [00000001]
#define KEYPRESS_STATE_PRESSED  (1 << 1)        // [00000010]
#define KEYPRESS_STATE_RELEASED (1 << 2)        // [00000100]
#define KEYPRESS_STATE_HELD     (1 << 3)        // [00001000]

// Defines function like macros for ease of use when using the above bitmasks, this gives the performance of bitmasks and compiler inlining with
// a simpler function-like syntax and reduced repeated comparison code.
#define KEY_IS_DOWN(keyEvent)       ((keyEvent).keyPressState & KEYPRESS_STATE_DOWN)
#define KEY_IS_PRESSED(keyEvent)    ((keyEvent).keyPressState & KEYPRESS_STATE_PRESSED)
#define KEY_IS_RELEASED(keyEvent)   ((keyEvent).keyPressState & KEYPRESS_STATE_RELEASED)
#define KEY_IS_HELD(keyEvent)       ((keyEvent).keyPressState & KEYPRESS_STATE_HELD)

typedef struct KeyEvent {
    uint8_t     keyPressState;      // A set of 4 flags [00001111] representing the state of the keypress (See Keypress States). 
    KeyCode     keyCode;            // Abstract keycode field used to map to the platform specific keycode.
    uint32_t    unicodeCharacter;   // The unicode character of the keypress.
} KeyEvent;

extern KeyEvent KeyboardEventState[KEYCODE_COUNT];

void Initialize_Keyboard_Subsystem();

#endif

/*
    IMPLEMENTATION NOTES:

    The term VK is thrown around in win32. This is an abbreviation for the term "Virtual_Key". Keyboards emit something called a scan code, and scan codes are not guaranteed to be 
    the same per keyboard. Because of this, keyboard drivers translate scan codes into virtual keys. Scan codes in general are not useful beyond the driver/os layer, though 
    the windows keydown event does store the raw scan code in a portion of WndProc's lParam parameter, if you did care about the raw scan code value for something like a keyboard
    remapper.

    The documentation makes a note that in most cases a virtual key doesn't map to a specific ascii character, for example the A key could be á, but it does note that some keys are guaranteed to 
    match up to their ASCII values, it notes that these are:

        " 
            ...the following virtual-key codes do map to ASCII equivalents:
            0 through 9 keys = ASCII '0' – '9' (0x30 – 0x39)
            A through Z keys = ASCII 'A' – 'Z' (0x41 – 0x5A)
        "
    
    Windows handles keyboard events via four avenues:

        * WM_SYSKEYDOWN
        * WM_KEYDOWN
        * WM_KEYUP
        * WM_SYSKEYUP

    WM_SYSKEYDOWN is a message type that indicates a "system key" was pressed - these are keys that invoke a system command. 
        * Alt + any
        * F10 - Menu bar

    WM_KEYDOWN is a message type that indicates any non-system keypress that has been made.

    WM_SYSKEYUP is a message type that indicates that a system key that was pressed has now been released.

    WM_KEYUP is a message type that indicates that a non-sysem key that was pressed has now been released.

    With the messages available to us, and the parameters that come into WndProc, we are able to determine the following keystates:

    Pressed down
    Pressed down and released
    Pressed down previously and being held (keyboard's repeat feature triggers)
    Pressed down previously and being held, and now being released

    This allows us a large range of options in terms of handling input combinations, alternative types of inputs, etc.

    A keypress triggers a WM_KEYDOWN or WM_SYSKEYDOWN message to be sent to WndProc. When the key is continuously held, and the keyboard's 
    repeat feature triggers, the same keypress message will get passed over and over, followed by a corresponding keyup message when it is released.

    Besides the message types and order they are sent in, windows gives us two parameters with relevant keypress information. 

    lparam and wparam represent "longParam" and "wordParam". wParam contains the virtual key code of the key. lParam contains bits with additional information, 
    one of these being bit 30 which is a bit that represents the previous key state. If this bit is set to 1, it indicates a repeated keydown message, 
    this allows us to not have to use a variable to track the previous keypress message to compare it against the current, we can just check the message type and 
    if it is being repeated.

    Notes about keys between Linux and Win32:
    Left/Right Win key in windows is mapped to the VK_LWIN and VK_RWIN keys. 
    This is equivalent to Linux's meta keys "KEY_LEFTMETA" and "KEY_RIGHTMETA".
    In an effort to get away from windows centric labels, I'm favoring usage of leftMeta and rightMeta over traditional leftWin rightWin

    VK_APPS is apparently similar to the menu key for linux flavors, which I think is much better than "apps", so this layer uses "Key_ContextMenu"

    VK_SLEEP is being left out intentionally because it doesn't map cleanly to a physical linux key and it's a rare key in general, 
    plus the os usually handles it for power state, no sense in this application competing to use it.

    These do not map cleanly in any real context we care about, even keyboard manufacturers can't seem to agree on what these are for, and
    linux does not map them cleanly either:
    VK_LAUNCH_MAIL        
    VK_LAUNCH_MEDIA_SELECT
    VK_LAUNCH_APP1        
    VK_LAUNCH_APP2   
    
    // These VK's do not map to a guaranteed key. Some of them map semi-cleanly and some have defaults for windows, 
    // but none of them can be guaranteed, these are conditionally present and may or may not map to the defaults in docs.
    VK_OEM_1     
    VK_OEM_PLUS  
    VK_OEM_COMMA 
    VK_OEM_MINUS 
    VK_OEM_PERIOD
    VK_OEM_2     
    VK_OEM_3
    VK_OEM_4  
    VK_OEM_5  
    VK_OEM_6  
    VK_OEM_7  
    VK_OEM_8  
    VK_OEM_102 

    // These VK's refer to apparent terminal keys - I can't say I know what they are, and they appear to be ancient, and rarely used on modern OS's outside emulation
    // software, which doesn't concern this engine.
    VK_ATTN                             
    VK_CRSEL                            
    VK_EXSEL                            
    VK_EREOF                            
    VK_PLAY                             
    VK_ZOOM                             
    VK_NONAME                           
    VK_PA1           
    
    VK_OEM_CLEAR is a duplicate we already handle
    VK_PACKET gets into stuff this engine (at least this portion) is not concerned with regarding unicode processing.

    // IME_PROCESS - This is seemingly useless to process, this is essentially windows telling us that the IME (input method editor) processed whatever 
    // key was in wParam, so we are just being told some key was pressed that ime processed and overwrote with this value, if you wanted to get the real 
    // value you would have to invoke ImmGetVirtualKey to get the actual value if you cared. 
    // As far as I can tell there's no reason for physical key inputs that we would care about this, as it doesn't
     indicate any particular key press, only a vague one.
*/

// LINUX SPECIFIC NOTES:
/*
    Linux has two enter keys, there is kpenter and enter. This will probably be handled by just unifying the two into one. Unsure, still need to get linux running...
*/
