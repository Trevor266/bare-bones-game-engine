#include "../include/leveleditor.h"

typedef struct LevelEditorInstance {
    Level *level;
    // Some sort of unique identifier for this editor instance, like an id. Unsure how best to handle this for now.
} LevelEditorInstance;

void DrawEditorWindow(OffscreenBuffer WindowBackBuffer, Font font)
{
    ClearBufferColor(&WindowBackBuffer, 0x00FBD2CB); 

    // First, we need to render out the basic sections of the editor. For starters, we need to divide the screen into a 4/5ths ratio with two 
    // main windows:
    int padding = 20;
    int leftRightPadding = padding * 2;
    int topBottomPadding = padding * 2;
    int paneGap = padding;
    int leftPaneX = padding; // 0 + padding
    int oneFifthWindowWidth = ((WindowBackBuffer.Width - leftRightPadding - paneGap) / 5);
    int leftPaneWidth = oneFifthWindowWidth * 4;
    int rightPaneWidth = oneFifthWindowWidth;
    int rightPaneX = leftPaneX + leftPaneWidth + paneGap;

    RenderQuad
    (
        WindowBackBuffer.Memory,
        WindowBackBuffer.Width,
        WindowBackBuffer.Height,
        WindowBackBuffer.Pitch,
        leftPaneX,
        padding,
        leftPaneWidth,
        (WindowBackBuffer.Height - topBottomPadding), 
        0,
        0x00FFE5E0
    );

    RenderQuad
    (
        WindowBackBuffer.Memory,
        WindowBackBuffer.Width,
        WindowBackBuffer.Height,
        WindowBackBuffer.Pitch,
        rightPaneX,
        padding,
        rightPaneWidth,
        (WindowBackBuffer.Height - topBottomPadding), 
        0,
        0x00FFE5E0
    );
}