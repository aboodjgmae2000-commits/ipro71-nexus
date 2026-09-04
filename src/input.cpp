#include "input.h"
#include <iostream>

void InputManager::MoveMouse(int deltaX, int deltaY, float smooth) {
    if (deltaX == 0 && deltaY == 0) {
        return;
    }

    // Apply smoothing
    float moveX = (float)deltaX / smooth;
    float moveY = (float)deltaY / smooth;

    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dx = (LONG)moveX;
    input.mi.dy = (LONG)moveY;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;

    if (!SendInput(1, &input, sizeof(INPUT))) {
        std::cerr << "[ERROR] Failed to send mouse input" << std::endl;
    }
}

bool InputManager::IsRightMouseButtonPressed() {
    return (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
}

bool InputManager::IsEscapeKeyPressed() {
    return (GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0;
}
