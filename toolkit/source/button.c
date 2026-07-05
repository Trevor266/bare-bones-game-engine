// Defines utilities for generating a button in client coordinate space. It is expected 
// that you provide the proper backbuffer.
#include "../../Shared/common/include/buffer.h"
#include "../../Shared/common/include/primitivetypes.h"
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

void DrawClientSpaceBox(OffscreenBuffer *Buffer, int X, int Y, int Width, int Height, uint32 Color)
{
    int Right = X + Width;
    int Bottom = Y + Height;

    // Clip the X, Y, Bottom and Right to the buffer size if any exceed it's bounds.
    if (X < 0)
    {
        X = 0;
    }

    if (Y < 0)
    {
        Y = 0;
    }

    if (Right > Buffer->Width)
    {
        Right = Buffer->Width;
    }

    if (Bottom > Buffer->Height)
    {
        Bottom = Buffer->Height;
    }

    // If either of these is true, nothing will render, don't waste compute.
    if (X >= Right || Y >= Bottom)
    {
        return;
    }

    // Fill out each pixel of the specified box with the provided color.
    for (int PixelY = Y; PixelY < Bottom; ++PixelY)
    {
        uint8 *Row = (uint8 *)Buffer->Memory + PixelY * Buffer->Pitch;
        uint32 *Pixel = (uint32 *)Row + X;

        for (int PixelX = X; PixelX < Right; ++PixelX)
        {
            *Pixel = Color;
            Pixel++;
        }
    }
}