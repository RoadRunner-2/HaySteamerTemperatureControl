#ifndef STARTCONDITIONS_H
#define STARTCONDITIONS_H

#include <functional>
#include <vector> // Ensure vector is included

#ifdef SANDBOX_ENVIRONMENT
#pragma once

#endif

#ifdef ARDUINO

#endif

class StartConditions
{
public:
	using ConditionFunction = std::function<bool()>;
	StartConditions()
	{ };

	void setGetTimeOfDayInMinutes(std::function<unsigned long()> func) 
	{
		if (!func) {
			return;
		}
		getTimeOfDayInMinutes = func;
	}
	void setGetStartTimeInMinutes(std::function<unsigned long()> func) 
	{
		if (!func) {
			return;
		}
		getStartTimeInMinutes = func;
	}

	/// <summary>
	///	add condition to the list of conditions.
	/// </summary>
	/// <param name="condition function to add"></param>
	void addCondition(const ConditionFunction& condition) 
	{
		if (!condition) {
			return;
		}
		conditions.push_back(condition);
	}
	/// <summary>
	///	check all conditions and return true if any condition is met.
	/// </summary>
	/// <returns>true if any condition is met, false otherwise</returns>
	bool checkAllConditions() const 
	{
		bool conditionMet = false;
		for (const auto& condition : conditions) {
			conditionMet |= condition();
		}
		return conditionMet;
	}

	/// <summary>
	///	check if the timer condition is met, i.e., if the current time of day is greater than or equal to the start time.
	/// </summary>
	/// <returns>true if the timer condition is met, false otherwise</returns>
	bool timerCondition() const 
	{
		return (getTimeOfDayInMinutes() >= getStartTimeInMinutes());
	}

private:
	std::vector<ConditionFunction> conditions;

	std::function<unsigned long()> getTimeOfDayInMinutes = []() {return 0; };
	std::function<unsigned long()> getStartTimeInMinutes = []() {return 0; };
};
#endif
