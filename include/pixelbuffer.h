#ifndef _PIXELBUFFERH
#define _PIXELBUFFERH
#include <stdint.h>

typedef struct PixelBuffer {
    uint8_t* pixelData;
    int height;
    int width;
} PixelBuffer;

#endif