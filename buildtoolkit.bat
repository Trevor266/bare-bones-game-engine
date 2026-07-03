@echo off
if not exist build-toolkit mkdir build-toolkit

Set CommonPath=Shared/common/source/
Set CommonSources=%CommonPath%dimensions.c
set Sources=Toolkit/toolkit-main.c Toolkit/source/gdifont.c Toolkit/source/gdibutton.c Toolkit/source/button.c Toolkit/source/homescreen.c Toolkit/source/buffer.c
set ReleaseBuild=%1

:: Compile resources (font, icons, etc.)
llvm-rc resources.rc
if %errorlevel% neq 0 (
    echo Resource compilation failed.
    pause
    exit /b 1
)

if /i "%ReleaseBuild%"=="release" (
    clang -O2 -D_CRT_SECURE_NO_WARNINGS -Iinclude %Sources% %CommonSources% resources.res -o build-toolkit\main.exe -luser32 -lgdi32 -lxinput -Wl,/subsystem:windows
) else (
    clang -g -gcodeview -DDEBUG -D_CRT_SECURE_NO_WARNINGS -Iinclude %Sources% %CommonSources% resources.res -o build-toolkit\main.exe -luser32 -lgdi32 -lxinput -Wl,/subsystem:windows
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