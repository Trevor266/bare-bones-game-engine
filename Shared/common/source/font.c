#define STB_RECT_PACK_IMPLEMENTATION
#include "../include/stb_rect_pack.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "../include/font.h"
#include <stdio.h>
#include <stdlib.h>

const uint8_t codePointOffset = 32;
const uint8_t codePointBoundaryIndex = 128;

// Fonts are loaded via STB BFontBitmap, where we provide the character set we would like it to load glyphs in for us, 
// as well as the font size, or points. 
Font LoadFont(const char *path, float fontSize)
{
    Font font = {0};
    font.fontSize = fontSize;

    FILE *fontFile = fopen(path, "rb");
    if (!fontFile)
    {
        return font;
    }

    fseek(fontFile, 0, SEEK_END);
    long fileSize = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);

    if (fileSize <= 0)
    {
        fclose(fontFile);
        return font;
    }

    uint8_t *fontFileBuffer = malloc((size_t)fileSize);
    if (!fontFileBuffer)
    {
        fclose(fontFile);
        return font;
    }

    size_t bytesRead = fread(fontFileBuffer, 1, (size_t)fileSize, fontFile);
    fclose(fontFile);

    if (bytesRead != (size_t)fileSize)
    {
        free(fontFileBuffer);
        return font;
    }

    // Pull all standard ASCII characters with actual glyphs. 32 is the first ascii codepoint in the standard ascii table.
    // (NOTE) This is obviously very limited and is candiate for future updates of unicode characters we may end up caring about. 
    font.firstAsciiCodepoint = 32;
    const int numChars  = 96;

    // Stb writes glyph data as raw pixels into a pixel buffer, if it runs out of space to insert glyph data into the buffer, it bails early 
    // and reports a zero value back to us (packed API: non-zero means every glyph fit, zero means it didn't - the simple/baked API 
    // used the opposite convention, negative for partial failure). Because the glyphs themselves are different sizes, and the size of the 
    // requested font is variable, we either have to allocate a giant buffer that we would never fill to cover all font sizes and fonts, 
    // or we start with a reasonably sized buffer, and size up the buffer when stb reports it didn't fit, rinse and repeat until we hit a 4096x4096 pixel buffer.
    int bufferPixelSize = 512;
    const int maxBufferPixelSize = 4096;

    int packResult = 0;

    // Double the buffer size each iteration until we hit 4096 or stbtt_PackFontRange reports success, then bail out.
    for (int currentBufferPixelSize = 512; currentBufferPixelSize <= 4096; currentBufferPixelSize *= 2)
    {
        uint8_t *atlasPixels = malloc((size_t)currentBufferPixelSize * currentBufferPixelSize);
        if (!atlasPixels)
        {
            break; // out of memory - stop trying to grow further
        }

        // Setup a context for packing the font bitmap, if PackBegin fails, we fail out of buffer creation and packing. 
        stbtt_pack_context packContext;
        if (!stbtt_PackBegin(&packContext, atlasPixels, currentBufferPixelSize, currentBufferPixelSize, 0, 2, NULL))
        {
            free(atlasPixels);
            break;
        }

        stbtt_PackSetOversampling(&packContext, 1, 1);

        packResult = stbtt_PackFontRange
        (
            &packContext,
            fontFileBuffer,
            0,
            fontSize,
            font.firstAsciiCodepoint,
            numChars,
            font.charData
        );

        stbtt_PackEnd(&packContext);

        if (packResult)
        {
            font.atlasPixels = atlasPixels;
            font.atlasDimension  = currentBufferPixelSize;
            font.atlasDimension = currentBufferPixelSize;
            break;
        }

        free(atlasPixels);
    }

    // Extract vertical metrics before freeing the raw file buffer - stbtt_fontinfo holds 
    // pointers into fontFileBuffer, so this must happen before the free() below.
    stbtt_fontinfo fontInfo;
    stbtt_InitFont(&fontInfo, fontFileBuffer, 0);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);
    float scale = stbtt_ScaleForPixelHeight(&fontInfo, fontSize);

    font.verticalCenterOffset = (int)(((ascent + descent) * scale) / 2.0f);

    free(fontFileBuffer);
    return font;
}

