#pragma once

#ifndef USE_TEST_MILLIS
#include <chrono>

// Mock implementation of millis() for sandbox environment
inline unsigned long millis() {
    static auto startTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    return static_cast<unsigned long>(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count());
}
#endif