#include <windows.h>
#include <stdint.h>
// Setup a KeyCode enum. This is used to represent operating system level keycodes that have specific values in the operating system. 
// This should in theory allow us to not have to worry about which operating system we're working with when we're in high level game logic trying 
// to deterine keypresses.

// enum KeyCode
typedef enum {
    /* Invalid */
    KEYCODE_INVALID     = 0,

    /* Alphabetics */
    KEY_A               = 1,
    KEY_B               = 2,
    KEY_C               = 3,
    KEY_D               = 4,
    KEY_E               = 5,
    KEY_F               = 6,
    KEY_G               = 7,
    KEY_H               = 8,
    KEY_I               = 9,
    KEY_J               = 10,
    KEY_K               = 11,
    KEY_L               = 12,
    KEY_M               = 13,
    KEY_N               = 14,
    KEY_O               = 15,
    KEY_P               = 16,
    KEY_Q               = 17,
    KEY_R               = 18,
    KEY_S               = 19,
    KEY_T               = 20,
    KEY_U               = 21,
    KEY_V               = 22,
    KEY_W               = 23,
    KEY_X               = 24,
    KEY_Y               = 25,
    KEY_Z               = 26,

    /* Numerics */
    KEY_0               = 27,
    KEY_1               = 28,
    KEY_2               = 29,
    KEY_3               = 30,
    KEY_4               = 31,
    KEY_5               = 32,
    KEY_6               = 33,
    KEY_7               = 34,
    KEY_8               = 35,
    KEY_9               = 36,

    /* Arrows */
    KEY_ARROW_UP        = 37,
    KEY_ARROW_DOWN      = 38,
    KEY_ARROW_LEFT      = 39,
    KEY_ARROW_RIGHT     = 40,

    /* Modifiers */
    KEY_SHIFT_LEFT      = 41,
    KEY_SHIFT_RIGHT     = 42,
    KEY_CTRL_LEFT       = 43,
    KEY_CTRL_RIGHT      = 44,
    KEY_ALT_LEFT        = 45,
    KEY_ALT_RIGHT       = 46,

    /* Modifiers — true if either side is held */
    KEY_SHIFT           = 47,
    KEY_CTRL            = 48,
    KEY_ALT             = 49,

    /* Whitespace */
    KEY_SPACE           = 50,
    KEY_ENTER           = 51,
    KEY_ESCAPE          = 52,
    KEY_TAB             = 53,
    KEY_BACKSPACE       = 54,

    /* Always last */
    KEYCODE_COUNT       = 55
} KeyCode;


