#ifndef TEMPREADER_H
#define TEMPREADER_H

#ifdef SANDBOX_ENVIRONMENT
#pragma once

#include "Sandbox/StringConversion.h"
#include "Sandbox/Sensor.h"
#include "Sandbox/Status.h"
#include "Sandbox/CyclicModule.h"

#endif

#ifdef ARDUINO
#include <Sensor.h>
#include <Status.h>
#include <CyclicModule.h>
#endif

using TempSensor = Sensor<int>;

class TempReader : public CyclicModule {
public:
    // No interval parameter needed anymore
    TempReader(TempSensor* sensor)
        : sensor(sensor), lastValue(0) {
    }

	/// <summary>
	/// This function updates the lastValue with the current sensor reading
	/// It should be called periodically to keep the lastValue updated.
	/// </summary>
    void update() override {
        lastValue = sensor->read();
    }

	/// <summary>
	/// Returns the last read temperature value in C.
	/// </summary>
	/// <returns>Last temperature value in C.</returns>
    int getLatestValue() const {
        return lastValue;
    }

	/// <summary>
	/// Get the latest value as string ("TTTC"), always 6 characters long,
	/// where TTT is the temperature in C.
	/// </summary>
	/// <returns>String representation of the last temperature value.</returns>
    String getDisplayString() const {
        String result = "";
		if (lastValue < 10) result += "  ";
		else if (lastValue < 100) result += " ";
        result += toString(lastValue) + "C";
        return result;
    }

private:
    TempSensor* sensor;
    int lastValue;
};

#endif
