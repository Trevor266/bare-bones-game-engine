#include "../include/primitive_geometry.h"

// Buffer is expected to be provided as bytes. Writes a colored quad within the specified buffer.
void RenderQuad(void *BufferMemory, int BufferWidth, int BufferHeight, int BufferPitch, int X, int Y, int Width, int Height, uint32 Color)
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

    if (Right > BufferWidth)
    {
        Right = BufferWidth;
    }

    if (Bottom > BufferHeight)
    {
        Bottom = BufferHeight;
    }

    // If either of these is true, nothing will render, don't waste compute.
    if (X >= Right || Y >= Bottom)
    {
        return;
    }

    // Fill out each pixel of the specified box with the provided color.
    for (int PixelY = Y; PixelY < Bottom; ++PixelY)
    {
        uint8 *Row = (uint8 *)BufferMemory + PixelY * BufferPitch;
        uint32 *Pixel = (uint32 *)Row + X;

        for (int PixelX = X; PixelX < Right; ++PixelX)
        {
            *Pixel = Color;
            Pixel++;
        }
    }
}