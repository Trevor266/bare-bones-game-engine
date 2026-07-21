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

Bitmap *ReadBitmapFromFile(const char *path)
{
    FILE *fileHandle = fopen(path, "rb");
    if (!fileHandle)
    {
        return NULL;
    }

    // Use the file handle to read the bitmap header. The BitmapFileHeader struct should map 1:1 with 
    // a bitmap file header. We first read the first byte of the file and read the size of the bitmap header.
    // If this read succeeds, we then need to check the signature, compression methods, and bits per pixel to 
    // verify this is a bitmap we can work with.
    // Signature is valid when the value is 'BM'.
    // Compression methods 0 and 3 mean RGB and RGB+alpa (RGBA).
    // This supports a 24 or 32 bit bitmap.
    BitmapFileHeader header;
    bool headerReadSucceeded = fread(&header, sizeof(BitmapFileHeader), 1, fileHandle) == 1;
    bool validHeaderSignature = header.signature == 0x4D42;
    bool validBmpCompression = header.compression == 0 || header.compression == 3;
    bool validBitsPerPixel = header.bitsPerPixel == 24 || header.bitsPerPixel == 32;
    
    // If any of these checks fail, we definitely don't have a valid bitmap, bail (rhyme).
    if (!headerReadSucceeded || !validHeaderSignature || !validBmpCompression || !validBitsPerPixel)
    {
        fclose(fileHandle);
        return NULL;
    }

    /*
        Next get the relevant bitmap data. We need the height to be abs() because bitmaps either store the height in the header as 
        positive if the pixels are laid out bottom to top, or negative if the pixels are laid out top to bottom.
        We need to determine the size of a row of pixel data in the bitmap to properly read it. 
        Bitmaps always pad pixel data to the nearest interval of 4. Because of this, we check the pixel width of the bitmap 
        against the channel count, and round up to the nearest interval of 4 to get the size of the row.

        Diagram for a 2 pixel wide 3 channel row layout

        Pixel 1          Pixel 2          Row Padding
        [B][G][R]        [B][G][R]        [pad][pad]
        0   1   2        3   4   5        6     7

        Padding is added to the end of the row to align it to the nearest interval of 4. If we had a 4 channel bitmap, the padding would not be needed 
        because the extra 2 bytes would be used for the alpha channel instead of padding. In this sense you can think of it like 3 channel bitmaps may or 
        may not include padding depending on how they end up aligning, and 4 channel bitmaps never will use padding as they always align on 4.
    */
    const int       width    = header.width;
    const int       height   = abs(header.height);
    const int       channels = header.bitsPerPixel / 8; // bits to bytes
    const int       rowSize  = (width * channels + 3) & ~3; 
    const int       padding = rowSize - (width * channels);
    const uint8_t   isTopDown = header.height < 0;

    // Allocate memory for a bitmap plus the pixel data size, we can then +1 bitmap to access the pixel data portion of this memory.
    Bitmap *bitmap = malloc(sizeof(Bitmap) + (rowSize * height));
    if (!bitmap)
    {
        fclose(fileHandle);
        return NULL;
    }
    
    // Set pixel pointer to upper portion of previously allocated memory, and set other bitmap information.
    bitmap->pixels = (uint8_t *)(bitmap + 1);
    bitmap->width    = width;
    bitmap->height   = height;
    bitmap->channels = channels;

    // Set the file cursor to the pixel data offset to get the first pixel position. If this fails, stop.
    if (fseek(fileHandle, header.pixelOffset, SEEK_SET)) 
    {
        free(bitmap);
        fclose(fileHandle);
        return NULL;
    }

    for (int row = 0; row < height; row++)
    {
        // Based on the orientation of the bitmap, get the next row number.
        int nextRowNumber  = isTopDown ? row : (height - 1 - row);

        // Get a pointer to the next chunk of row data out of our bitmap structure based on the current iteration.
        uint8_t *nextRow = bitmap->pixels + nextRowNumber * width * channels;

        // Attempt to write the next chunk of pixel data into the bitmap pixel buffer, if we fail to do this, we cannot guarantee validity of the bitmap, bail out.
        if (fread(nextRow, width * channels, 1, fileHandle) != 1)
        {
            free(bitmap);
            fclose(fileHandle);
            return NULL;
        }

        // Now move the file io cursor to account for any potential padding.
        if (padding)
        {
            fseek(fileHandle, padding, SEEK_CUR);
        }
            

        // We convert BGR(A) to RGB(A) here, because bitmaps use BGR(A). 
        for (int col = 0; col < width * channels; col += channels)
        {
            uint8_t b = nextRow[col];
            uint8_t g = nextRow[col + 1];
            uint8_t r = nextRow[col + 2];

            // Set the next row's r/g/b components accordingly by offsetting from the column.
            nextRow[col]     = r;
            nextRow[col + 1] = g;
            nextRow[col + 2] = b;
        }
    }

    fclose(fileHandle);
    return bitmap;
}

