#ifndef _BUTTON_H
#define _BUTTON_H
#include <stdbool.h>
#include "../../Shared/common/include/buffer.h"
#include "../../Shared/common/include/primitive_types.h"
#include "../../Shared/common/include/primitive_geometry.h"
#include "../../Shared/common/include/dimensions.h"
#include "../../Shared/common/include/mouse.h"
#include "../../Shared/common/include/font.h"

typedef struct Button {
    char *text;
    uint32_t textColor;
    uint32_t backgroundColor;
    uint32_t borderColor;
    uint32_t borderWidth;
    uint32_t borderRadius;
    uint32_t hoverColor;
    bool hovered;
    Dimensions dimensions;
} Button;

bool ButtonHitTest(Button button, int hitX, int hitY);
void RenderButton(Button button, void *bufferMemory, int bufferWidth, int bufferHeight, int bufferPitch, Font font);

#endif