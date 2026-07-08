#ifndef _PIXELBUFFERH
#define _PIXELBUFFERH
#include <stdint.h>
#include "../../Shared/common/include/bitmap.h"

typedef struct PixelBuffer {
    int channelCount;
    uint8_t* pixelData;
    int height;
    int width;
} PixelBuffer;

PixelBuffer*    AllocatePixelBuffer();
void            FreePixelBuffer(PixelBuffer *pixelBuffer);
PixelBuffer*    ConvertBitmapToPixelBuffer(Bitmap *bitmap);
Bitmap*         ConvertPixelBufferToBitmap(PixelBuffer *pixelBuffer);

#endif