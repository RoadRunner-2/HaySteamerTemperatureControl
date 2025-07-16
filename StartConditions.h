#ifndef STARTCONDITIONS_H
#define STARTCONDITIONS_H

#include <functional>

#ifdef SANDBOX_ENVIRONMENT
#pragma once

#endif

#ifdef ARDUINO

#endif

class StartConditions
{
public:
	using ConditionFunction = std::function<bool()>;
	StartConditions() = default;

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

private:
	std::vector<ConditionFunction> conditions;
};
#endif
