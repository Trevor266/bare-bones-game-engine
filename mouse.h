#ifndef MOUSE_H
#define MOUSE_H

// VK_LBUTTON                          0x01    Left mouse button
// VK_RBUTTON                          0x02    Right mouse button
// VK_CANCEL                           0x03    Control-break processing
// VK_MBUTTON                          0x04    Middle mouse button
// VK_XBUTTON1                         0x05    X1 mouse button
// VK_XBUTTON2                         0x06    X2 mouse button
typedef enum MouseButton {
    MOUSEBUTTON_INVALID     = 0,
    MOUSEBUTTON_ONE         = 1,
    MOUSEBUTTON_TWO         = 2,
    MOUSEBUTTON_THREE       = 3,
    MOUSEBUTTON_FOUR        = 4,
    MOUSEBUTTON_FIVE        = 5,
    MOUSEBUTTON_COUNT       = 6,
} MouseButton;

#endif