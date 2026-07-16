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

    Dimensions editorPaneDimensions = 
    {
        .x = leftPaneX,
        .y = padding,
        .width = leftPaneWidth,
        .height = WindowBackBuffer.Height - topBottomPadding
    };

    Dimensions tilePickerPaneDimensions = 
    {
        .x = rightPaneX,
        .y = padding,
        .width = rightPaneWidth,
        .height = WindowBackBuffer.Height - topBottomPadding
    };

    // Next we need to render two more quads. These quads are a toolbar at the top of the editor window, what these buttons are specifically doesn't matter right now.
    // we just need to position and draw the quad these buttons would live in.
    Dimensions editorToolbarDimensions = 
    {
        .x = editorPaneDimensions.x + 5,
        .y = editorPaneDimensions.y + 5,
        .width = editorPaneDimensions.width - 10,
        .height = 50,
    };

    Dimensions editorMapDimensions = 
    {
        .x = editorToolbarDimensions.x,
        .y = editorToolbarDimensions.y + editorToolbarDimensions.height + 5,
        .width = editorPaneDimensions.width - 10,
        .height = editorPaneDimensions.height - editorToolbarDimensions.height - 15,
    };

    RenderQuad
    (
        WindowBackBuffer.Memory,
        WindowBackBuffer.Width,
        WindowBackBuffer.Height,
        WindowBackBuffer.Pitch,
        editorPaneDimensions.x,
        editorPaneDimensions.y,
        editorPaneDimensions.width,
        editorPaneDimensions.height, 
        0,
        0x00FFE5E0
    );

    RenderQuad
    (
        WindowBackBuffer.Memory,
        WindowBackBuffer.Width,
        WindowBackBuffer.Height,
        WindowBackBuffer.Pitch,
        tilePickerPaneDimensions.x,
        tilePickerPaneDimensions.y,
        tilePickerPaneDimensions.width,
        tilePickerPaneDimensions.height,
        0,
        0x00FFE5E0
    );

    RenderQuad
    (
        WindowBackBuffer.Memory,
        WindowBackBuffer.Width,
        WindowBackBuffer.Height,
        WindowBackBuffer.Pitch,
        editorToolbarDimensions.x,
        editorToolbarDimensions.y,
        editorToolbarDimensions.width,
        editorToolbarDimensions.height, 
        0,
        0x00FF4545
    );

    RenderQuad
    (
        WindowBackBuffer.Memory,
        WindowBackBuffer.Width,
        WindowBackBuffer.Height,
        WindowBackBuffer.Pitch,
        editorMapDimensions.x,
        editorMapDimensions.y,
        editorMapDimensions.width,
        editorMapDimensions.height, 
        0,
        0x0033A2A2
    );
}