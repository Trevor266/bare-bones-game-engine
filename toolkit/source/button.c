// Defines utilities for generating a button in client coordinate space. It is expected 
// that you provide the proper backbuffer.
#include "../../Shared/common/include/buffer.h"
#include "../../Shared/common/include/primitive_types.h"
#include "../../Shared/common/include/mouse.h"
#include "../include/button.h"
#include <stdbool.h>

bool ButtonHitTest(Button button, int hitX, int hitY)
{
    bool passed = 
        hitX >= button.dimensions.x && hitX <= button.dimensions.x + button.dimensions.width
        && hitY >= button.dimensions.y && hitY <= button.dimensions.y + button.dimensions.height;

    return passed;
}

void RenderButton(Button button)
{
    
}