# ipro71-nexus

**Windows C++ Application for Real-Time ONNX Object Detection with Screen Capture, FOV Filtering, and Transparent Overlay.**

## Overview

ipro71-nexus is a high-performance Windows desktop application that performs real-time object detection using ONNX Runtime. It captures the center of the screen, runs inference on captured frames, detects targets, filters them by field of view (FOV), and renders an interactive transparent overlay.

**Key Features:**
- Real-time screen capture (center region, customizable resolution)
- ONNX Runtime-based object detection
- Confidence and NMS filtering
- FOV-based target filtering with configurable radius
- Intelligent target selection (closest to screen center)
- Transparent, click-through overlay rendering
- Configurable mouse-based targeting and aimbot
- Debug logging and performance statistics

## Requirements

### System
- **OS:** Windows 10 or later (x64)
- **Architecture:** x86-64
- **RAM:** 4 GB minimum
- **GPU:** Optional (CPU inference supported)

### Build Tools
- **Compiler:** MSVC (Visual Studio 2019 or later) with C++17 support
- **Build System:** CMake 3.20+
- **Git:** For repository management

## Dependencies

The project requires the following libraries:

### 1. OpenCV (Core & Image Processing)
```bash
# Pre-built binaries available at: https://opencv.org/releases/
# Or install via package manager (vcpkg recommended):
vcpkg install opencv:x64-windows
```

### 2. ONNX Runtime (Inference Engine)
```bash
# Download pre-built packages from: https://github.com/microsoft/onnxruntime/releases
# Or via vcpkg:
vcpkg install onnxruntime:x64-windows
```

### 3. Windows SDK
Included with Visual Studio. Provides Windows API headers and libraries.

## Project Structure

```
ipro71-nexus/
├── CMakeLists.txt              # CMake build configuration
├── README.md                   # This file
├── .gitignore                  # Git ignore rules
├── include/                    # Header files
│   ├── config.h               # Central configuration
│   ├── capture.h              # Screen capture interface
│   ├── detector.h             # ONNX detector interface
│   ├── input.h                # Input management
│   ├── target_selector.h      # Target selection logic
│   ├── overlay.h              # Overlay rendering
│   └── math_utils.h           # Mathematical utilities
├── src/                        # Implementation files
│   ├── main.cpp               # Application entry point
│   ├── capture.cpp            # Screen capture implementation
│   ├── detector.cpp           # ONNX inference implementation
│   ├── input.cpp              # Input handling implementation
│   ├── target_selector.cpp    # Target selection implementation
│   └── overlay.cpp            # Overlay rendering implementation
└── model.onnx                 # ONNX model file (not included)
```

## Installation

### Step 1: Clone Repository
```bash
git clone https://github.com/aboodjgmae2000-commits/ipro71-nexus.git
cd ipro71-nexus
```

### Step 2: Install Dependencies (vcpkg)

**Install vcpkg if not already installed:**
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
cd ..
```

**Install required packages:**
```bash
.\vcpkg\vcpkg install opencv:x64-windows onnxruntime:x64-windows
```

### Step 3: Configure OpenCV (CMake)

If CMake cannot find OpenCV automatically, set the `OpenCV_DIR` environment variable or CMake parameter:

**Option A: Environment Variable**
```cmd
set OpenCV_DIR=C:\path\to\opencv\build
```

**Option B: CMake Parameter**
```bash
cmake -DOpenCV_DIR=C:\path\to\opencv\build ..
```

### Step 4: Configure ONNX Runtime

If CMake cannot find ONNX Runtime, set the path:

**Option A: Environment Variable**
```cmd
set ONNXRUNTIME_DIR=C:\path\to\onnxruntime
```

**Option B: CMake Parameter (with vcpkg)**
```bash
cmake -DCMAKE_TOOLCHAIN_FILE=.\vcpkg\scripts\buildsystems\vcpkg.cmake ..
```

## Building

### Step 1: Create Build Directory
```bash
mkdir build
cd build
```

### Step 2: Configure with CMake
```bash
# With vcpkg toolchain
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=..\vcpkg\scripts\buildsystems\vcpkg.cmake ..

