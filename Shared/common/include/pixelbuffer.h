#ifndef _PIXELBUFFERH
#define _PIXELBUFFERH
#include <stdint.h>
#include <stdlib.h>

typedef struct PixelBuffer {
    int channelCount;
    uint8_t* pixelData;
    int height;
    int width;
} PixelBuffer;

PixelBuffer*    AllocatePixelBuffer();
void            FreePixelBuffer(PixelBuffer *pixelBuffer);

#endif