#pragma once
#include <vector>
#include <string>
#include <onnxruntime_cxx_api.h>

/**
 * Target: Represents a detected object.
 */
struct Target {
    float x, y;              // Center coordinates in capture space
    float width, height;     // Bounding box dimensions
    float confidence;        // Detection confidence [0, 1]
    int class_id = 0;        // Detected class (default 0)
};

/**
 * Detector: ONNX-based object detection.
 * 
 * Loads an ONNX model and performs inference on captured frames.
 * Handles preprocessing, inference, and output parsing.
 */
class Detector {
private:
    Ort::Env env;
    Ort::Session* session;
    Ort::MemoryInfo memory_info;
    
    // Model metadata
    std::vector<std::string> input_names;
    std::vector<std::string> output_names;
    std::vector<int64_t> input_shape;

public:
    /**
     * Initialize detector with ONNX model.
     * @param model_path Path to .onnx model file
     */
    Detector(const wchar_t* model_path);
    ~Detector();

    /**
     * Detect targets in a frame.
     * @param frameBuffer Frame data in BGRA format
     * @param width Frame width
     * @param height Frame height
     * @return Vector of detected targets
     */
    std::vector<Target> DetectTargets(const std::vector<unsigned char>& frameBuffer, 
                                       int width, int height);

    bool IsInitialized() const { return session != nullptr; }
};
