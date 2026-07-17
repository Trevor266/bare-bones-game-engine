#include "../include/pixelbuffer.h"


PixelBuffer *AllocatePixelBuffer()
{
    return malloc(sizeof(PixelBuffer));
}

void FreePixelBuffer(PixelBuffer *pixelBuffer)
{
    free(pixelBuffer);
}