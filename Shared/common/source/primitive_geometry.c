#include "../include/primitive_geometry.h"

// Buffer is expected to be provided as bytes. Writes a colored quad within the specified buffer.
void RenderQuad(void *BufferMemory, int BufferWidth, int BufferHeight, int BufferPitch, int X, int Y, int Width, int Height, int borderRadius, uint32 Color)
{
    uint8 ColorR = (uint8)(Color >> 16);
    uint8 ColorG = (uint8)(Color >> 8);
    uint8 ColorB = (uint8)(Color >> 0);

    int Right = X + Width;
    int Bottom = Y + Height;

    // Clip the X, Y, Bottom and Right to the buffer size if any exceed it's bounds.
    if (X < 0)
    {
        X = 0;
    }

    if (Y < 0)
    {
        Y = 0;
    }

    if (Right > BufferWidth)
    {
        Right = BufferWidth;
    }

    if (Bottom > BufferHeight)
    {
        Bottom = BufferHeight;
    }

    // If either of these is true, nothing will render, don't waste compute.
    if (X >= Right || Y >= Bottom)
    {
        return;
    }

    // Fill out each pixel of the specified box with the provided color and handle any corner rounding.
    for (int PixelY = Y; PixelY < Bottom; ++PixelY)
    {
        uint8 *Row = (uint8 *)BufferMemory + PixelY * BufferPitch;
        uint32 *Pixel = (uint32 *)Row + X;

        for (int PixelX = X; PixelX < Right; ++PixelX)
        {
            // If there is no border radius, we can just set the pixel color and be done with it.
            if (borderRadius <= 0)
            {
                *Pixel = Color;
            }

            // For rounding our radius, we take our border radius and imaging a smaller quad within this one. We then use the corners of this 
            // rect as the center points for a circle on each corner, rough visualization:

            /*
                . . . . . . . . . . . . . .
                .   ,--.            ,--.  .
                .  (    )          (    ) .
                .   `--'            `--'  .
                .                         .
                .                         .
                .   ,--.            ,--.  .
                .  (    )          (    ) .
                .   `--'            `--'  .
                . . . . . . . . . . . . . .
            */
           
            // Using these center points, we check each pixel to see if it's close to this edge. If it is, we save the vertical and horizontal 
            // distance to the edge, and use it to compute how much we should blend that pixel to get a rounded edge.
            int horizontalDistanceToNearestCornerCircle = 0;
            int verticalDistanceToNearestCornerCircle = 0;

            if (PixelX < X + borderRadius)
            {
                // Pixel is close to the left corner.
                horizontalDistanceToNearestCornerCircle = (X + borderRadius) - PixelX;
            }
            else if (PixelX > Right - borderRadius)
            {
                // Pixel is close to the right corner.
                horizontalDistanceToNearestCornerCircle = PixelX - (Right - borderRadius);
            }

            if (PixelY < Y + borderRadius)
            {
                // Pixel is close to the top corner
                verticalDistanceToNearestCornerCircle = (Y + borderRadius) - PixelY;
            }
            else if (PixelY > Bottom - borderRadius)
            {
                // Pixel is close to the bottom corner
                verticalDistanceToNearestCornerCircle = PixelY - (Bottom - borderRadius);
            }

            if (horizontalDistanceToNearestCornerCircle == 0 && verticalDistanceToNearestCornerCircle == 0)
            {
                *Pixel = Color;
                Pixel++;
                continue;
            }
            else
            {
                float horizontalDistanceToNearestCornerCircleSquared = horizontalDistanceToNearestCornerCircle * horizontalDistanceToNearestCornerCircle;
                float verticalDistanceToNearestCornerCircleSquared = verticalDistanceToNearestCornerCircle * verticalDistanceToNearestCornerCircle;
                float distance = sqrtf((float)(horizontalDistanceToNearestCornerCircleSquared + verticalDistanceToNearestCornerCircleSquared));

                float circleEdge = (float)borderRadius;

                float coverage = circleEdge - distance + 0.5f;
                if (coverage < 0.0f)
                {
                    coverage = 0.0f;   
                }
                if (coverage > 1.0f)
                {
                    coverage = 1.0f;
                }

                if (coverage > 0.0f)
                {
                    // If this pixel is not fully within the rounded quad, then blend it's rgb values by the coverage factor.
                    if (coverage >= 1.0f)
                    {
                        *Pixel = Color;
                    }
                    else
                    {
                        uint32 BgColor = *Pixel;
                        uint8 BgR = (uint8)(BgColor >> 16);
                        uint8 BgG = (uint8)(BgColor >> 8);
                        uint8 BgB = (uint8)(BgColor >> 0);

                        uint8 OutR = (uint8)(ColorR * coverage + BgR * (1.0f - coverage));
                        uint8 OutG = (uint8)(ColorG * coverage + BgG * (1.0f - coverage));
                        uint8 OutB = (uint8)(ColorB * coverage + BgB * (1.0f - coverage));

                        *Pixel = ((uint32)OutR << 16) | ((uint32)OutG << 8) | (uint32)OutB;
                    }
                }
            }

            Pixel++;
        }
    }
}