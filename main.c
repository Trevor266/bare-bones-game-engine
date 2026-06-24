#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "engine.h"
#include "keyboard.h"
#include "mouse.h"
#include "controller.h"
#include "file.h"
#include "win32.h"
#include "bitmap.h"
#include "pixelbuffer.h"

// Set the width and height the game will work with internally, this size is used regardless of the actual screen size.
#define LOGICAL_WIDTH  1280
#define LOGICAL_HEIGHT 720

float tickAccumulator = 0.0f;
float targetTickSeconds = 1.0f / 60.0f;  // 60 tick
float targetFrameSeconds = 1.0f / 60.0f; // 60 fps

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    #if DEBUG
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
	#endif

    LARGE_INTEGER frequency;
    LARGE_INTEGER lastCounter;

    #if DEBUG
        char *testTextFile = ReadTextFile("assets/data/test.txt");
        printf("Validating text file io: %s\n", testTextFile);
        free(testTextFile);
    #endif

    // Set custom cursor and window icon.
    Bitmap *testBitmap = ReadBitmapFromFile("assets/resources/bitmaps/test.bmp");
    PixelBuffer *pixelBuffer = ConvertBitmapToPixelBuffer(testBitmap);

    WindowCreationParameters windowCreationParams;
    windowCreationParams.cursorImage = pixelBuffer;
    windowCreationParams.iconImage = pixelBuffer;
    windowCreationParams.title = "Bare Bones Engine";

    Win32_Start(&windowCreationParams);

    InitializeEngine();
    GenerateRandomLevel();

    // Query for the high resolution performance frequency. This value represents the counts per second of the 
    // counter, this value is determined at system boot and therefore will not change during program runtime. 
    // It only needs queried one time due to this.
    // This value can be 0 for pre-windows xp systems, as machines at the time didn't always have a high performance counter.
    //
    // An example value is 10000000 representing a speed of 10 mhz.
    QueryPerformanceFrequency(&frequency);

    // We use to this query the hardware's performance counter. This is literally a counter that increments at the aforementioned 
    // frequency, so we can check the difference between the previous and current counter values across game loops to determine passed time per cycle.
    QueryPerformanceCounter(&lastCounter);

    // Setup a tick accumulator. This works as follows:
    // * Get delta time in milliseconds since our last iteration of this loop -> 
    //      currentQueryCounter.value - previousQueryCounter.value / performance counter frequency
    //
    // * Add the delta time to the tick accumulator.
    //
    // * Run the tick loop - This works by calculating a time each tick should last, then checking if the tick accumulator 
    //      still has a value greater than the target tick cycle time, if it does, then we subtract the duration of one tick from the 
    //      accumulator and run again. The idea is if a cycle ends up taking 3 ticks worth of time, we can play catch up.
    float tickAccumulator   = 0.0f;
    float targetTickSeconds  = 1.0f / 60.0f;
    float targetFrameSeconds = 1.0f / 60.0f;

    ControllerState previousControllerStates[4] = {0};

    while (Win32_PeekMessages())
    {
        // Get the current time, and delta time from the last counter and current time.
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        float deltaTime = (float)(now.QuadPart - lastCounter.QuadPart) / (float)frequency.QuadPart;
        tickAccumulator += deltaTime;

        // Set the last counter to now for the next cycle, and declare now to be the start time of the next 
        // frame cycle.
        lastCounter = now;
        LARGE_INTEGER frameStart = now;

        // Iterate over this loop until we no longer have a ticks worth of time to catch up on.
        while (tickAccumulator >= targetTickSeconds)
        {
            Win32PollControllers();

            #if DEBUG
            for (int i = 0; i < 4; i++)
            {
                if (ControllerStates[i].Connected &&
                    memcmp(&ControllerStates[i], &previousControllerStates[i], sizeof(ControllerState)) != 0)
                {
                    PrintControllerState();
                }
            }

            memcpy(previousControllerStates, ControllerStates, sizeof(ControllerStates));
            #endif
            tickAccumulator -= targetTickSeconds;
        }

        // TODO: Implement rendering -> RenderNextFrame();

        // Now we need to freeze this cycle until the next frame is ready to be drawn - This is how we are able 
        // to lock the frame rate, and additionally have an independent tick rate.
        float frameTimeElapsed = (float)(now.QuadPart - frameStart.QuadPart) / (float)frequency.QuadPart;
        if (frameTimeElapsed < targetFrameSeconds)
        {
            // NOTE: We are doing a sleep here to avoid hammering the cpu with more work than it needs to do.
            // in reality, an actual game would have plenty of work to do here and would not need to actually sleep the 
            // main thread, if anything you'd be fighting to finish this block in time for the next frame, not waiting it out, 
            // so this is mainly for testing.
            DWORD sleepMs = (DWORD)(1000.0f * (targetFrameSeconds - frameTimeElapsed));
            if (sleepMs > 1)
            {
                Sleep(sleepMs - 1);
            }

            do
            {
                // Sleep is not precise, it takes a minute to get this to call, so we drain the bulk of the frame time off with sleep, 
                // and then we wait out any remaining time in this dowhile loop.
                QueryPerformanceCounter(&now);
                frameTimeElapsed = (float)(now.QuadPart - frameStart.QuadPart) / (float)frequency.QuadPart;
            } while (frameTimeElapsed < targetFrameSeconds);
        }
    }

    return 0;
}
#endif
// TODO: Linux support.
