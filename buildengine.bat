@echo off
if not exist build mkdir build

Set CommonPath=Shared/common/source/
Set SourcePath=Engine/source/
Set DebugSources=%SourcePath%engine-debug.c
Set CommonSources=%CommonPath%dimensions.c
set Sources=%SourcePath%keyboard.c %SourcePath%win32.c %SourcePath%engine.c %SourcePath%mouse.c %SourcePath%controller.c %SourcePath%window.c %SourcePath%file.c %SourcePath%pixelbuffer.c
set ReleaseBuild=%1

if /i "%ReleaseBuild%"=="release" (
    clang -O2 -D_CRT_SECURE_NO_WARNINGS -Iinclude Engine/main.c %Sources% %CommonSources% -o build\main.exe -luser32 -lgdi32 -lxinput -Wl,/subsystem:windows
) else (
    clang -g -gcodeview -DDEBUG -D_CRT_SECURE_NO_WARNINGS -Iinclude Engine/main.c %Sources% %CommonSources% %DebugSources% -o build\main.exe -luser32 -lgdi32 -lxinput -Wl,/subsystem:windows
)

if %errorlevel% neq 0 (
    echo Build failed.
    pause
    exit /b 1
)

:: Copy out asset folder into build directory.
robocopy assets build\assets /E /NJH /NJS /NFL /NDL > nul

"C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.exe" /debugexe build\main.exe

exit

:: %errorlevel% == 0 means the build worked completely fine, so we can start our debugger.
::-g enables debug information.
::/debugexe opens visual studio debugger instance.