# Or with manual paths
cmake -DCMAKE_BUILD_TYPE=Release -DOpenCV_DIR=C:\path\to\opencv\build ..
```

### Step 3: Build
```bash
cmake --build . --config Release
```

**Expected output:** `bin/ipro71-nexus.exe`

## Configuration

All configurable parameters are defined in `include/config.h`. Key settings:

### Capture Settings
```cpp
Config::CAPTURE_WIDTH = 640;   // Capture width
Config::CAPTURE_HEIGHT = 640;  // Capture height
```

### Detection Settings
```cpp
Config::CONFIDENCE_THRESHOLD = 0.55f;  // Min confidence
Config::NMS_THRESHOLD = 0.45f;         // NMS IoU threshold
Config::MAX_DETECTIONS = 100;          // Max detections per frame
```

### FOV Settings
```cpp
Config::FOV_RADIUS = 150.0f;           // Detection circle radius (pixels)
Config::ENABLE_FOV_FILTERING = true;   // Enable FOV filtering
```

### Input Control
```cpp
Config::ENABLE_AIMBOT = true;          // Enable aimbot
Config::ENABLE_MOUSE_CONTROL = true;   // Enable mouse movement
Config::MOUSE_SMOOTH = 2.5f;           // Smoothing factor
```

### Overlay Settings
```cpp
Config::ENABLE_OVERLAY = true;         // Show overlay
Config::SHOW_FOV_CIRCLE = true;        // Draw FOV circle
Config::SHOW_DETECTIONS = true;        // Draw detection boxes
Config::SHOW_CONFIDENCE = true;        // Show confidence values
Config::DEBUG_MODE = false;            // Enable debug output
```

## Model Configuration

The detector expects an ONNX model in a specific format:

### Model Path
```cpp
Config::MODEL_PATH = L"model.onnx";  // Relative or absolute path
```

### Model Input/Output Names
```cpp
Config::MODEL_INPUT_NAME = "images";   // Input tensor name
Config::MODEL_OUTPUT_NAME = "output0"; // Output tensor name
Config::MODEL_INPUT_SIZE = 640;        // Input resolution (square)
```

### Expected Model Format

The application assumes a YOLOv8-compatible ONNX model:

- **Input:** `[1, 3, 640, 640]` (batch, channels, height, width) in RGB format
- **Output:** `[1, 84, 8400]` (batch, channels, proposals)
  - Channels 0-3: Bounding box coordinates (cx, cy, w, h)
  - Channels 4+: Class probabilities
- **Preprocessing:** Letterbox resizing with gray padding (114, 114, 114)
- **Normalization:** Pixel values divided by 255

**To inspect your model:**
1. Install Netron: `pip install netron`
2. Open model: `netron model.onnx`
3. Verify input/output tensor names and shapes

If your model differs, update the configuration and detection parsing in `src/detector.cpp`.

## Running the Application

### Execution
```bash
bin/ipro71-nexus.exe
```

### Console Output
The application logs:
- Initialization status
- Model loading information
- Frame statistics (detections per second)
- Debug information (if `DEBUG_MODE` is enabled)

### Controls
- **ESC:** Exit application
- **RIGHT MOUSE BUTTON:** Activate aimbot (move mouse toward selected target)

### Overlay Display
- **Green Circle:** FOV detection radius
- **Red Rectangles:** Detected objects
- **Yellow Dashed Box:** Currently selected target
- **Green Text:** Detection confidence percentages

## Coordinate Systems

The application uses three coordinate systems:

1. **Capture Space:** (0, 0) to (CAPTURE_WIDTH, CAPTURE_HEIGHT)
   - Center of screen is captured region center
   - Detections reported in this space

2. **Model Space:** (0, 0) to (MODEL_INPUT_SIZE, MODEL_INPUT_SIZE)
   - ONNX model expects normalized input
   - Letterbox preprocessing used

3. **Screen Space:** (0, 0) to (SCREEN_WIDTH, SCREEN_HEIGHT)
   - Final rendering and input control
   - Overlay window matches screen dimensions

Automatic conversion handles mapping between spaces.

## Performance Considerations

### Optimization Tips
1. **Reduce capture resolution** if CPU usage is high (modify `CAPTURE_WIDTH/HEIGHT`)
2. **Increase detection threshold** to reduce false positives
3. **Reduce FOV radius** to limit target search area
4. **Disable debug mode** in production
5. **Use GPU inference** (requires ONNX Runtime with CUDA support)

### Typical Performance (CPU)
- **Inference:** 30-60 ms per frame (depends on model and CPU)
- **Overlay:** <5 ms per frame
- **Total:** 35-65 ms per frame (~15-30 FPS)

## Troubleshooting

### Issue: Model file not found
**Solution:** Place `model.onnx` in the working directory or update `Config::MODEL_PATH` with absolute path.

### Issue: "OpenCV not found" CMake error
**Solution:**
```bash
set OpenCV_DIR=C:\path\to\opencv\build
cmake ..
```

### Issue: "ONNX Runtime not found" CMake error
**Solution:**
```bash
# If using vcpkg, rebuild with toolchain:
cmake -DCMAKE_TOOLCHAIN_FILE=.\vcpkg\scripts\buildsystems\vcpkg.cmake ..

