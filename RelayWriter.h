#ifndef RELAY_WRITER_H
#define RELAY_WRITER_H

#include <functional>

#ifdef SANDBOX_ENVIRONMENT
#pragma once

#include "Sandbox/StringConversion.h"
#include "Sandbox/millis.h"
#include "Sandbox/Actor.h"
#include "Sandbox/CyclicModule.h"
using namespace std;
#endif

#ifdef ARDUINO
#include <string>
#include <functional>
// millis() is provided by the Arduino framework, no need to define it
#include <CyclicModule.h>
#include <Actor.h>
#include <Arduino.h>
#endif

using relay_output = Actor<byte>;

class RelayWriter : public CyclicModule {
public:
    using ContentProvider = std::function<byte()>;
    RelayWriter(relay_output* relay)
		: relay(relay)
    { };
    ~RelayWriter() {};

    /// <summary>
    /// set (and overwrite) content provider for all lines
    /// </summary>
    void setProvider(ContentProvider provider)
    {
        if (!provider) {
            return;
        }
        relay_condition = provider;
    };

    /// <summary>
    /// calls ContentProvider to update all lines and writes to hardware
    /// call cyclically
    /// </summary>
    void update() override
    {
        if (!relay_condition) {
            return;
        }
        currentState = relay_condition();

        relay->write(currentState);
    };

private:
    relay_output* relay;
    ContentProvider relay_condition;
    byte currentState = byte{ 0 };
};

#endif