#include "../include/buffer.h"
#include "../include/primitivetypes.h"

void ClearBufferColor(OffscreenBuffer *Buffer, uint32 Color)
{
    // Cast our buffer to a byte sized buffer so we can do pointer arithmetic on it byte by byte.
    // We will use this to initialize a pointer we do this arithmetic on in the inner for loop, and 
    // we reinitialize it from this value per row, this is so we are 
    uint8 *bufferBytes = (uint8 *)Buffer->Memory;
    
    // Go through each row of the buffer and get each pixel, then set it's color to the provided clear color.
    for (int i = 0; i < Buffer->Height; ++i)
    {
        uint32 *bufferPixels = (uint32 *)bufferBytes;

        for (int j = 0; j < Buffer->Width; ++j)
        {
            // Set the color value, then advance the pointer to the next pixel in the row.
            *bufferPixels = Color;
            bufferPixels++;
        }

        // Advance the buffer byte array to the next row
        bufferBytes += Buffer->Pitch;
    }
}