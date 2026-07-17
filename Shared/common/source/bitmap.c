#include "../include/bitmap.h"

Bitmap* ConvertPixelBufferToBitmap(PixelBuffer *pixelBuffer);

PixelBuffer *ConvertBitmapToPixelBuffer(Bitmap *bitmap)
{
    PixelBuffer *pixelBuffer = AllocatePixelBuffer();
    pixelBuffer->channelCount = bitmap->channels;
    pixelBuffer->height     = bitmap->height;
    pixelBuffer->width      = bitmap->width;
    pixelBuffer->pixelData  = bitmap->pixels;
    
    return pixelBuffer;
}

// Blits a PixelBuffer into destMemory at (destX, destY) scaled to (destW, destH).
// Uses nearest-neighbor scaling so the bitmap fits whatever rect you give it (e.g. a button).
// Supports 24-bit (RGB, opaque) and 32-bit (RGBA, alpha-blended) source pixel buffers.
void RenderBitmap
(
    void        *destMemory,
    int          destBufferWidth,
    int          destBufferHeight,
    int          destPitch,
    PixelBuffer *pixelBuffer,
    int          destX,
    int          destY,
    int          destW,
    int          destH
)
{
    if (!pixelBuffer || !pixelBuffer->pixelData || destW <= 0 || destH <= 0)
    {
        return;
    }

    // Clip destination rect against the back buffer bounds.
    int clipX0 = destX < 0 ? 0 : destX;
    int clipY0 = destY < 0 ? 0 : destY;
    int clipX1 = (destX + destW) > destBufferWidth  ? destBufferWidth  : (destX + destW);
    int clipY1 = (destY + destH) > destBufferHeight ? destBufferHeight : (destY + destH);

    if (clipX0 >= clipX1 || clipY0 >= clipY1)
    {
        return; // Fully offscreen.
    }

    const int srcWidth    = pixelBuffer->width;
    const int srcHeight   = pixelBuffer->height;
    const int srcChannels = pixelBuffer->channelCount;
    const uint8_t *srcPixels = pixelBuffer->pixelData;

    for (int y = clipY0; y < clipY1; y++)
    {
        // Map destination row back to source row (nearest-neighbor scaling).
        int srcY = (y - destY) * srcHeight / destH;
        if (srcY >= srcHeight) srcY = srcHeight - 1;

        uint32_t *destRow = (uint32_t *)((uint8_t *)destMemory + y * destPitch);

        for (int x = clipX0; x < clipX1; x++)
        {
            int srcX = (x - destX) * srcWidth / destW;
            if (srcX >= srcWidth) srcX = srcWidth - 1;

            const uint8_t *srcPixel = srcPixels + (srcY * srcWidth + srcX) * srcChannels;
            uint8_t r = srcPixel[0];
            uint8_t g = srcPixel[1];
            uint8_t b = srcPixel[2];

            if (srcChannels == 4)
            {
                uint8_t a = srcPixel[3];
                if (a == 0)
                {
                    continue; // Fully transparent, skip.
                }
                if (a < 255)
                {
                    // Alpha-blend against whatever is already in the back buffer.
                    uint32_t destPixel = destRow[x];
                    uint8_t dr = (destPixel >> 16) & 0xFF;
                    uint8_t dg = (destPixel >> 8)  & 0xFF;
                    uint8_t db = (destPixel >> 0)  & 0xFF;

                    r = (uint8_t)((r * a + dr * (255 - a)) / 255);
                    g = (uint8_t)((g * a + dg * (255 - a)) / 255);
                    b = (uint8_t)((b * a + db * (255 - a)) / 255);
                }
            }

            destRow[x] = (r << 16) | (g << 8) | b;
        }
    }
}