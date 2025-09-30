#ifndef FAULTCONDITIONS_H
#define FAULTCONDITIONS_H

#include <functional>
#include <string>
#include <vector>

#ifdef SANDBOX_ENVIRONMENT
#pragma once

#include "Sandbox/StringConversion.h"
#include "Sandbox/Status.h"
#endif

#ifdef ARDUINO
#include "Status.h"
#endif

class FaultConditions {
public:
    using FaultCondition = std::function<bool(Status)>;

    /// <summary>
    /// Adds a fault condition and its associated message to the conditions list if the condition is valid.
    /// </summary>
    /// <param name="condition">The fault condition to add.</param>
    /// <param name="message">The message associated with the fault condition.</param>
    void addCondition(FaultCondition condition, const String& message) {
        if (!condition) return;
        conditions.emplace_back(condition, message);
    }

    /// <summary>
    /// Checks all fault conditions and returns the message of the first condition that is met.
    /// </summary>
    /// <returns>The message of the first met condition, or empty string if none are met.</returns>
    String checkConditions(Status state) const {
        for (size_t i = 0; i < conditions.size(); i++) {
            const auto& condition_pair = conditions[i];
            if (condition_pair.first(state)) {
                return condition_pair.second;
            }
        }
        return "";
    }

private:
    std::vector<std::pair<FaultCondition, String>> conditions;
};

#endif