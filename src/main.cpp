#include <iostream>
#include <vector>
#include <chrono>
#include <windows.h>

#include "capture.h"
#include "detector.h"
#include "input.h"
#include "config.h"
#include "target_selector.h"
#include "overlay.h"

int main() {
    std::cout << "================================" << std::endl;
    std::cout << "ipro71-nexus - Professional Engine" << std::endl;
    std::cout << "================================" << std::endl << std::endl;

    // Initialize screen capture
    std::cout << "[INIT] Initializing screen capture (" << Config::CAPTURE_WIDTH << "x"
              << Config::CAPTURE_HEIGHT << ")..." << std::endl;
    ScreenCapture capture(Config::CAPTURE_WIDTH, Config::CAPTURE_HEIGHT);
    std::vector<unsigned char> frameBuffer;

    // Initialize detector
    std::cout << "[INIT] Loading ONNX model: " << "model.onnx" << std::endl;
    Detector detector(Config::MODEL_PATH);
    if (!detector.IsInitialized()) {
        std::cerr << "[FATAL] Detector failed to initialize" << std::endl;
        return 1;
    }

    // Initialize overlay
    std::cout << "[INIT] Initializing overlay..." << std::endl;
    Overlay overlay;
    if (!overlay.Initialize()) {
        std::cerr << "[FATAL] Failed to initialize overlay" << std::endl;
        return 1;
    }

    std::cout << std::endl << "[INFO] Engine ready. Controls:" << std::endl;
    std::cout << "  - ESC: Exit" << std::endl;
    std::cout << "  - RIGHT MOUSE BUTTON: Activate targeting" << std::endl << std::endl;

    int frame_count = 0;
    auto last_print_time = std::chrono::high_resolution_clock::now();

    while (overlay.ProcessMessages()) {
        // Check for exit key
        if (InputManager::IsEscapeKeyPressed()) {
            std::cout << "[INFO] Escape key pressed, exiting..." << std::endl;
            break;
        }

        // Capture frame
        if (!capture.CaptureFrame(frameBuffer)) {
            std::cerr << "[ERROR] Failed to capture frame" << std::endl;
            Sleep(Config::FRAME_SLEEP_MS);
            continue;
        }

        // Run detection
        std::vector<Target> detections = detector.DetectTargets(
            frameBuffer, Config::CAPTURE_WIDTH, Config::CAPTURE_HEIGHT);

        // Select best target
        TargetSelector::SelectedTarget selected_target = TargetSelector::SelectBestTarget(
            detections,
            Config::CAPTURE_WIDTH, Config::CAPTURE_HEIGHT,
            Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);

        // Update overlay with detections
        overlay.Update(detections, 
                      (selected_target.distance_to_center >= 0.0f) ? &selected_target : nullptr);

        // Aimbot: Move mouse if right button is pressed and target is selected
        if (Config::ENABLE_AIMBOT && Config::ENABLE_MOUSE_CONTROL &&
            InputManager::IsRightMouseButtonPressed() &&
            selected_target.distance_to_center >= 0.0f) {
            InputManager::MoveMouse(
                (int)selected_target.delta_x,
                (int)selected_target.delta_y,
                Config::MOUSE_SMOOTH);
        }

        // Stats every second
        frame_count++;
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_print_time);
        if (elapsed.count() >= 1000) {
            std::cout << "[STATS] Frame " << frame_count << " | Detections: " << detections.size()
                      << " | Selected: " << (selected_target.distance_to_center >= 0.0f ? "Yes" : "No")
                      << std::endl;
            frame_count = 0;
            last_print_time = now;
        }

        // Frame delay
        Sleep(Config::FRAME_SLEEP_MS);
    }

    std::cout << "[INFO] Shutting down..." << std::endl;
    std::cout << "[INFO] Engine stopped safely." << std::endl;

    return 0;
}
