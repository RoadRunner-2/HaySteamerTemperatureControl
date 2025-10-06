#ifndef HAYSTEAMERLOGIC_H
#define HAYSTEAMERLOGIC_H

#include <functional>
#include "StateMachine.h"

#ifdef SANDBOX_ENVIRONMENT
#pragma once
#include "Sandbox/Status.h"
#include "Sandbox/StringConversion.h"
#include "Sandbox/CyclicModule.h"
#endif
#ifdef ARDUINO
#include "Status.h"
#include "CyclicModule.h"
#endif

class HaySteamerLogic : public CyclicModule
{
public:
    void update() override
    {
        switch (stateMachine.getCurrentStatus()) {
        case Status::idle:
            if (startConditions())
            {
                actualStartTime = getTimeOfDayInMinutes();
                stateMachine.changeStatus(Status::heating);
                message = "heating";
                minimumTemperature = getMinimumTemperature();
            }
            else if (startTimer())
            {
                stateMachine.changeStatus(Status::ready);
                message = "ready";
            }
			break;

        case Status::ready:
            if (runTimer())
            {
				actualStartTime = getTimeOfDayInMinutes();
                stateMachine.changeStatus(Status::heating);
				message = "heating";
				minimumTemperature = getMinimumTemperature();
            }
            break;
        case Status::heating:
            if (getTemperature() >= minimumTemperature)
            {
				reachedMinimumTemperature = getTimeOfDayInMinutes();
                stateMachine.changeStatus(Status::holding);
                message = "holding";
				waitTime = getWaitTime();
            }
            break;
        case Status::holding:
            if (getTimeOfDayInMinutes() - reachedMinimumTemperature >= waitTime)
            {
				timeWhenDone = getTimeOfDayInMinutes();
                stateMachine.changeStatus(Status::done);
                message = "done";
            }
            break;
        case Status::done:
            // signal done for an hour, the go back to idle
            if (getTimeOfDayInMinutes() - timeWhenDone >= 60)
            {
                stateMachine.changeStatus(Status::idle);
				message = "idle";

            }
            break;
        }

		checkFaults();
    }

    void setStartConditions(std::function<bool()> conditions) 
    { 
        if (!conditions) {
            return;
		}
        startConditions = conditions; 
    }
    void setStartTimer(std::function<bool()> func)
    {
        if (!func) {
            return;
        }
        startTimer = func;
    }
    void setRunTimer(std::function<bool()> func)
    {
        if (!func) {
            return;
        }
        runTimer = func;
    }
    void setHasFault(std::function<String(Status)> faultCondition) 
    { 
        if (!faultCondition) {
            return;
        }
        hasFault = faultCondition; 
        }
    void setGetTimeOfDayInMinutes(std::function<unsigned long()> timeFunction) 
    { 
        if (!timeFunction) {
            return;
		}
        getTimeOfDayInMinutes = timeFunction; 
    }
    void setGetTemperature(std::function<int()> temperatureFunction) 
    { 
        if (!temperatureFunction) {
            return;
        }
        getTemperature = temperatureFunction; 
    }
    void setGetMinimumTemperature(std::function<int()> getMinTemp) 
    { 
        if (!getMinTemp) {
            return;
		}
        getMinimumTemperature = getMinTemp; 
    }
    void setGetWaitTime(std::function<unsigned long()> getTimeSpan) 
    { 
        if (!getTimeSpan) {
            return;
        }
        getWaitTime = getTimeSpan; 
    }

    String getMessage() const { return message; }
	Status getCurrentStatus() const { return stateMachine.getCurrentStatus(); }
	
private:
    std::function<bool()> startConditions;
    std::function<bool()> startTimer;
    std::function<bool()> runTimer;
	std::function<String(Status)> hasFault;
    void checkFaults()
    {
        String errorMessage = "";
        switch (stateMachine.getCurrentStatus()) {
        case Status::heating:
            if (getTimeOfDayInMinutes() - actualStartTime > heatingTimeout)
            {
                stateMachine.changeStatus(Status::error);
                errorMessage = "heating timeout";
            }
            break;
        case Status::holding:
            if (getTemperature() < minimumTemperature - holdingTemperatureDrop)
            {
                stateMachine.changeStatus(Status::error);
                errorMessage = "temperature drop";
            }
            break;
        }

        if (errorMessage == "")
        {
            errorMessage = hasFault(stateMachine.getCurrentStatus());
        }

        if (errorMessage != "")
        {
            stateMachine.changeStatus(Status::error);
            message = errorMessage;
        }
        return;
    }
	std::function<unsigned long()> getTimeOfDayInMinutes;
	std::function<int()> getTemperature;

	HaySteamerStateMachine stateMachine;
    String message = "idle";

    // track process
    unsigned long actualStartTime = 0;
	unsigned long reachedMinimumTemperature = 0;
    unsigned long timeWhenDone = 0;
	// parameters for detecting faults
    unsigned long heatingTimeout = 60;
    int holdingTemperatureDrop = 5;
    // Parameters for the hay steaming process
	std::function<int()> getMinimumTemperature = []() { return 60; };
	int minimumTemperature;
	std::function<unsigned long()> getWaitTime = []() { return 30; };
    unsigned long waitTime;
};

#endif