/*
    Win32 defines the following virtual keys that we can map to game layer key codes.
VK_LBUTTON                          0x01    Left mouse button
VK_RBUTTON                          0x02    Right mouse button
VK_CANCEL                           0x03    Control-break processing
VK_MBUTTON                          0x04    Middle mouse button
VK_XBUTTON1                         0x05    X1 mouse button
VK_XBUTTON2                         0x06    X2 mouse button
                                    0x07    Reserved
VK_BACK                             0x08    Backspace key
VK_TAB                              0x09    Tab key
                                    0x0A-0B Reserved
VK_CLEAR                            0x0C    Clear key
VK_RETURN                           0x0D    Enter key
                                    0x0E-0F Unassigned
VK_SHIFT                            0x10    Shift key
VK_CONTROL                          0x11    Ctrl key
VK_MENU                             0x12    Alt key
VK_PAUSE                            0x13    Pause key
VK_CAPITAL                          0x14    Caps lock key
VK_KANA                             0x15    IME Kana mode
VK_HANGUL                           0x15    IME Hangul mode
VK_IME_ON                           0x16    IME On
VK_JUNJA                            0x17    IME Junja mode
VK_FINAL                            0x18    IME final mode
VK_HANJA                            0x19    IME Hanja mode
VK_KANJI                            0x19    IME Kanji mode
VK_IME_OFF                          0x1A    IME Off
VK_ESCAPE                           0x1B    Esc key
VK_CONVERT                          0x1C    IME convert
VK_NONCONVERT                       0x1D    IME nonconvert
VK_ACCEPT                           0x1E    IME accept
VK_MODECHANGE                       0x1F    IME mode change request
VK_SPACE                            0x20    Spacebar key
VK_PRIOR                            0x21    Page up key
VK_NEXT                             0x22    Page down key
VK_END                              0x23    End key
VK_HOME                             0x24    Home key
VK_LEFT                             0x25    Left arrow key
VK_UP                               0x26    Up arrow key
VK_RIGHT                            0x27    Right arrow key
VK_DOWN                             0x28    Down arrow key
VK_SELECT                           0x29    Select key
VK_PRINT                            0x2A    Print key
VK_EXECUTE                          0x2B    Execute key
VK_SNAPSHOT                         0x2C    Print screen key
VK_INSERT                           0x2D    Insert key
VK_DELETE                           0x2E    Delete key
VK_HELP                             0x2F    Help key
0                                   0x30    0 key
1                                   0x31    1 key
2                                   0x32    2 key
3                                   0x33    3 key
4                                   0x34    4 key
5                                   0x35    5 key
6                                   0x36    6 key
7                                   0x37    7 key
8                                   0x38    8 key
9                                   0x39    9 key
                                    0x3A-40 Undefined
A                                   0x41    A key
B                                   0x42    B key
C                                   0x43    C key
D                                   0x44    D key
E                                   0x45    E key
F                                   0x46    F key
G                                   0x47    G key
H                                   0x48    H key
I                                   0x49    I key
J                                   0x4A    J key
K                                   0x4B    K key
L                                   0x4C    L key
M                                   0x4D    M key
N                                   0x4E    N key
O                                   0x4F    O key
P                                   0x50    P key
Q                                   0x51    Q key
R                                   0x52    R key
S                                   0x53    S key
T                                   0x54    T key
U                                   0x55    U key
V                                   0x56    V key
W                                   0x57    W key
X                                   0x58    X key
Y                                   0x59    Y key
Z                                   0x5A    Z key
VK_LWIN                             0x5B    Left Windows logo key
VK_RWIN                             0x5C    Right Windows logo key
VK_APPS                             0x5D    Application key
                                    0x5E    Reserved
VK_SLEEP                            0x5F    Computer Sleep key
VK_NUMPAD0                          0x60    Numeric keypad 0 key
VK_NUMPAD1                          0x61    Numeric keypad 1 key
VK_NUMPAD2                          0x62    Numeric keypad 2 key
VK_NUMPAD3                          0x63    Numeric keypad 3 key
VK_NUMPAD4                          0x64    Numeric keypad 4 key
VK_NUMPAD5                          0x65    Numeric keypad 5 key
VK_NUMPAD6                          0x66    Numeric keypad 6 key
VK_NUMPAD7                          0x67    Numeric keypad 7 key
VK_NUMPAD8                          0x68    Numeric keypad 8 key
VK_NUMPAD9                          0x69    Numeric keypad 9 key
VK_MULTIPLY                         0x6A    Multiply key
VK_ADD                              0x6B    Add key
VK_SEPARATOR                        0x6C    Separator key
VK_SUBTRACT                         0x6D    Subtract key
VK_DECIMAL                          0x6E    Decimal key
VK_DIVIDE                           0x6F    Divide key
VK_F1                               0x70    F1 key
VK_F2                               0x71    F2 key
VK_F3                               0x72    F3 key
VK_F4                               0x73    F4 key
VK_F5                               0x74    F5 key
VK_F6                               0x75    F6 key
VK_F7                               0x76    F7 key
VK_F8                               0x77    F8 key
VK_F9                               0x78    F9 key
VK_F10                              0x79    F10 key
VK_F11                              0x7A    F11 key
VK_F12                              0x7B    F12 key
VK_F13                              0x7C    F13 key
VK_F14                              0x7D    F14 key
VK_F15                              0x7E    F15 key
VK_F16                              0x7F    F16 key
VK_F17                              0x80    F17 key
VK_F18                              0x81    F18 key
VK_F19                              0x82    F19 key
VK_F20                              0x83    F20 key
VK_F21                              0x84    F21 key
VK_F22                              0x85    F22 key
VK_F23                              0x86    F23 key
VK_F24                              0x87    F24 key
                                    0x88-8F Reserved
VK_NUMLOCK                          0x90    Num lock key
VK_SCROLL                           0x91    Scroll lock key
                                    0x92-96 OEM specific
                                    0x97-9F Unassigned
VK_LSHIFT                           0xA0    Left Shift key
VK_RSHIFT                           0xA1    Right Shift key
VK_LCONTROL                         0xA2    Left Ctrl key
VK_RCONTROL                         0xA3    Right Ctrl key
VK_LMENU                            0xA4    Left Alt key
VK_RMENU                            0xA5    Right Alt key
VK_BROWSER_BACK                     0xA6    Browser Back key
VK_BROWSER_FORWARD                  0xA7    Browser Forward key
VK_BROWSER_REFRESH                  0xA8    Browser Refresh key
VK_BROWSER_STOP                     0xA9    Browser Stop key
VK_BROWSER_SEARCH                   0xAA    Browser Search key
VK_BROWSER_FAVORITES                0xAB    Browser Favorites key
VK_BROWSER_HOME                     0xAC    Browser Start and Home key
VK_VOLUME_MUTE                      0xAD    Volume Mute key
VK_VOLUME_DOWN                      0xAE    Volume Down key
VK_VOLUME_UP                        0xAF    Volume Up key
VK_MEDIA_NEXT_TRACK                 0xB0    Next Track key
VK_MEDIA_PREV_TRACK                 0xB1    Previous Track key
VK_MEDIA_STOP                       0xB2    Stop Media key
VK_MEDIA_PLAY_PAUSE                 0xB3    Play/Pause Media key
VK_LAUNCH_MAIL                      0xB4    Start Mail key
VK_LAUNCH_MEDIA_SELECT              0xB5    Select Media key
VK_LAUNCH_APP1                      0xB6    Start Application 1 key
VK_LAUNCH_APP2                      0xB7    Start Application 2 key
                                    0xB8-B9 Reserved
VK_OEM_1                            0xBA    Semicolon and Colon key (US ANSI)
VK_OEM_PLUS                         0xBB    Equals and Plus key
VK_OEM_COMMA                        0xBC    Comma and Less Than key
VK_OEM_MINUS                        0xBD    Dash and Underscore key
VK_OEM_PERIOD                       0xBE    Period and Greater Than key
VK_OEM_2                            0xBF    Forward Slash and Question Mark key (US ANSI)
VK_OEM_3                            0xC0    Grave Accent and Tilde key (US ANSI)
                                    0xC1-C2 Reserved
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
VK_OEM_4                            0xDB    Left Brace key (US ANSI)
VK_OEM_5                            0xDC    Backslash and Pipe key (US ANSI)
VK_OEM_6                            0xDD    Right Brace key (US ANSI)
VK_OEM_7                            0xDE    Apostrophe and Double Quotation Mark key (US ANSI)
VK_OEM_8                            0xDF    Right Ctrl key (Canadian CSA)
                                    0xE0    Reserved
                                    0xE1    OEM specific
VK_OEM_102                          0xE2    Backslash and Pipe key (European ISO)
                                    0xE3-E4 OEM specific
VK_PROCESSKEY                       0xE5    IME PROCESS key
                                    0xE6    OEM specific
VK_PACKET                           0xE7    Unicode character keystrokes
                                    0xE8    Unassigned
                                    0xE9-F5 OEM specific
VK_ATTN                             0xF6    Attn key
VK_CRSEL                            0xF7    CrSel key
VK_EXSEL                            0xF8    ExSel key
VK_EREOF                            0xF9    Erase EOF key
VK_PLAY                             0xFA    Play key
VK_ZOOM                             0xFB    Zoom key
VK_NONAME                           0xFC    Reserved
VK_PA1                              0xFD    PA1 key
VK_OEM_CLEAR                        0xFE    Clear key
*/

/* 
    [Abstract]
    Keyboard abstraction layer - The goal of this file is to be able to take input from a linux or Win32 system and 
    allow a game logic layer to make considerations on those inputs based on the type of input. In any given game, a portion of game logic might generically want to ask the question: 
    "Was W pressed?". The starting point therefore should be something that sets up the minimum structure required to display all given possible keys 
    that can be input and whether or not they have been pressed. Once we can look at this structure and simply determine if the key is unpressed or not, 
    we can extend it to then take in the actual state of the key from each operating system so that we know with certainty, this can then be utilized by 
    generic game layer code.
*/

// Win32 Specific Notes:
/*
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
*/

// To get started, we should setup a structure that represents the overall state of the keypress.

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

typedef struct {
    uint8_t     keyPressState;      // A set of 4 flags [00001111] representing the state of the keypress (See Keypress States). 
    KeyCode     keyCode;            // Abstract keycode field used to map to the platform specific keycode.
    uint32_t    unicodeCodepoint;   // The codepoint of the unicode character (a codepoint is a table index essentially).
} KeyEvent;

extern KeyEvent KeyboardEventState[KEYCODE_COUNT];

void Initialize_Keyboard_Subsystem();