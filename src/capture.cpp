#include "capture.h"
#include <iostream>

ScreenCapture::ScreenCapture(int w, int h) : width(w), height(h) {
    hdcScreen = GetDC(NULL);
    if (!hdcScreen) {
        std::cerr << "[ERROR] Failed to get screen DC" << std::endl;
        return;
    }

    hdcMem = CreateCompatibleDC(hdcScreen);
    if (!hdcMem) {
        std::cerr << "[ERROR] Failed to create compatible DC" << std::endl;
        ReleaseDC(NULL, hdcScreen);
        return;
    }

    // Setup BITMAPINFOHEADER for BGRA 32-bit format
    ZeroMemory(&bi, sizeof(BITMAPINFOHEADER));
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;  // Negative height = top-down DIB
    bi.biPlanes = 1;
    bi.biBitCount = 32;     // BGRA format
    bi.biCompression = BI_RGB;

    // Create DIB section
    void* pBits = nullptr;
    hBitmap = CreateDIBSection(hdcScreen, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &pBits, NULL, 0);
    if (!hBitmap) {
        std::cerr << "[ERROR] Failed to create DIB section" << std::endl;
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);
        return;
    }

    // Select bitmap into memory DC
    SelectObject(hdcMem, hBitmap);
}

ScreenCapture::~ScreenCapture() {
    if (hBitmap) {
        DeleteObject(hBitmap);
        hBitmap = NULL;
    }
    if (hdcMem) {
        DeleteDC(hdcMem);
        hdcMem = NULL;
    }
    if (hdcScreen) {
        ReleaseDC(NULL, hdcScreen);
        hdcScreen = NULL;
    }
}

bool ScreenCapture::CaptureFrame(std::vector<unsigned char>& buffer) {
    if (!hdcScreen || !hdcMem || !hBitmap) {
        std::cerr << "[ERROR] Capture objects not initialized" << std::endl;
        return false;
    }

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (screenWidth <= 0 || screenHeight <= 0) {
        std::cerr << "[ERROR] Invalid screen dimensions" << std::endl;
        return false;
    }

    // Calculate center position
    int startX = (screenWidth - width) / 2;
    int startY = (screenHeight - height) / 2;

    // Clamp to valid screen bounds
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    if (startX + width > screenWidth) startX = screenWidth - width;
    if (startY + height > screenHeight) startY = screenHeight - height;

    // Capture screen region to memory DC
    if (!BitBlt(hdcMem, 0, 0, width, height, hdcScreen, startX, startY, SRCCOPY | CAPTUREBLT)) {
        std::cerr << "[ERROR] BitBlt failed" << std::endl;
        return false;
    }

    // Resize buffer if needed
    size_t bufferSize = (size_t)width * height * 4;
    if (buffer.size() != bufferSize) {
        buffer.resize(bufferSize);
    }

    // Extract pixel data from DIB
    int lines = GetDIBits(hdcMem, hBitmap, 0, height, buffer.data(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);
    if (lines == 0) {
        std::cerr << "[ERROR] GetDIBits failed" << std::endl;
        return false;
    }

    return true;
}
