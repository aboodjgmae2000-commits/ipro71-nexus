# CMake Troubleshooting Guide

## Common CMake Issues

### Issue 1: OpenCV Not Found

**Error Message:**
```
CMake Error at CMakeLists.txt:14 (find_package):
  Could not find OpenCV
```

**Diagnosis:**
```bash
cmake -DCMAKE_PREFIX_PATH=C:\opencv\build ..
cmake . --debug-output | findstr "OpenCV"
```

**Solutions:**
1. Set OpenCV_DIR before CMake:
   ```bash
   set OpenCV_DIR=C:\opencv\build
   cmake ..
   ```

2. Pass as CMake argument:
   ```bash
   cmake -DOpenCV_DIR=C:\opencv\build ..
   ```

3. Use CMake GUI:
   ```bash
   cmake-gui
   # Set OpenCV_DIR in GUI, then Configure
   ```

### Issue 2: ONNX Runtime Not Found

**Error Message:**
```
CMake Error at CMakeLists.txt:19 (find_package):
  Could not find ONNX
```

**Diagnosis:**
```bash
echo %ONNXRUNTIME_DIR%
dir %ONNXRUNTIME_DIR%\cmake
```

**Solutions:**
1. Set ONNXRUNTIME_DIR:
   ```bash
   set ONNXRUNTIME_DIR=C:\onnxruntime
   cmake ..
   ```

2. Set ONNX_DIR:
   ```bash
   cmake -DONNX_DIR=C:\onnxruntime\cmake ..
   ```

3. Verify ONNX Runtime structure:
   ```
   onnxruntime/
   ├── cmake/
   ├── include/
   └── lib/
   ```

### Issue 3: CMake Generator Not Found

**Error Message:**
```
CMake Error: Could not create working directory for try_compile check.
```

**Solutions:**
1. List available generators:
   ```bash
   cmake --help
   ```

2. Use correct generator for your VS version:
   - VS 2022: `"Visual Studio 17 2022"`
   - VS 2019: `"Visual Studio 16 2019"`
   - VS 2017: `"Visual Studio 15 2017"`
   - NMake: `"NMake Makefiles"`
   - Ninja: `"Ninja"`

3. Example:
   ```bash
   cmake -G "Visual Studio 17 2022" ..
   ```

### Issue 4: Configuration Cache Issues

**Error Message:**
```
The CMake files in the build directory were not created by this version of CMake.
```

**Solutions:**
1. Remove CMakeCache.txt:
   ```bash
   del CMakeCache.txt
   cmake ..
   ```

2. Remove entire build directory:
   ```bash
   cd ..
   rmdir /s /q build
   mkdir build
   cd build
   cmake ..
   ```

### Issue 5: Linking Issues

**Error Message:**
```
error LNK2019: unresolved external symbol ...
error LNK1120: X unresolved externals
```

**Diagnosis:**
1. Check if libraries exist:
   ```bash
   dir C:\opencv\build\lib\Release\*.lib
   dir C:\onnxruntime\lib\*.lib
   ```

2. Check CMakeLists.txt linking:
   ```cmake
   target_link_libraries(ipro71-nexus PRIVATE
       ${OpenCV_LIBS}
       onnxruntime
       user32 gdi32 kernel32
   )
   ```

**Solutions:**
1. Verify library paths in CMake output:
   ```bash
   cmake . --debug-output | findstr "OpenCV_LIBS"
   ```

2. Add library search paths:
   ```bash
   cmake -DCMAKE_PREFIX_PATH="C:\opencv\build;C:\onnxruntime" ..
   ```

3. Manually specify libraries:
   ```cmake
   link_directories(C:\opencv\build\lib C:\onnxruntime\lib)
   target_link_libraries(ipro71-nexus PRIVATE opencv_core opencv_imgproc onnxruntime)
   ```

## Debugging CMake

### Enable Debug Output

```bash
cmake --debug-output . > cmake_debug.log 2>&1
findstr "OpenCV" cmake_debug.log
```

### Print CMake Variables

Add to CMakeLists.txt:
```cmake
message(STATUS "OpenCV_FOUND: ${OpenCV_FOUND}")
message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")
message(STATUS "ONNX_FOUND: ${ONNX_FOUND}")
```

### Verbose Build

```bash
cmake --build . --verbose
REM or
cmake --build . -- VERBOSE=1
```

## CMake Cache Variables

### Important Variables to Check

```bash
# View all cache variables
cmake -L

# View specific variables
cmake -L | findstr "OpenCV"

# Edit with GUI
cmake-gui .
```

### Common Cache Variables

- `OpenCV_DIR` - OpenCV installation directory
- `ONNX_DIR` - ONNX Runtime cmake directory
- `CMAKE_BUILD_TYPE` - Release or Debug
- `CMAKE_GENERATOR` - Visual Studio version
- `CMAKE_PREFIX_PATH` - Additional search paths

## Advanced CMake Configuration

### Custom FindOpenCV Module

If find_package fails, create `cmake/FindOpenCV.cmake`:

```cmake
find_path(OpenCV_INCLUDE_DIR
    NAMES opencv2/core/core.hpp
    PATHS C:\\opencv\\include
)

find_library(OpenCV_CORE_LIB
    NAMES opencv_core
    PATHS C:\\opencv\\lib
)

find_library(OpenCV_IMGPROC_LIB
    NAMES opencv_imgproc
    PATHS C:\\opencv\\lib
)

if(OpenCV_INCLUDE_DIR AND OpenCV_CORE_LIB AND OpenCV_IMGPROC_LIB)
    set(OpenCV_FOUND TRUE)
    set(OpenCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIR})
    set(OpenCV_LIBS ${OpenCV_CORE_LIB} ${OpenCV_IMGPROC_LIB})
else()
    set(OpenCV_FOUND FALSE)
endif()
```

Then in CMakeLists.txt:
```cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(OpenCV REQUIRED)
```

## Verification Commands

```bash
# Check CMake version
cmake --version

# Check Visual Studio installation
where cl.exe
cl.exe /?

# Check libraries exist
dir C:\opencv\build\lib\Release\*.lib
dir C:\onnxruntime\lib\*.lib

# Test CMake can find packages
cmake -DCMAKE_PREFIX_PATH=C:\opencv\build;C:\onnxruntime . -L | findstr "OpenCV\|ONNX"
```

## Complete Fresh Build

```bash
# 1. Clean everything
rmdir /s /q build

# 2. Set environment
set OpenCV_DIR=C:\opencv\build
set ONNXRUNTIME_DIR=C:\onnxruntime

# 3. Create and configure
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release ..

# 4. Verify configuration
cmake . --debug-output | findstr "OpenCV\|ONNX\|Building"

# 5. Build
cmake --build . --config Release --verbose
```

## Support Resources

- [CMake Documentation](https://cmake.org/documentation/)
- [OpenCV CMake](https://docs.opencv.org/master/)
- [ONNX Runtime Build](https://onnxruntime.ai/docs/build/)
