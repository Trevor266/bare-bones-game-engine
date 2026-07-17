@echo off
if not exist build-toolkit mkdir build-toolkit

Set CommonPath=Shared/common/source/
Set CommonSources=%CommonPath%dimensions.c %CommonPath%buffer.c %CommonPath%font.c %CommonPath%mouse.c %CommonPath%primitive_geometry.c %CommonPath%level.c %CommonPath%file.c %CommonPath%window.c  %CommonPath%pixelbuffer.c %CommonPath%bitmap.c
Set DebugSources=%CommonPath%debug.c
Set SourcePath=Toolkit/source/
set Sources=Toolkit/toolkit-main.c %SourcePath%gdifont.c %SourcePath%gdibutton.c %SourcePath%button.c %SourcePath%homescreen.c %SourcePath%leveleditor.c
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
    clang -g -gcodeview -DDEBUG -D_CRT_SECURE_NO_WARNINGS -Iinclude %Sources% %CommonSources% %DebugSources% resources.res -o build-toolkit\main.exe -luser32 -lgdi32 -lxinput -Wl,/subsystem:windows
)

if %errorlevel% neq 0 (
    echo Build failed.
    pause
    exit /b 1
)

:: Copy out asset folder into build directory.
robocopy assets build-toolkit\assets /E /NJH /NJS /NFL /NDL > nul

::"C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.exe" /debugexe build\main.exe
"C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\devenv.exe" /debugexe build-toolkit\main.exe

exit

:: %errorlevel% == 0 means the build worked completely fine, so we can start our debugger.
::-g enables debug information.
::/debugexe opens visual studio debugger instance.