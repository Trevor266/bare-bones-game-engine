#ifndef _PRIMITIVE_GEOMETRY_H
#define _PRIMITIVE_GEOMETRY_H

#include "buffer.h"
#include "math.h"

void RenderQuad(void *BufferMemory, int BufferWidth, int BufferHeight, int BufferPitch, int X, int Y, int Width, int Height, int borderRadius, uint32 Color);

#endif