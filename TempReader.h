#ifndef TEMPREADER_H
#define TEMPREADER_H

#ifdef SANDBOX_ENVIRONMENT
#pragma once

#include <string> // Include the standard string library
using String = std::string; // Define "String" as an alias for std::string
// Define toString(int) as std::to_string
using Fptr = std::string(*)(int);
constexpr Fptr toString = &std::to_string;

#include <chrono>
// Mock implementation of millis() for sandbox environment
inline unsigned long millis() {
    static auto startTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    return static_cast<unsigned long>(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count());
}
#include "Sandbox/Sensor.h"
#include "Sandbox/Status.h"

#endif

#ifdef ARDUINO
// Define toString(int) as String(int)
using Fptr = std::string(*)(int);
constexpr Fptr toString = String;

// millis() is provided by the Arduino framework, no need to define it
#include <Sensor.h>
#include <Status.h>
#endif

using TempSensor = Sensor<int>;

class TempReader {
public:
    // No interval parameter needed anymore
    TempReader(TempSensor* sensor)
        : sensor(sensor), lastValue(0) {
    }

	/// This function updates the lastValue with the current sensor reading
	/// It should be called periodically to keep the lastValue updated.
    void update() {
        lastValue = sensor->read();
    }

	/// Returns the last read temperature value in °C.
	/// <returns>Last temperature value in °C.</returns>
    int getLatestValue() const {
        return lastValue;
    }

	/// Get the latest value as string ("TTT°C"), always 6 characters long,
	/// where TTT is the temperature in °C.
	/// <returns>String representation of the last temperature value.</returns>
    String getDisplayString() const {
        char buf[8];
        snprintf(buf, sizeof(buf), "%3d°C", lastValue);
        return String(buf);
    }

private:
    TempSensor* sensor;
    int lastValue;
};

#endif
