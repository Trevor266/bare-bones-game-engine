#include <windows.h>
#include <stdint.h>
#include <stdbool.h>
#include "keyboard.h"
#include "win32.h"

/*
    A note on defines. Defines when compiled with clang at least, produce a constant value that's baked directly into the binary.
    If you take a look at usages of these constants, you can see examples where if they are multiplied together, the compiler optimizes 
    this into a single numeric constant. If we defined these as constant variables, the compiler will add a compare instruction to the generated 
    machine code due to the optimizer being less confident without hard-defined variables.                                                                                        
*/

// Set the width and height the game will work with internally, this size is used regardless of the actual screen size.
#define LOGICAL_WIDTH  1280
#define LOGICAL_HEIGHT 720

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    Win32_Start();
    return 0;
}
#endif
// TODO: Linux support.
//test