# Or set manually:
set ONNX_ROOT=C:\path\to\onnxruntime
```

### Issue: No detections showing
- Verify model format matches configuration (input/output names, tensor shapes)
- Check confidence threshold is reasonable for your model
- Ensure objects are in center capture region
- Review debug console output for inference errors

### Issue: Overlay window not appearing
- Ensure `ENABLE_OVERLAY = true` in config.h
- Check Windows permissions (may require admin on some systems)
- Verify screen resolution is detected correctly

### Issue: Mouse control not working
- Verify `ENABLE_MOUSE_CONTROL = true` and `ENABLE_AIMBOT = true`
- Check application is running with appropriate permissions
- Confirm target is selected (shown with yellow dashed box)
- Press and hold RIGHT MOUSE BUTTON to activate

## Known Limitations

1. **Windows Only:** Uses Windows API for screen capture and overlay
2. **ONNX Model Specific:** Requires properly formatted ONNX model
3. **Single Target Tracking:** `MAX_TARGETS = 1` by default (configurable)
4. **CPU Performance:** Inference speed depends on CPU and model complexity
5. **Screen Capture Latency:** ~10-20ms typical latency from real-time events
6. **Center-Only Capture:** Only captures center region (by design for targeting)

## Future Improvements

- [ ] GPU acceleration (ONNX Runtime with CUDA)
- [ ] Multi-target tracking
- [ ] Model hot-swap without restart
- [ ] Configuration file (JSON/YAML) instead of header-only
- [ ] Network streaming (remote model/overlay)
- [ ] Performance profiling framework

## License

This project is provided as-is for educational and authorized use only.

## Support

For issues, questions, or contributions:
- Check GitHub Issues for existing problems
- Review console output for detailed error messages
- Verify all dependencies are correctly installed
- Consult the troubleshooting section above

## References

- [ONNX Runtime Documentation](https://onnxruntime.ai/)
- [OpenCV Documentation](https://docs.opencv.org/)
- [YOLOv8 Official Repository](https://github.com/ultralytics/ultralytics)
- [Windows API Documentation](https://learn.microsoft.com/en-us/windows/win32/)
- [Netron Model Viewer](https://github.com/lutzroeder/netron)

---

**Version:** 1.0.0  
**Last Updated:** September 2026
