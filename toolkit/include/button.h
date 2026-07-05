#ifndef _BUTTON_H
#define _BUTTON_H
#include "../../Shared/common/include/buffer.h"
#include "../../Shared/common/include/primitivetypes.h"

typedef struct Button {
    char *text;
    uint32_t textColor;
    uint32_t backgroundColor;
    uint32_t hoverColor;
    Dimensions dimensions;
} Button;

void DrawClientSpaceBox(OffscreenBuffer *Buffer, int X, int Y, int Width, int Height, uint32 Color);

#endif