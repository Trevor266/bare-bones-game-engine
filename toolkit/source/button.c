// Defines utilities for generating a button in client coordinate space. It is expected 
// that you provide the proper backbuffer.
#include "../../Shared/common/include/buffer.h"
#include "../../Shared/common/include/primitive_types.h"
#include "../../Shared/common/include/primitive_geometry.h"
#include "../../Shared/common/include/mouse.h"
#include "../../Shared/common/include/font.h"
#include "../include/button.h"
#include <stdbool.h>

bool ButtonHitTest(Button button, int hitX, int hitY)
{
    bool passed = 
        hitX >= button.dimensions.x && hitX <= button.dimensions.x + button.dimensions.width
        && hitY >= button.dimensions.y && hitY <= button.dimensions.y + button.dimensions.height;

    return passed;
}

void RenderButton(Button button, void *bufferMemory, int bufferWidth, int bufferHeight, int bufferPitch, Font font)
{
    bool renderBorder = button.borderWidth > 0;
    int buttonX = renderBorder ? button.dimensions.x + button.borderWidth : button.dimensions.x;
    int buttonY = renderBorder ? button.dimensions.y + button.borderWidth : button.dimensions.y; 
    int buttonWidth = renderBorder ? button.dimensions.width - (button.borderWidth * 2) : button.dimensions.width;
    int buttonHeight = renderBorder ? button.dimensions.height - (button.borderWidth * 2) : button.dimensions.height;

    // Conditionally render a quad underneath the actual button for a border effect.
    if (button.borderWidth > 0)
    {
        RenderQuad(
            bufferMemory,
            bufferWidth,
            bufferHeight,
            bufferPitch,
            button.dimensions.x,
            button.dimensions.y,
            button.dimensions.width,
            button.dimensions.height,
            button.borderRadius,
            button.borderColor
        );
    }

    RenderQuad(
        bufferMemory,
        bufferWidth,
        bufferHeight,
        bufferPitch,
        buttonX,
        buttonY,
        buttonWidth,
        buttonHeight,
        button.borderRadius - button.borderWidth,
        button.hovered ? button.hoverColor : button.backgroundColor
    );

    int textWidth = MeasureTextWidth(&font, button.text);
    int textX = button.dimensions.x + (button.dimensions.width / 2) - (textWidth / 2);
    int textY = button.dimensions.y + (button.dimensions.height / 2) + font.verticalCenterOffset;

    DrawCustomText(bufferMemory, bufferWidth, bufferHeight, bufferPitch, &font, button.text, textX, textY, button.textColor);
}