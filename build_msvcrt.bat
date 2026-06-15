@echo off
REM Build script for LibrarySystem
REM Uses MinGW GCC 11.5.0 MSVCRT (compatible with Qt 6.8.2)

set "PATH=C:\Qt\6.8.2\mingw_64\bin;C:\Tool\pandacpp\mingw64\bin;C:\Tool\mingw64\bin;%PATH%"

if not exist "build\" mkdir build
cd build

echo Configuring with MSVCRT MinGW (GCC 11.5.0)...
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_CXX_COMPILER="C:/Tool/pandacpp/mingw64/bin/g++.exe"

if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

echo Building...
ninja

if %ERRORLEVEL% equ 0 (
    echo.
    echo Build successful!

    echo Deploying Qt DLLs + plugins...
    REM Copy essential DLLs from Qt (windeployqt sometimes skips them)
    copy /Y "C:\Qt\6.8.2\mingw_64\bin\Qt6Core.dll" . > nul
    copy /Y "C:\Qt\6.8.2\mingw_64\bin\Qt6Gui.dll" . > nul
    copy /Y "C:\Qt\6.8.2\mingw_64\bin\Qt6Widgets.dll" . > nul
    copy /Y "C:\Qt\6.8.2\mingw_64\bin\Qt6Sql.dll" . > nul
    copy /Y "C:\Qt\6.8.2\mingw_64\bin\Qt6Network.dll" . > nul
    copy /Y "C:\Qt\6.8.2\mingw_64\bin\Qt6Svg.dll" . > nul
    copy /Y "C:\Qt\6.8.2\mingw_64\bin\libstdc++-6.dll" . > nul
    copy /Y "C:\Qt\6.8.2\mingw_64\bin\libgcc_s_seh-1.dll" . > nul
    copy /Y "C:\Qt\6.8.2\mingw_64\bin\libwinpthread-1.dll" . > nul
    copy /Y "C:\Qt\6.8.2\mingw_64\bin\opengl32sw.dll" . > nul
    copy /Y "C:\Qt\6.8.2\mingw_64\bin\D3Dcompiler_47.dll" . > nul

    REM Copy plugins
    if not exist "platforms\" mkdir platforms
    copy /Y "C:\Qt\6.8.2\mingw_64\plugins\platforms\qwindows.dll" .\platforms\ > nul
    if not exist "sqldrivers\" mkdir sqldrivers
    copy /Y "C:\Qt\6.8.2\mingw_64\plugins\sqldrivers\qsqlite.dll" .\sqldrivers\ > nul
    if not exist "styles\" mkdir styles
    copy /Y "C:\Qt\6.8.2\mingw_64\plugins\styles\qmodernwindowsstyle.dll" .\styles\ > nul

    echo.
    echo Done! You can now run: build\LibrarySystem.exe
) else (
    echo Build failed!
)

pause
