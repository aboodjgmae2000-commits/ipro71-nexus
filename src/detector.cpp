#include "detector.h"
#include "config.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <numeric>

struct DetectionBox {
    float cx, cy, w, h;
    float xmin, ymin, xmax, ymax;
    float confidence;
    int class_id;
};

struct LetterboxInfo {
    float scale;
    float pad_x, pad_y;
    int orig_width, orig_height;
};

static LetterboxInfo PreprocessLetterbox(const std::vector<unsigned char>& frameBuffer,
                                          int width, int height,
                                          std::vector<float>& input_tensor_values) {
    LetterboxInfo info;
    info.orig_width = width;
    info.orig_height = height;

    // Calculate scaling to fit frame into MODEL_INPUT_SIZE while maintaining aspect ratio
    float scale = std::min((float)Config::MODEL_INPUT_SIZE / (float)width,
                          (float)Config::MODEL_INPUT_SIZE / (float)height);
    int new_w = std::max(1, (int)std::round(width * scale));
    int new_h = std::max(1, (int)std::round(height * scale));
    info.scale = scale;
    info.pad_x = (Config::MODEL_INPUT_SIZE - new_w) / 2.0f;
    info.pad_y = (Config::MODEL_INPUT_SIZE - new_h) / 2.0f;

    size_t channel_size = (size_t)Config::MODEL_INPUT_SIZE * Config::MODEL_INPUT_SIZE;
    // Initialize with gray padding (114/255 is ultralytics default)
    input_tensor_values.assign(channel_size * 3, 114.0f / 255.0f);

    int pad_x_i = (int)info.pad_x;
    int pad_y_i = (int)info.pad_y;

    // Nearest-neighbor resize
    for (int y = 0; y < new_h; ++y) {
        int src_y = std::min(height - 1, (int)(y / scale));
        int dst_y = y + pad_y_i;
        if (dst_y < 0 || dst_y >= Config::MODEL_INPUT_SIZE) continue;

        for (int x = 0; x < new_w; ++x) {
            int src_x = std::min(width - 1, (int)(x / scale));
            int dst_x = x + pad_x_i;
            if (dst_x < 0 || dst_x >= Config::MODEL_INPUT_SIZE) continue;

            // frameBuffer is BGRA
            size_t src_idx = ((size_t)src_y * width + src_x) * 4;
            size_t dst_idx = (size_t)dst_y * Config::MODEL_INPUT_SIZE + dst_x;

            input_tensor_values[dst_idx]                              = frameBuffer[src_idx + 2] / 255.0f;  // R
            input_tensor_values[dst_idx + channel_size]               = frameBuffer[src_idx + 1] / 255.0f;  // G
            input_tensor_values[dst_idx + 2 * channel_size]           = frameBuffer[src_idx + 0] / 255.0f;  // B
        }
    }

    return info;
}

Detector::Detector(const wchar_t* model_path)
    : env(ORT_LOGGING_LEVEL_WARNING, "ipro71-nexus"),
      session(nullptr),
      memory_info(Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU)) {

    if (!model_path) {
        std::cerr << "[ERROR] Model path is null" << std::endl;
        return;
    }

    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(Config::ONNX_THREADS);
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

    try {
        session = new Ort::Session(env, model_path, session_options);
        
        // Log model info
        std::cout << "[INFO] ONNX model loaded successfully" << std::endl;
        std::cout << "[INFO] Number of inputs: " << session->GetInputCount() << std::endl;
        std::cout << "[INFO] Number of outputs: " << session->GetOutputCount() << std::endl;

        // Get input names using GetInputNameAllocated
        Ort::AllocatorWithDefaultOptions allocator;
        for (size_t i = 0; i < session->GetInputCount(); ++i) {
            auto name_allocator = session->GetInputNameAllocated(i, allocator);
            std::string input_name(name_allocator.get());
            input_names.push_back(input_name);
            std::cout << "[INFO] Input " << i << ": " << input_name << std::endl;
        }

        // Get output names using GetOutputNameAllocated
        for (size_t i = 0; i < session->GetOutputCount(); ++i) {
            auto name_allocator = session->GetOutputNameAllocated(i, allocator);
            std::string output_name(name_allocator.get());
            output_names.push_back(output_name);
            std::cout << "[INFO] Output " << i << ": " << output_name << std::endl;
        }

        // Get input shape
        auto input_shape_info = session->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo();
        input_shape = input_shape_info.GetShape();
        std::cout << "[INFO] Input shape: ";
        for (auto dim : input_shape) {
            std::cout << dim << " ";
        }
        std::cout << std::endl;

    } catch (const Ort::Exception& e) {
        std::cerr << "[ERROR] Failed to create ONNX session: " << e.what() << std::endl;
        session = nullptr;
    }
}

Detector::~Detector() {
    if (session) {
        delete session;
        session = nullptr;
    }
}

