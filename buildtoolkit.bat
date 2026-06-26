@echo off
if not exist build-toolkit mkdir build-toolkit

set Sources=source/keyboard.c source/win32.c source/engine.c source/mouse.c source/controller.c source/window.c source/file.c source/pixelbuffer.c
set ReleaseBuild=%1

if /i "%ReleaseBuild%"=="release" (
    clang -O2 -D_CRT_SECURE_NO_WARNINGS -Iinclude toolkit-main.c %Sources% -o build-toolkit\main.exe -luser32 -lgdi32 -lxinput -Wl,/subsystem:windows
) else (
    clang -g -gcodeview -DDEBUG -D_CRT_SECURE_NO_WARNINGS -Iinclude toolkit-main.c %Sources% source/debug.c -o build-toolkit\main.exe -luser32 -lgdi32 -lxinput -Wl,/subsystem:windows
)

if %errorlevel% neq 0 (
    echo Build failed.
    pause
    exit /b 1
)

:: Copy out asset folder into build directory.
robocopy assets build-toolkit\assets /E /NJH /NJS /NFL /NDL > nul

"C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.exe" /debugexe build-toolkit\main.exe

exit

:: %errorlevel% == 0 means the build worked completely fine, so we can start our debugger.
::-g enables debug information.
::/debugexe opens visual studio debugger instance.