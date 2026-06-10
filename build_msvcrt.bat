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
    echo Deploying Qt DLLs...
    windeployqt --release --no-translations LibrarySystem.exe
    echo.
    echo Done! You can now run: build\LibrarySystem.exe
) else (
    echo Build failed!
)

pause
