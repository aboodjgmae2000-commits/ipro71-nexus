#pragma once
#include <windows.h>

/**
 * InputManager: Handles mouse and keyboard input.
 * 
 * Provides methods for moving the mouse with smoothing applied.
 */
class InputManager {
public:
    /**
     * Move mouse relative to current position with smoothing.
     * @param deltaX Horizontal offset
     * @param deltaY Vertical offset
     * @param smooth Smoothing factor (higher = slower)
     */
    static void MoveMouse(int deltaX, int deltaY, float smooth);

    /**
     * Check if right mouse button is pressed.
     */
    static bool IsRightMouseButtonPressed();

    /**
     * Check if escape key is pressed.
     */
    static bool IsEscapeKeyPressed();
};
