#ifndef _BUTTON_H
#define _BUTTON_H
#include "../../Shared/common/include/buffer.h"
#include "../../Shared/common/include/primitive_types.h"
#include "../../Shared/common/include/dimensions.h"
#include <stdbool.h>

typedef struct Button {
    char *text;
    uint32_t textColor;
    uint32_t backgroundColor;
    uint32_t hoverColor;
    Dimensions dimensions;
} Button;

void DrawClientSpaceBox(OffscreenBuffer *Buffer, int X, int Y, int Width, int Height, uint32 Color);
bool ButtonHitTest(Button button, int hitX, int hitY);
void RenderButton(Button button);

#endif