int WriteBitmapToFile(const char *path, const Bitmap *bitmap)
{
    // NOTE: Right now this codebase only recognizes 24 and 32 bit bitmaps, as technically you don't really need the others, they can be represented by 
    // the channels of rgba bitmaps - This is subject to change
    if (!bitmap
        || !bitmap->pixels
        || (bitmap->channels != BITMAP_CHANNEL_FORMAT_RGB && bitmap->channels != BITMAP_CHANNEL_FORMAT_RGBA)
    )
    {
        return FALSE;
    }

    const int width             = bitmap->width;
    const int height            = bitmap->height;
    const int channels          = (int)bitmap->channels;
    const int rowSize           = (width * channels + 3) & ~3; // Zero out the bottom 2 bits here, because 24 bit bitmaps won't align properly otherwise.
    const int padding           = rowSize - (width * channels);
    const uint32_t pixelDataSize = (uint32_t)(rowSize * height);

    const BitmapFileHeader header = {
        .signature        = 0x4D42, // 'BM'
        .fileSize         = sizeof(BitmapFileHeader) + pixelDataSize,
        .reserved         = 0,
        .pixelOffset      = sizeof(BitmapFileHeader),
        .dibSize          = sizeof(BitmapFileHeader) - offsetof(BitmapFileHeader, dibSize),
        .width            = width,
        .height           = height,
        .colorPlanes      = 1,
        .bitsPerPixel     = (uint16_t)(channels * 8),
        .compression      = 0, // BI_RGB
        .imageSize        = pixelDataSize,
        .xPixelsPerMeter  = 0,
        .yPixelsPerMeter  = 0,
        .colorsInTable    = 0,
        .importantColors  = 0
    };

    FILE *file = fopen(path, "wb");
    if (!file)
    {
        #if DEBUG
        fprintf(stderr, "WriteBitmapToFile: failed to open '%s' for writing\n", path);
        #endif
        return FALSE;
    }

    if (fwrite(&header, sizeof(header), 1, file) != 1)
    {
        #if DEBUG
        fprintf(stderr, "WriteBitmapToFile: failed to write header to '%s'\n", path);
        #endif
        fclose(file);
        return FALSE;
    }

    // Create a buffer to represent our working row data as (BGR(A)) - FREE WHEN DONE.
    uint8_t *workingRowBuffer = calloc(1, rowSize);
    if (!workingRowBuffer)
    {
        fclose(file);
        return FALSE;
    }

    int writeFailed = FALSE;

    // Bitmap->pixels is stored top-down in memory (row 0 = first row), which is how this application is setup to handle bitmaps. 
    // Since header.height is positive (bottom-up on disk), we write rows in reverse order here so row (height - 1) in memory ends up first on disk.
    for (int rowIndex = 0; rowIndex < height; rowIndex++)
    {
        const int sourceRow = height - 1 - rowIndex;
        const uint8_t *srcRow = bitmap->pixels + (size_t)sourceRow * width * channels;

        // Each column pixel of the bitmap is actually 4 separate portions of data used to represent the bgra segement of each pixel. This establishes our
        // actual iteration count we will need to go through all channels of each pixel. 
        const int totalChannelWidth = width * channels;

        for (int col = 0; col < totalChannelWidth; col += channels)
        {
            const uint8_t r = srcRow[col];
            const uint8_t g = srcRow[col + 1];
            const uint8_t b = srcRow[col + 2];

            workingRowBuffer[col]     = b;
            workingRowBuffer[col + 1] = g;
            workingRowBuffer[col + 2] = r;

            if (channels == BITMAP_CHANNEL_FORMAT_RGBA)
            {
                workingRowBuffer[col + 3] = srcRow[col + 3];
            }
        }

        // Once we've built out our row's pixel data above, attempt to write and fail out if we can't write these pixels as at the very best, we will have 
        // a malformed bitmap.
        if (fwrite(workingRowBuffer, rowSize, 1, file) != 1)
        {
            #if DEBUG
            fprintf(stderr, "WriteBitmapToFile: failed to write row %d to '%s'\n", rowIndex, path);
            #endif
            writeFailed = TRUE;

            break;
        }
    }

    free(workingRowBuffer);
    fclose(file);

    return !writeFailed;
}