# Build Instructions for ipro71-nexus

## System Requirements

### Compiler
- **Visual Studio 2019 or later** (with C++ workload)
- **MSVC compiler** (v142 or later)
- **C++17 support**

### Tools
- **CMake 3.20 or later**
- **Git**

### Dependencies
1. **OpenCV 4.x** (core, imgproc modules)
2. **ONNX Runtime 1.8.0 or later**
3. **Windows SDK** (user32, gdi32, kernel32 libraries)

## Installation Steps

### 1. Install OpenCV

```bash
# Download from https://github.com/opencv/opencv/releases
# Extract to a location, e.g., C:\opencv

# Set environment variable
set OpenCV_DIR=C:\opencv\build
```

### 2. Install ONNX Runtime

```bash
# Download from https://github.com/microsoft/onnxruntime/releases
# Extract to a location, e.g., C:\onnxruntime

# Set environment variable
set ONNXRUNTIME_DIR=C:\onnxruntime
```

### 3. Clone and Navigate to Repository

```bash
git clone https://github.com/aboodjgmae2000-commits/ipro71-nexus.git
cd ipro71-nexus
git checkout develop
```

## Building

### Option 1: Using Build Script (Recommended)

```bash
# For Release build
BUILD_SCRIPT_WINDOWS.bat Release

# For Debug build
BUILD_SCRIPT_WINDOWS.bat Debug
```

### Option 2: Manual CMake Build

```bash
# Create build directory
mkdir build
cd build

# Configure (Visual Studio 2022)
cmake -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release ..

# Build
cmake --build . --config Release --verbose

# Verify
dir bin\Release\ipro71-nexus.exe
```

### Option 3: Using Visual Studio IDE

```bash
# Generate Visual Studio solution
mkdir build
cd build
cmake -G "Visual Studio 17 2022" ..

# Open in Visual Studio
start ipro71-nexus.sln

# Build -> Build Solution (Ctrl+Shift+B)
```

## Expected Build Output

### Success Indicators

```
-- Project: ipro71-nexus
-- C++ Standard: 17
-- OpenCV: 4.x.x
-- ONNX Runtime: Found
-- Build files have been written to: ...
```

```
[XX/XX] Linking CXX executable bin\Release\ipro71-nexus.exe
[100%] Built target ipro71-nexus
```

### Common Issues and Solutions

#### Error: "Could not find OpenCV"

**Solution:**
```bash
set OpenCV_DIR=C:\opencv\build
cmake -DCMAKE_PREFIX_PATH=C:\opencv\build ..
```

#### Error: "Could not find ONNX"

**Solution:**
```bash
set ONNX_DIR=C:\onnxruntime\cmake
cmake -DONNX_DIR=C:\onnxruntime\cmake ..
```

#### Error: "Unresolved external symbol"

**Solution:**
1. Verify OpenCV_LIBS are correct
2. Verify onnxruntime.lib is in library paths
3. Check CMakeLists.txt target_link_libraries

#### Error: "CXX1300: An internal error has occurred"

**Solution:**
1. Clean build: `cmake --build . --config Release --clean-first`
2. Update MSVC: `cl.exe /?` to check version
3. If issue persists, rebuild from scratch:
   ```bash
   rmdir /s build
   mkdir build
   cd build
   cmake ...
   ```

## Troubleshooting

### Verify CMake Configuration

```bash
cd build
cmake . --debug-output
```

### Verbose Build Output

```bash
cmake --build . --config Release --verbose
```

### Check Dependencies

```bash
# List found packages
cmake --debug-output | findstr "Found"
```

## Running the Application

### Command Line

```bash
# Navigate to executable directory
cd build\bin\Release

# Run
ipro71-nexus.exe
```

### Expected Console Output

```
================================
ipro71-nexus - Professional Engine
================================

[INIT] Initializing screen capture (640x640)...
[INIT] Loading ONNX model: model.onnx
[INFO] ONNX model loaded successfully
[INFO] Number of inputs: 1
[INFO] Number of outputs: 1
[INFO] Input 0: images
[INFO] Output 0: output0
[INFO] Input shape: 1 640 640 3
[INIT] Initializing overlay...
[INFO] Overlay window initialized 1920x1080

[INFO] Engine ready. Controls:
  - ESC: Exit
  - RIGHT MOUSE BUTTON: Activate targeting
```

## Build Artifacts

```
build/
├── bin/
│   ├── Release/
│   │   └── ipro71-nexus.exe          (main executable)
│   └── Debug/
│       └── ipro71-nexus.exe
├── CMakeFiles/                       (CMake intermediate files)
├── CMakeCache.txt                    (CMake configuration cache)
└── ipro71-nexus.sln                  (Visual Studio solution)
```

## Clean Build

```bash
# Remove all build artifacts
rmdir /s /q build

# Recreate build directory
mkdir build
cd build

# Start fresh build
cmake -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

## Performance Build

For optimized release build:

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="/O2 /W4" ..
cmake --build . --config Release
```

## Additional Notes

- **Model File**: Place `model.onnx` in the same directory as the executable
- **Resolution**: Default is 640x640 capture, 1920x1080 screen
- **Threads**: ONNX uses 4 threads for inference (configurable in config.h)
- **Windows Version**: Requires Windows 7 or later for all GDI functions

## Version Information

- **Project Version**: 1.0.0
- **C++ Standard**: C++17
- **CMake**: 3.20+
- **OpenCV**: 4.x+
- **ONNX Runtime**: 1.8.0+

## Support

For build issues:
1. Check system requirements above
2. Review CMake output carefully
3. Verify environment variables are set
4. Try clean build (see "Clean Build" section)
5. Check GitHub issues for similar problems
