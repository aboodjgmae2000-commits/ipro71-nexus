#pragma once
#include <cmath>
#include <algorithm>

/**
 * Mathematical utilities for coordinate calculations and geometry.
 */
class MathUtils {
public:
    /**
     * Calculate Euclidean distance between two 2D points.
     */
    static float CalculateDistance(float x1, float y1, float x2, float y2) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        return std::sqrt(dx * dx + dy * dy);
    }

    /**
     * Check if a point (x, y) is within a circle.
     */
    static bool IsPointInCircle(float x, float y, float cx, float cy, float radius) {
        return CalculateDistance(x, y, cx, cy) <= radius;
    }

    /**
     * Clamp a value between min and max.
     */
    static float Clamp(float value, float min_val, float max_val) {
        return std::max(min_val, std::min(value, max_val));
    }

    /**
     * Linear interpolation between two values.
     */
    static float Lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    /**
     * Calculate intersection over union (IoU) between two rectangles.
     * Rectangles are defined as (xmin, ymin, xmax, ymax).
     */
    static float CalculateIoU(float x1_min, float y1_min, float x1_max, float y1_max,
                              float x2_min, float y2_min, float x2_max, float y2_max) {
        float inter_xmin = std::max(x1_min, x2_min);
        float inter_ymin = std::max(y1_min, y2_min);
        float inter_xmax = std::min(x1_max, x2_max);
        float inter_ymax = std::min(y1_max, y2_max);

        if (inter_xmax <= inter_xmin || inter_ymax <= inter_ymin) {
            return 0.0f;  // No intersection
        }

        float inter_area = (inter_xmax - inter_xmin) * (inter_ymax - inter_ymin);
        float area1 = (x1_max - x1_min) * (y1_max - y1_min);
        float area2 = (x2_max - x2_min) * (y2_max - y2_min);
        float union_area = area1 + area2 - inter_area;

        if (union_area <= 0.0f) {
            return 0.0f;
        }

        return inter_area / union_area;
    }
};
