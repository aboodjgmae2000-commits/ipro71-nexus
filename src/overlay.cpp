#include "overlay.h"
#include "config.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// Global pointer for window procedure
static Overlay* g_overlay_instance = nullptr;

LRESULT CALLBACK OverlayWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_ERASEBKGND:
        return 1;  // Prevent background erase

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        if (g_overlay_instance) {
            // Rendering handled by Update()
        }

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        if (g_overlay_instance) {
            g_overlay_instance->RequestStop();
        }
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

Overlay::Overlay() : hwnd(nullptr), is_running(false) {
    g_overlay_instance = this;
}

Overlay::~Overlay() {
    if (hwnd) {
        DestroyWindow(hwnd);
        hwnd = nullptr;
    }
}

bool Overlay::Initialize() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    if (!hInstance) {
        std::cerr << "[ERROR] Failed to get module handle" << std::endl;
        return false;
    }

    // Register window class
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = OverlayWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"ipro71OverlayClass";
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    if (!RegisterClass(&wc)) {
        DWORD err = GetLastError();
        if (err != ERROR_CLASS_ALREADY_EXISTS) {
            std::cerr << "[ERROR] Failed to register window class: " << err << std::endl;
            return false;
        }
    }

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Create transparent, click-through overlay window
    hwnd = CreateWindowExW(
        WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST,
        L"ipro71OverlayClass",
        L"ipro71-nexus Overlay",
        WS_POPUP,
        0, 0, screenWidth, screenHeight,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) {
        std::cerr << "[ERROR] Failed to create overlay window" << std::endl;
        return false;
    }

    // Set layered window attributes (transparent background)
    if (!SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY)) {
        std::cerr << "[ERROR] Failed to set layered window attributes" << std::endl;
        DestroyWindow(hwnd);
        hwnd = nullptr;
        return false;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    is_running = true;
    std::cout << "[INFO] Overlay window initialized " << screenWidth << "x" << screenHeight << std::endl;

    return true;
}

void Overlay::Update(const std::vector<Target>& detections,
                     const TargetSelector::SelectedTarget* selected_target) {
    if (!hwnd || !is_running) {
        return;
    }

    HDC hdc = GetDC(hwnd);
    if (!hdc) {
        return;
    }

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;

    // Draw FOV circle
    if (Config::SHOW_FOV_CIRCLE) {
        HPEN hGreenPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
        HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
        SelectObject(hdc, hGreenPen);
        SelectObject(hdc, hNullBrush);

        int fovRadius = (int)Config::FOV_RADIUS;
        Ellipse(hdc, centerX - fovRadius, centerY - fovRadius,
                centerX + fovRadius, centerY + fovRadius);

        DeleteObject(hGreenPen);
    }

    // Draw detections
    if (Config::SHOW_DETECTIONS && !detections.empty()) {
        HPEN hRedPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
        SelectObject(hdc, hRedPen);
        SelectObject(hdc, hNullBrush);

        for (const auto& det : detections) {
            // Convert from capture space to screen space
            float offsetX = det.x - (Config::CAPTURE_WIDTH / 2.0f);
            float offsetY = det.y - (Config::CAPTURE_HEIGHT / 2.0f);
            float screenX = centerX + offsetX;
            float screenY = centerY + offsetY;

            int drawX = (int)(screenX - (det.width / 2.0f));
            int drawY = (int)(screenY - (det.height / 2.0f));
            int drawW = (int)det.width;
            int drawH = (int)det.height;

            Rectangle(hdc, drawX, drawY, drawX + drawW, drawY + drawH);

            // Draw confidence text
            if (Config::SHOW_CONFIDENCE) {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2) << det.confidence * 100 << "%";
                std::string conf_str = oss.str();

                SetTextColor(hdc, RGB(0, 255, 0));
                SetBkMode(hdc, TRANSPARENT);
                TextOutA(hdc, drawX, drawY - 20, conf_str.c_str(), (int)conf_str.length());
            }
        }

        DeleteObject(hRedPen);
    }

    // Draw selected target indicator
    if (selected_target && selected_target->distance_to_center >= 0.0f) {
        HPEN hYellowPen = CreatePen(PS_DASH, 3, RGB(255, 255, 0));
        HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
        SelectObject(hdc, hYellowPen);
        SelectObject(hdc, hNullBrush);

        float offsetX = selected_target->detection.x - (Config::CAPTURE_WIDTH / 2.0f);
        float offsetY = selected_target->detection.y - (Config::CAPTURE_HEIGHT / 2.0f);
        float screenX = centerX + offsetX;
        float screenY = centerY + offsetY;

        int drawX = (int)(screenX - (selected_target->detection.width / 2.0f));
        int drawY = (int)(screenY - (selected_target->detection.height / 2.0f));
        int drawW = (int)selected_target->detection.width;
        int drawH = (int)selected_target->detection.height;

        // Draw thicker border around selected target
        Rectangle(hdc, drawX - 2, drawY - 2, drawX + drawW + 2, drawY + drawH + 2);

        DeleteObject(hYellowPen);
    }

    ReleaseDC(hwnd, hdc);
}

bool Overlay::ProcessMessages() {
    MSG msg = { 0 };
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return is_running;
}
