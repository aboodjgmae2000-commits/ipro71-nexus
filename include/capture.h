#pragma once
#include <windows.h>
#include <vector>

/**
 * ScreenCapture: Captures a region of the screen using Windows GDI.
 * 
 * Captures the center of the screen into a buffer in BGRA format.
 * The capture region is centered and has dimensions (width x height).
 */
class ScreenCapture {
private:
    int width;
    int height;
    HDC hdcScreen = nullptr;
    HDC hdcMem = nullptr;
    HBITMAP hBitmap = nullptr;
    BITMAPINFOHEADER bi = {};

public:
    /**
     * Initialize screen capture with specified dimensions.
     * @param w Capture width in pixels
     * @param h Capture height in pixels
     */
    ScreenCapture(int w, int h);
    ~ScreenCapture();

    /**
     * Capture a frame from the center of the screen.
     * @param buffer Output buffer (will be resized to w*h*4 bytes, BGRA format)
     * @return true if capture succeeded, false otherwise
     */
    bool CaptureFrame(std::vector<unsigned char>& buffer);

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
};
