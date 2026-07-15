// Provides definitions for various buffer types
#ifndef _BUFFER_H
#define _BUFFER_H

#include <windows.h>
#include "primitive_types.h"

typedef struct OffscreenBuffer
{
    // 32 bit pixels: BB GG RR XX
    BITMAPINFO Info; // TODO: Check if abstraction layer bitmap can provide this instead.
    void *Memory;
    int Width;
    int Height;
    int Pitch; // The width of a given row of pixels in bytes.
} OffscreenBuffer;

extern void ClearBufferColor(OffscreenBuffer *Buffer, uint32 Color);
#endif