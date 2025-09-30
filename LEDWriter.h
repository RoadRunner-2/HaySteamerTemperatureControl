#ifndef LED_WRITER_H
#define LED_WRITER_H

#include <functional>

#ifdef SANDBOX_ENVIRONMENT
#pragma once

#include "Sandbox/StringConversion.h"
#include "Sandbox/millis.h"
#include "Sandbox/Actor.h"
#include "Sandbox/CyclicModule.h"
#include "Sandbox/Status.h"
using namespace std;
#endif

#ifdef ARDUINO
#include <String.h>
#include <functional>
// millis() is provided by the Arduino framework, no need to define it
#include <CyclicModule.h>
#include <Actor.h>
#include <Arduino.h>
#include <Status.h> 
#endif

using LED = Actor<Status>;

class LEDWriter : public CyclicModule {
public:
    using ContentProvider = std::function<Status()>;
    LEDWriter(LED* led)
        :led(led)
    { };
    ~LEDWriter() { };

    /// <summary>
    /// set (and overwrite) content provider for all lines
    /// </summary>
    void setProvider(ContentProvider provider)
    {
        if (!provider) {
            return;
        }
        LED_condition = provider;
    };

    /// <summary>
    /// calls ContentProvider to update all lines and writes to hardware
    /// call cyclically
    /// </summary>
    void update() override
    {
        if (!LED_condition) {
            return;
        }
        led->write(LED_condition());
    };

private:
    LED* led;
    ContentProvider LED_condition;
};

#endif