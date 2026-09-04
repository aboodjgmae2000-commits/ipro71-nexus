#pragma once
#include <windows.h>
#include <vector>
#include "detector.h"
#include "target_selector.h"

/**
 * Overlay: Renders detection data on a transparent Windows overlay.
 * 
 * Creates a transparent, click-through window that displays:
 * - FOV circle
 * - Detection boxes
 * - Target information
 * - Debug information
 */
class Overlay {
private:
    HWND hwnd = nullptr;
    bool is_running = false;

public:
    Overlay();
    ~Overlay();

    /**
     * Initialize the overlay window.
     * @return true if successful
     */
    bool Initialize();

    /**
     * Update overlay with detection data.
     */
    void Update(const std::vector<Target>& detections,
                const TargetSelector::SelectedTarget* selected_target);

    /**
     * Process window messages and return whether the application should continue.
     */
    bool ProcessMessages();

    HWND GetHWND() const { return hwnd; }
    bool IsRunning() const { return is_running; }
    void RequestStop() { is_running = false; }
};
