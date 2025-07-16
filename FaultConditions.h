#ifndef FAULTCONDITIONS_H
#define FAULTCONDITIONS_H

#include <functional>
#include <string>
#include <vector>

#ifdef SANDBOX_ENVIRONMENT
#pragma once

#include "Sandbox/StringConversion.h"

#endif

#ifdef ARDUINO

#endif

class FaultConditions {
public:
	using FaultCondition = std::function<bool()>;

	/// <summary>
	/// Sets the default message using the provided function if it is valid.
	/// </summary>
	/// <param name="message">A function that returns a String to be used as the default message.</param>
	void addDefaultMessage(const std::function<String()>& message) {
		if (!message) return;
		defaultMessage = message;
	}

	/// <summary>
	/// Adds a fault condition and its associated message to the conditions list if the condition is valid.
	/// </summary>
	/// <param name="condition">The fault condition to add.</param>
	/// <param name="message">The message associated with the fault condition.</param>
	void addCondition(FaultCondition condition, const std::string& message) {
		if (!condition) return;
		conditions.emplace_back(condition, message);
	}

	/// <summary>
	/// Checks all fault conditions and returns the message of the first condition that is met.
	/// </summary>
	/// <returns>The message of the first met condition, or the default message if none are met.</returns>
	String checkConditions() const {
		for (const auto& [fault, msg] : conditions) {
			if (fault()) return msg;
		}
		return defaultMessage();
	}

private:
	std::vector<std::pair<FaultCondition, std::string>> conditions;
	std::function<String()> defaultMessage = [](){ return ""; };
};

#endif