#ifndef _BITMAPH
#define _BITMAPH
#include <stdint.h>
/*
    A note on bitmaps
    Bitmaps are just files that lay out image data in a very specific way. This format is defined as:

    * BITMAPFILEHEADER - 1st chunk - shows size and signature of file.
    * BITMAPV5HEADER - 2nd chunk - shows compression and color info, image size, etc.
    * COLOR TABLE - 3rd chunk - optional portion - if present, specifies color info when bits per pixel <= 8
    * IMAGE DATA - 4th chunk - holds the underlying pixel data as a pixel array of x,y positions.
    * ICC COLOR PROFILE - 5th chunk - optional portion - holds optional color management metadata.

    If the color table or icc color profile segments are missing, they may instead have gaps inserted to replace them, 
    this depends on the encoder, but it is done so to preseve alignment.

    Bitmaps are laid out in BGR order over RGB order, readers must account for this.
*/

#pragma pack(push, 1)
typedef struct BitmapFileHeader{
    // File header          (BITMAPFILEHEADER)
    uint16_t signature;    // 'BM' for bitmap
    uint32_t fileSize;
    uint32_t reserved;
    uint32_t pixelOffset;  // Offset to get to pixel data segment.

    // DIB / info header    (BITMAPINFOHEADER)
    uint32_t dibSize;
    int32_t  width;
    int32_t  height;       // Negative = top-down, positive = bottom-up
    uint16_t colorPlanes;
    uint16_t bitsPerPixel;
    uint32_t compression;  // 0 = BI_RGB (uncompressed)
    uint32_t imageSize;
    int32_t  xPixelsPerMeter;
    int32_t  yPixelsPerMeter;
    uint32_t colorsInTable;
    uint32_t importantColors;
} BitmapFileHeader;
#pragma pack(pop)

typedef enum BitmapChannelFormatType
{
    BITMAP_CHANNEL_FORMAT_UNKNOWN         = 0,
    BITMAP_CHANNEL_FORMAT_GRAYSCALE       = 1,  // 1 channel  - indexed/grayscale BMPs
    BITMAP_CHANNEL_FORMAT_RGB             = 3,  // 3 channels - 24-bit BMP
    BITMAP_CHANNEL_FORMAT_RGBA            = 4,  // 4 channels - 32-bit BMP
} BitmapChannelFormatType;

typedef struct Bitmap {
    int                         width;
    int                         height;
    BitmapChannelFormatType     channels;
    uint8_t                     *pixels;
} Bitmap;

#endif