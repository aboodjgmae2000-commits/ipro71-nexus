#pragma once

/**
 * Central configuration for ipro71-nexus
 * All tunable parameters are centralized here.
 */

namespace Config {

// ===== Screen & Capture =====
constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;
constexpr int CAPTURE_WIDTH = 640;
constexpr int CAPTURE_HEIGHT = 640;

// ===== ONNX Model =====
// Update this to your model path relative to executable or use absolute path
const wchar_t* MODEL_PATH = L"model.onnx";

// Model input/output names (inspect your model with Netron)
const char* MODEL_INPUT_NAME = "images";
const char* MODEL_OUTPUT_NAME = "output0";

// Model input size (square, typically 640)
constexpr int MODEL_INPUT_SIZE = 640;

// ===== Detection Parameters =====
constexpr float CONFIDENCE_THRESHOLD = 0.55f;  // Minimum confidence to consider detection
constexpr float NMS_THRESHOLD = 0.45f;         // Non-Maximum Suppression IoU threshold
constexpr int MAX_DETECTIONS = 100;            // Maximum detections to process per frame
constexpr int MAX_TARGETS = 1;                 // Maximum targets to track simultaneously

// ===== FOV (Field of View) =====
constexpr float FOV_RADIUS = 150.0f;           // Radius of detection circle in pixels
constexpr bool ENABLE_FOV_FILTERING = true;    // Enable FOV-based target filtering

// ===== Input Control =====
constexpr float MOUSE_SMOOTH = 2.5f;           // Smoothing factor for mouse movement
constexpr bool ENABLE_MOUSE_CONTROL = true;   // Enable automatic mouse movement
constexpr bool ENABLE_AIMBOT = true;           // Enable aimbot feature

// ===== Overlay =====
constexpr bool ENABLE_OVERLAY = true;          // Enable transparent overlay rendering
constexpr bool SHOW_FOV_CIRCLE = true;         // Draw FOV circle on overlay
constexpr bool SHOW_DETECTIONS = true;         // Draw detection boxes on overlay
constexpr bool SHOW_CONFIDENCE = true;         // Display confidence values
constexpr bool DEBUG_MODE = false;             // Enable debug logging and additional info

// ===== Performance =====
constexpr int FRAME_SLEEP_MS = 12;             // Sleep time between frames (ms)
constexpr int ONNX_THREADS = 4;                // Number of ONNX inference threads

// ===== Color Values (RGB) =====
constexpr unsigned int COLOR_FOV = 0x00FF00;   // Green for FOV circle
constexpr unsigned int COLOR_BOX = 0xFF0000;   // Red for detection boxes
constexpr unsigned int COLOR_TEXT = 0x00FF00;  // Green for text

// ===== Minimum Box Dimensions (to filter false positives) =====
constexpr float MIN_BOX_WIDTH = 10.0f;
constexpr float MIN_BOX_HEIGHT = 20.0f;

}  // namespace Config
