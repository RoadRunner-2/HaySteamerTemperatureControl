#ifndef TaskScheduler_h
#define TaskScheduler_h

#include "TimeReader.h"
#include "TempReader.h"
#include "DisplayWriter.h"
#include "ParameterEditor.h"
#include "KeypadReader.h"
#include "DisplayWriter.h"
#include "RelayWriter.h"
#include "LEDWriter.h"
#include "HaySteamerLogic.h"
#include "StartConditions.h"
#include "FaultConditions.h"

#include <array>
#include <vector>


#ifdef SANDBOX_ENVIRONMENT
#pragma once

#include "Sandbox/StringConversion.h"
#include "Sandbox/millis.h"
#include "Sandbox/Sensor.h"
#include "Sandbox/Actor.h"

using namespace std;
#endif

#ifdef ARDUINO
#include "Sensor.h"
#include "Actor.h"
#include <Arduino.h>

#define toString(x) String(x)
#endif

//#define DEBUG

struct CyclicTask {
    CyclicTask(unsigned long cycle_interval)
    {
        cycle_interval <= 10 ? interval = 10 : interval = cycle_interval; // ensure interval is at least 10 ms
        if (interval > 10000) { // limit interval to 10 seconds
            interval = 10000;
		}
    }
	virtual ~CyclicTask() = default;

    virtual void initializeTaskTimer(const unsigned long& currentTimeStamp) {
        nextRun = currentTimeStamp + interval;
    };

    virtual bool isRunScheduled(const unsigned long& currentTimeStamp)
    {
		bool isRunScheduled = (currentTimeStamp >= nextRun);
		// correct timing if scheduled runs were missed
        while (currentTimeStamp >= nextRun) {
            nextRun += interval;
        }
        return isRunScheduled;
    }
    virtual void cycleTask()
    {
#ifdef DEBUG 
            Serial.print("interval: ");
            Serial.print(interval);
            Serial.print(" nextRun: ");
            Serial.println(nextRun);
#endif
        for (CyclicModule* module : modules) {
            if (module) module->update();
		}
    }

    virtual void addModule(CyclicModule* module) {
        modules.push_back(module);
	}
    std::vector<CyclicModule*> modules;

	unsigned long interval;   // internal in milliseconds
    unsigned long nextRun = 0;    // Timestamp of next execution
};

struct SlowInputTask : public CyclicTask {
    SlowInputTask(unsigned long interval)
        : CyclicTask(interval)
    { };
};

struct FastInputTask : public CyclicTask {
    FastInputTask(unsigned long interval)
        : CyclicTask(interval)
    { };
    void enable() { enabled = true; };
    void disable() { enabled = false; };

    void cycleTask() override {
        if (enabled) {
			CyclicTask::cycleTask();
        }
	}
    
	volatile bool enabled = true;
};

struct OutputTask : public CyclicTask {
    OutputTask(const unsigned long& slowInterval, const unsigned long& fastinterval)
        : CyclicTask(slowInterval)
		, slowInterval(slowInterval)
		, fastInterval(fastinterval)
    {
    };

    void initializeTaskTimer(const unsigned long& currentTimeStamp) override {
		CyclicTask::initializeTaskTimer(currentTimeStamp);
		lastRun = currentTimeStamp;
    };

    void enableFast() { 
        enabledFast = true; 
        if ((lastRun + fastInterval) < nextRun) {
			nextRun = lastRun + fastInterval; // adjust next run time to fast interval
        }
    };
    void disableFast() { enabledFast = false; };

    bool isRunScheduled(const unsigned long& currentTimeStamp) override
    {
        bool isRunScheduled = (currentTimeStamp >= nextRun);
        if (isRunScheduled) {
            lastRun = currentTimeStamp;
        }

        // correct timing if scheduled runs were missed
        while (currentTimeStamp >= nextRun) {
            if (enabledFast) {
                nextRun += fastInterval;
            } else {
                nextRun += slowInterval;
			}
        }
        
        return isRunScheduled;
    }

    volatile bool enabledFast = false;
	unsigned long slowInterval = 0;
    unsigned long fastInterval = 0;
    unsigned long lastRun = 0;

};

struct LogicTask : public CyclicTask {
    LogicTask(unsigned long interval)
        : CyclicTask(interval)
    { };
};

