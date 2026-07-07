// font.h
#ifndef _FONT_H
#define _FONT_H
#include <stdint.h>
#include "stb_truetype.h"
#include "buffer.h"

typedef struct Font
{
    uint8_t*            atlasPixels;
    int                 atlasDimension;
    stbtt_packedchar    charData[96];
    float               fontSize;
    int                 firstAsciiCodepoint;
    // This is a calculated field that stores the baseline-to-vertical-center offset amount.
    int                 verticalCenterOffset;
} Font;

Font LoadFont(const char *path, float fontSize);
void FreeFont(Font *font);

int MeasureTextWidth(Font *font, const char *text);
void DrawCustomText(void *BufferMemory, int BufferWidth, int BufferHeight, int BufferPitch, Font *font, const char *text, int x, int y, uint32 colorRGB);

#endif