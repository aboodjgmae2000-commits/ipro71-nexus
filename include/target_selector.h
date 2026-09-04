#pragma once
#include <vector>
#include "detector.h"

/**
 * TargetSelector: Selects the best target from detected objects.
 * 
 * Implements filtering (FOV, confidence), scoring, and selection logic.
 */
class TargetSelector {
public:
    /**
     * Structure representing a selected target with screen coordinates.
     */
    struct SelectedTarget {
        Target detection;           // Original detection
        float distance_to_center;   // Distance to screen center
        float delta_x, delta_y;     // Delta from screen center
    };

    /**
     * Select best target from detections.
     * Applies FOV filtering, confidence filtering, and scoring.
     * @param detections Vector of detected targets in capture space
     * @param capture_width Capture width
     * @param capture_height Capture height
     * @param screen_width Screen width
     * @param screen_height Screen height
     * @return Best target or empty SelectedTarget if none found
     */
    static SelectedTarget SelectBestTarget(const std::vector<Target>& detections,
                                           int capture_width, int capture_height,
                                           int screen_width, int screen_height);

private:
    /**
     * Convert coordinates from capture space to screen space.
     */
    static void ConvertToScreenSpace(float& x, float& y,
                                    int capture_width, int capture_height,
                                    int screen_width, int screen_height);

    /**
     * Calculate target score (lower is better/closer to center).
     */
    static float CalculateTargetScore(float distance_to_center);
};
