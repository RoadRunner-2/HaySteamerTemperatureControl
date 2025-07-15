#ifndef KEYPADREADER_H
#define KEYPADREADER_H

#ifdef SANDBOX_ENVIRONMENT
#pragma once

#include "Sandbox/StringConversion.h"
#include "Sandbox/Sensor.h"
#include "Sandbox/Status.h"
#include "Sandbox/CyclicModule.h"

#endif

#ifdef ARDUINO
// Define toString(int) as String(int)
using Fptr = std::string(*)(int);
constexpr Fptr toString = String;

#include <Sensor.h>
#include <Status.h>
#include <CyclicModule.h>
#endif

using Keypad = Sensor<char>;

class KeypadReader : public CyclicModule {
public:
    // No interval parameter needed anymore
    KeypadReader(Keypad* keypad)
        : keypad(keypad), lastValue(0) {
    }

    /// <summary>
    /// This function updates the lastValue with the current sensor reading
    /// It should be called periodically to keep the lastValue updated.
    /// </summary>
    void update() override {
        lastValue = keypad->read();
    }

    /// <summary>
    /// Returns the last read temperature value in °C.
    /// </summary>
    /// <returns>Last temperature value in °C.</returns>
    char getLatestValue() const {
        return lastValue;
    }

private:
    Keypad* keypad;
    int lastValue;
};

#endif
