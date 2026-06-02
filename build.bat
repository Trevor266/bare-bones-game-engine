@echo off
if not exist build mkdir build

set Sources=main.c keyboard.c win32.c
set ReleaseBuild=%1

if /i "%ReleaseBuild%"=="release" (
    clang -O2 -D_CRT_SECURE_NO_WARNINGS %Sources% -o build\main.exe -luser32 -lgdi32 -Wl,/subsystem:windows
) else (
    clang -g -gcodeview -D_CRT_SECURE_NO_WARNINGS %Sources% -o build\main.exe -luser32 -lgdi32 -Wl,/subsystem:windows
)

if %errorlevel% neq 0 (
    echo Build failed.
    exit /b 1
)

"C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.exe" /debugexe build\main.exe

:: %errorlevel% == 0 means the build worked completely fine, so we can start our debugger.
::-g enables debug information.
::/debugexe opens visual studio debugger instance.