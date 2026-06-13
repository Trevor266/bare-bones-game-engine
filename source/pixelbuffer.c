#include <stdint.h>
#include <stdlib.h>
#include "pixelbuffer.h"
#include "bitmap.h" 

PixelBuffer *AllocatePixelBuffer()
{
    return malloc(sizeof(PixelBuffer));
}

void FreePixelBuffer(PixelBuffer *pixelBuffer)
{
    free(pixelBuffer);
}

PixelBuffer *ConvertBitmapToPixelBuffer(Bitmap *bitmap)
{
    PixelBuffer *pixelBuffer = AllocatePixelBuffer();
    pixelBuffer->channelCount = bitmap->channels;
    pixelBuffer->height     = bitmap->height;
    pixelBuffer->width      = bitmap->width;
    pixelBuffer->pixelData  = bitmap->pixels;
    
    return pixelBuffer;
}