class CyclicCaller
{
public:
    CyclicCaller(Sensor<time_t>* clock, Sensor<int>* temp, Sensor<char>* keypad, Actor<String[4]>* display, Actor<byte>* relay, Actor<Status>* led)
        : slowInputTask(1000)
        , fastInputTask(100)
        , logicTask(2000)
        , outputTask(1000, 100)
		, timeReader(clock)
		, tempReader(temp)
		, keypadReader(keypad)
		, display(display)
		, relay(relay)
		, led(led)
    {
    };

    void initializeTasks() {
        unsigned long currentMillis = millis();
        for (CyclicTask* task : tasks) {
            task->initializeTaskTimer(currentMillis);
        }

		slowInputTask.addModule(&timeReader);
		slowInputTask.addModule(&tempReader);

		fastInputTask.addModule(&keypadReader);
		fastInputTask.addModule(&parameterEditor);

        logicTask.addModule(&logic);

		outputTask.addModule(&display);
		outputTask.addModule(&relay);
		outputTask.addModule(&led);


		ParameterEditor::CharacterProvider characterProvider = [&] { return keypadReader.getLatestValue(); };
		parameterEditor.setCharacterProvider(characterProvider);

        logic.setStartConditions([&] { return startConditions.checkAllConditions(); });
		logic.setRunTimer([&] { return startConditions.timerCondition(); });
		logic.setStartTimer([&] { return startTimer; });
        logic.setHasFault([&](Status state) { return faultConditions.checkConditions(state); });
		logic.setGetTimeOfDayInMinutes([&] { return timeReader.getTimeOfDayInMinutes(); });
		logic.setGetTemperature([&] { return tempReader.getLatestValue(); });
		logic.setGetMinimumTemperature([&] { return parameterEditor.getTemperature(); });
		logic.setGetWaitTime([&] { return parameterEditor.getTimeSpan(); });

		startConditions.setGetTimeOfDayInMinutes([&] { return timeReader.getTimeOfDayInMinutes(); });
		startConditions.setGetStartTimeInMinutes([&] { return parameterEditor.getTimeInMinutes(); });

        DisplayWriter::ContentProvider line1 = [&] { return timeReader.getDisplayString(); };
        DisplayWriter::ContentProvider line2 = [&] { return logic.getMessage(); };
        DisplayWriter::ContentProvider line3 = [&] { return tempReader.getDisplayString(); };
		DisplayWriter::ContentProvider line4 = [&] { return parameterEditor.getDisplayString(); };
        display.setAllProvider(line1, line2, line3, line4);
        relay.setProvider([&] { return byte{ ((logic.getCurrentStatus() == Status::heating) || (logic.getCurrentStatus() == holding)) }; });
		led.setProvider([&] { return logic.getCurrentStatus(); });
    }

    void attach_start_condition(const StartConditions::ConditionFunction& condition)
    {
        startConditions.addCondition(condition);
    };

    void attach_fault_condition(const FaultConditions::FaultCondition& condition, const String& message)
    {
        faultConditions.addCondition(condition, message);
	};

    void enableFastInputTask() {
        fastInputTask.enable();
	};

    void disableFastInputTask() {
		fastInputTask.disable();
	};

    // execute cyclic tasks with adaptive timing
    void executeCyclicTasks() {
        unsigned long currentMillis = millis();

        for (CyclicTask* task : tasks) {
            if (task->isRunScheduled(currentMillis)) {
                task->cycleTask();
            }
        }
    }

public:
	volatile bool startTimer = false;

private:
    SlowInputTask slowInputTask;
    FastInputTask fastInputTask;
    LogicTask logicTask;
    OutputTask outputTask;
    std::array<CyclicTask*, 4> tasks{ &slowInputTask, &fastInputTask, &logicTask, &outputTask };

	// modules in slow input task
    TimeReader timeReader;
    TempReader tempReader;

	// modules in fast input task
    KeypadReader keypadReader;
    ParameterEditor parameterEditor;

    // modules in logic task
    HaySteamerLogic logic;
	StartConditions startConditions;
	FaultConditions faultConditions;

	// modules in output task
    DisplayWriter display;
	RelayWriter relay;
	LEDWriter led;
	
};

#endif
