#include "target_selector.h"
#include "config.h"
#include "math_utils.h"
#include <iostream>
#include <algorithm>
#include <limits>

void TargetSelector::ConvertToScreenSpace(float& x, float& y,
                                          int capture_width, int capture_height,
                                          int screen_width, int screen_height) {
    // Calculate offset from capture center
    float offset_x = x - (capture_width / 2.0f);
    float offset_y = y - (capture_height / 2.0f);

    // Add to screen center
    x = (screen_width / 2.0f) + offset_x;
    y = (screen_height / 2.0f) + offset_y;
}

float TargetSelector::CalculateTargetScore(float distance_to_center) {
    // Lower score is better (closer to center)
    // Linear scoring based on distance
    return distance_to_center;
}

TargetSelector::SelectedTarget TargetSelector::SelectBestTarget(
    const std::vector<Target>& detections,
    int capture_width, int capture_height,
    int screen_width, int screen_height) {

    SelectedTarget result = {};

    if (detections.empty()) {
        if (Config::DEBUG_MODE) {
            std::cout << "[DEBUG] No detections to select from" << std::endl;
        }
        return result;
    }

    // Filter by confidence
    std::vector<Target> filtered;
    for (const auto& det : detections) {
        if (det.confidence >= Config::CONFIDENCE_THRESHOLD) {
            filtered.push_back(det);
        }
    }

    if (filtered.empty()) {
        if (Config::DEBUG_MODE) {
            std::cout << "[DEBUG] No detections passed confidence threshold" << std::endl;
        }
        return result;
    }

    // Calculate screen center in capture space
    float capture_center_x = capture_width / 2.0f;
    float capture_center_y = capture_height / 2.0f;

    // Filter by FOV if enabled
    std::vector<Target> fov_filtered;
    if (Config::ENABLE_FOV_FILTERING) {
        for (const auto& det : filtered) {
            if (MathUtils::IsPointInCircle(det.x, det.y, capture_center_x, capture_center_y, Config::FOV_RADIUS)) {
                fov_filtered.push_back(det);
            }
        }
    } else {
        fov_filtered = filtered;
    }

    if (fov_filtered.empty()) {
        if (Config::DEBUG_MODE) {
            std::cout << "[DEBUG] No detections within FOV" << std::endl;
        }
        return result;
    }

    // Score and find best target
    float best_score = std::numeric_limits<float>::max();
    int best_idx = -1;

    for (int i = 0; i < (int)fov_filtered.size(); ++i) {
        float dist = MathUtils::CalculateDistance(fov_filtered[i].x, fov_filtered[i].y,
                                                  capture_center_x, capture_center_y);
        float score = CalculateTargetScore(dist);

        if (score < best_score) {
            best_score = score;
            best_idx = i;
        }
    }

    if (best_idx >= 0) {
        result.detection = fov_filtered[best_idx];
        result.distance_to_center = best_score;

        // Calculate delta from screen center
        float screen_x = result.detection.x;
        float screen_y = result.detection.y;
        ConvertToScreenSpace(screen_x, screen_y, capture_width, capture_height, screen_width, screen_height);

        float screen_center_x = screen_width / 2.0f;
        float screen_center_y = screen_height / 2.0f;

        result.delta_x = screen_x - screen_center_x;
        result.delta_y = screen_y - screen_center_y;

        if (Config::DEBUG_MODE) {
            std::cout << "[DEBUG] Selected target: conf=" << result.detection.confidence
                      << " dist=" << result.distance_to_center
                      << " delta=(" << result.delta_x << ", " << result.delta_y << ")" << std::endl;
        }
    }

    return result;
}