void FreeFont(Font *font)
{
    free(font->atlasPixels);
    font->atlasPixels = NULL;
}

void DrawCustomText(void *BufferMemory, int BufferWidth, int BufferHeight, int BufferPitch, Font *font, const char *text, int x, int y, uint32 colorRGB)
{
    float penX = (float)x;
    float penY = (float)y; // baseline, not top-left

    uint8_t *bufferBytes = (uint8_t *)BufferMemory;

    // Use GetPackedQuad to get the bitmap pixels for the next character we need to get.
    for (const char *nextChar = text; *nextChar; nextChar++)
    {
        // Standard ascii codepoints are just the character themselves, here we verify that the next character falls within 
        // the ascii range.
        if (*nextChar < codePointOffset || *nextChar >= codePointBoundaryIndex)
        {
            continue;
        }

        stbtt_aligned_quad quad;
        stbtt_GetPackedQuad
        (
            font->charData,
            font->atlasDimension,
            font->atlasDimension,
            *nextChar - font->firstAsciiCodepoint,
            &penX,
            &penY,
            &quad,
            1
        );

        int destX0 = (int)quad.x0;
        int destY0 = (int)quad.y0;
        int glyphWidth  = (int)quad.x1 - destX0;
        int glyphHeight = (int)quad.y1 - destY0;

        // Get the packed glyph bitmap from the font and go through each pixel.
        for (int glyphRow = 0; glyphRow < glyphHeight; glyphRow++)
        {
            int destinationY = destY0 + glyphRow;

            if (destinationY < 0 || destinationY >= BufferHeight)
            {
                continue;
            }

            for (int glyphColumn = 0; glyphColumn < glyphWidth; glyphColumn++)
            {
                int destinationX = destX0 + glyphColumn;

                if (destinationX < 0 || destinationX >= BufferWidth)
                {
                    continue;
                }

                float atlasNormalizedU = quad.s0 + (quad.s1 - quad.s0) * ((float)glyphColumn / glyphWidth);
                float atlasNormalizedV = quad.t0 + (quad.t1 - quad.t0) * ((float)glyphRow / glyphHeight);

                int atlasPixelX = (int)(atlasNormalizedU * font->atlasDimension);
                int atlasPixelY = (int)(atlasNormalizedV * font->atlasDimension);
                uint8_t glyphCoverage = font->atlasPixels[atlasPixelY * font->atlasDimension + atlasPixelX];

                if (glyphCoverage == 0)
                {
                    continue;   
                }

                // Pitch is in bytes, so index the row via a byte pointer, then treat that row as uint32s.
                uint32 *destinationRow = (uint32 *)(bufferBytes + destinationY * BufferPitch);
                uint32 *destinationPixel = &destinationRow[destinationX];

                uint8_t textColorR = (colorRGB >> 16) & 0xFF;
                uint8_t textColorG = (colorRGB >> 8) & 0xFF;
                uint8_t textColorB = colorRGB & 0xFF;
                uint8_t backgroundR = (*destinationPixel >> 16) & 0xFF;
                uint8_t backgroundG = (*destinationPixel >> 8) & 0xFF;
                uint8_t backgroundB = *destinationPixel & 0xFF;

                uint8_t blendedR = (textColorR * glyphCoverage + backgroundR * (255 - glyphCoverage)) / 255;
                uint8_t blendedG = (textColorG * glyphCoverage + backgroundG * (255 - glyphCoverage)) / 255;
                uint8_t blendedB = (textColorB * glyphCoverage + backgroundB * (255 - glyphCoverage)) / 255;

                *destinationPixel = (blendedR << 16) | (blendedG << 8) | blendedB;
            }
        }
    }
}

int MeasureTextWidth(Font *font, const char *text)
{
    float width = 0.0f;
    for (const char *nextChar = text; *nextChar; nextChar++)
    {
        if (*nextChar >= codePointOffset && *nextChar < codePointBoundaryIndex)
        {
            width += font->charData[*nextChar - codePointOffset].xadvance;
        }
    }

    return (int)width;
}