std::vector<Target> Detector::DetectTargets(const std::vector<unsigned char>& frameBuffer,
                                             int width, int height) {
    std::vector<Target> targets;

    if (!session) {
        std::cerr << "[ERROR] Detector not initialized" << std::endl;
        return targets;
    }

    if (width <= 0 || height <= 0) {
        std::cerr << "[ERROR] Invalid frame dimensions: " << width << "x" << height << std::endl;
        return targets;
    }

    if (frameBuffer.size() < (size_t)width * height * 4) {
        std::cerr << "[ERROR] Frame buffer too small" << std::endl;
        return targets;
    }

    // Preprocess
    std::vector<float> input_tensor_values;
    LetterboxInfo lb = PreprocessLetterbox(frameBuffer, width, height, input_tensor_values);

    std::vector<int64_t> input_shape_vec = { 1, 3, Config::MODEL_INPUT_SIZE, Config::MODEL_INPUT_SIZE };

    // Create input tensor
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
        memory_info, input_tensor_values.data(), input_tensor_values.size(),
        input_shape_vec.data(), input_shape_vec.size()
    );

    try {
        // Run inference
        std::vector<const char*> input_names_c = { Config::MODEL_INPUT_NAME };
        std::vector<const char*> output_names_c = { Config::MODEL_OUTPUT_NAME };

        auto output_tensors = session->Run(
            Ort::RunOptions{ nullptr },
            input_names_c.data(), &input_tensor, 1,
            output_names_c.data(), output_names_c.size()
        );

        if (output_tensors.empty()) {
            std::cerr << "[ERROR] No output tensors returned" << std::endl;
            return targets;
        }

        float* data = output_tensors[0].GetTensorMutableData<float>();
        auto type_info = output_tensors[0].GetTensorTypeAndShapeInfo();
        std::vector<int64_t> shape = type_info.GetShape();

        if (shape.size() < 3) {
            std::cerr << "[ERROR] Invalid output shape" << std::endl;
            return targets;
        }

        int num_channels = (int)shape[1];
        int num_proposals = (int)shape[2];

        std::cout << "[DEBUG] Output shape: [" << shape[0] << ", " << num_channels << ", " << num_proposals << "]" << std::endl;

        std::vector<DetectionBox> boxes;

        // Parse detections
        for (int i = 0; i < num_proposals; ++i) {
            // Find max confidence across classes
            float max_conf = 0.0f;
            int best_class = 0;
            for (int c = 4; c < num_channels; ++c) {
                float conf = data[c * num_proposals + i];
                if (conf > max_conf) {
                    max_conf = conf;
                    best_class = c - 4;
                }
            }

            if (max_conf > Config::CONFIDENCE_THRESHOLD) {
                float cx_model = data[0 * num_proposals + i];
                float cy_model = data[1 * num_proposals + i];
                float w_model = data[2 * num_proposals + i];
                float h_model = data[3 * num_proposals + i];

                // Convert from model space to original frame space
                float cx = (cx_model - lb.pad_x) / lb.scale;
                float cy = (cy_model - lb.pad_y) / lb.scale;
                float w = w_model / lb.scale;
                float h = h_model / lb.scale;

                float xmin = cx - (w / 2.0f);
                float ymin = cy - (h / 2.0f);
                float xmax = cx + (w / 2.0f);
                float ymax = cy + (h / 2.0f);

                // Sanity checks
                if (w > Config::MIN_BOX_WIDTH && h > Config::MIN_BOX_HEIGHT &&
                    w < width && h < height &&
                    cx >= 0 && cx <= width && cy >= 0 && cy <= height) {
                    boxes.push_back({ cx, cy, w, h, xmin, ymin, xmax, ymax, max_conf, best_class });
                }
            }
        }

        // Sort by confidence
        std::sort(boxes.begin(), boxes.end(), [](const DetectionBox& a, const DetectionBox& b) {
            return a.confidence > b.confidence;
        });

        // NMS
        std::vector<DetectionBox> selected_boxes;
        for (const auto& box : boxes) {
            bool keep = true;
            for (const auto& selected : selected_boxes) {
                float iou = 0.0f;
                {
                    float inter_xmin = std::max(box.xmin, selected.xmin);
                    float inter_ymin = std::max(box.ymin, selected.ymin);
                    float inter_xmax = std::min(box.xmax, selected.xmax);
                    float inter_ymax = std::min(box.ymax, selected.ymax);

                    if (inter_xmax > inter_xmin && inter_ymax > inter_ymin) {
                        float inter_area = (inter_xmax - inter_xmin) * (inter_ymax - inter_ymin);
                        float area_box = (box.xmax - box.xmin) * (box.ymax - box.ymin);
                        float area_selected = (selected.xmax - selected.xmin) * (selected.ymax - selected.ymin);
                        float union_area = area_box + area_selected - inter_area;
                        if (union_area > 0) {
                            iou = inter_area / union_area;
                        }
                    }
                }

                if (iou > Config::NMS_THRESHOLD) {
                    keep = false;
                    break;
                }
            }

            if (keep) {
                selected_boxes.push_back(box);
                if ((int)selected_boxes.size() >= Config::MAX_DETECTIONS) {
                    break;
                }
            }
        }

        // Convert to Target format
        for (const auto& sb : selected_boxes) {
            targets.push_back({ sb.cx, sb.cy, sb.w, sb.h, sb.confidence, sb.class_id });
        }

        if (Config::DEBUG_MODE) {
            std::cout << "[DEBUG] Detected " << targets.size() << " targets after NMS" << std::endl;
        }

    } catch (const Ort::Exception& e) {
        std::cerr << "[ERROR] Inference failed: " << e.what() << std::endl;
    }

    return targets;
}
