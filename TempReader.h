#ifndef TEMPREADER_H
#define TEMPREADER_H

#ifdef SANDBOX_ENVIRONMENT
#pragma once

#include "Sandbox/StringConversion.h"
#include "Sandbox/Sensor.h"
#include "Sandbox/Status.h"

#endif

#ifdef ARDUINO
// Define toString(int) as String(int)
using Fptr = std::string(*)(int);
constexpr Fptr toString = String;

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

	/// <summary>
	/// This function updates the lastValue with the current sensor reading
	/// It should be called periodically to keep the lastValue updated.
	/// </summary>
    void update() {
        lastValue = sensor->read();
    }

	/// <summary>
	/// Returns the last read temperature value in °C.
	/// </summary>
	/// <returns>Last temperature value in °C.</returns>
    int getLatestValue() const {
        return lastValue;
    }

	/// <summary>
	/// Get the latest value as string ("TTT°C"), always 6 characters long,
	/// where TTT is the temperature in °C.
	/// </summary>
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
