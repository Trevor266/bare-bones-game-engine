#include "../include/leveleditor.h"

typedef struct LevelEditorInstance {
    Level *level;
    // Some sort of unique identifier for this editor instance, like an id. Unsure how best to handle this for now.
} LevelEditorInstance;

void DrawEditorWindow(OffscreenBuffer WindowBackBuffer, Font font)
{
    ClearBufferColor(&WindowBackBuffer, 0x00FBD2CB);
    Dimensions windowDimensions = GetWin32WindowDimensions();

    RenderQuad(WindowBackBuffer.Memory, WindowBackBuffer.Width, WindowBackBuffer.Height, WindowBackBuffer.Pitch, 50, 50, 200, 200, 8, 0x00FFE5E0);
}