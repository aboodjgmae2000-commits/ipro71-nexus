@echo off
REM Build script for ipro71-nexus on Windows with MSVC
REM Usage: BUILD_SCRIPT_WINDOWS.bat [Release|Debug]

setlocal enabledelayedexpansion

if "%1"==" " (
    set BUILD_TYPE=Release
) else (
    set BUILD_TYPE=%1
)

echo.
echo ================================
echo ipro71-nexus Build Script
echo ================================
echo Build Type: %BUILD_TYPE%
echo.

if not exist "build" (
    echo Creating build directory...
    mkdir build
)

echo.
echo ================================
echo Step 1: CMake Configure
echo ================================
echo Configuring with CMake...
echo.

cd build
cmake -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ..

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: CMake configuration failed!
    echo Please verify:
    echo   - CMake is installed (cmake --version)
    echo   - OpenCV is installed and OpenCV_DIR is set
    echo   - ONNX Runtime is installed and ONNX_DIR or ONNXRUNTIME_DIR is set
    echo.
    cd ..
    exit /b 1
)

echo.
echo ================================
echo Step 2: CMake Build
echo ================================
echo Building project...
echo.

cmake --build . --config %BUILD_TYPE% --verbose

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Build failed!
    echo Check the output above for compilation errors.
    echo.
    cd ..
    exit /b 1
)

echo.
echo ================================
echo Step 3: Build Verification
echo ================================
echo.

if exist "bin\%BUILD_TYPE%\ipro71-nexus.exe" (
    echo SUCCESS: Executable created at bin\%BUILD_TYPE%\ipro71-nexus.exe
    dir bin\%BUILD_TYPE%\ipro71-nexus.exe
    echo.
    echo Build completed successfully!
) else if exist "bin\ipro71-nexus.exe" (
    echo SUCCESS: Executable created at bin\ipro71-nexus.exe
    dir bin\ipro71-nexus.exe
    echo.
    echo Build completed successfully!
) else (
    echo ERROR: Executable not found after build!
    echo Build may have failed silently.
    cd ..
    exit /b 1
)

cd ..
echo.
echo ================================
echo Build Process Complete
echo ================================
echo.
echo To run the application:
echo   - Release: .\build\bin\Release\ipro71-nexus.exe
echo   - Debug:   .\build\bin\Debug\ipro71-nexus.exe
echo.

exit /b 0
