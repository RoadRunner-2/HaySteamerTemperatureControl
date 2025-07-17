#ifndef StateMachine_h
#define StateMachine_h

#include <map>
#include <functional>


#ifdef SANDBOX_ENVIRONMENT
#pragma once

#include "Sandbox/StringConversion.h"
#include "Sandbox/Status.h" 
#endif

#ifdef ARDUINO
// Define toString(int) as String(int)
using Fptr = std::string(*)(int);
constexpr Fptr toString = String;

#include <Arduino.h>
#include <Status.h>
#endif

class HaySteamerStateMachine {
public:
  HaySteamerStateMachine()
      : currentStatus(Status::idle) {}

  void changeStatus(Status newStatus) {
    Status oldStatus = currentStatus;
    if (oldStatus == newStatus) return; // No Status change

    // Allow transition to error from any state
    if (newStatus == Status::error) {
      currentStatus = Status::error;
      return;
    }

    // If currently in error, only allow reset to idle
    if (oldStatus == Status::error && newStatus == Status::idle) {
      currentStatus = Status::idle;
      return;
    }

    // Allowed transitions
    if ((oldStatus == Status::idle    && newStatus == Status::ready)   ||
        (oldStatus == Status::idle    && newStatus == Status::heating) ||
        (oldStatus == Status::ready   && newStatus == Status::heating) ||
        (oldStatus == Status::heating && newStatus == Status::holding) ||
        (oldStatus == Status::holding && newStatus == Status::done)    ||
        (oldStatus == Status::done    && newStatus == Status::idle)) {
      currentStatus = newStatus;
      return;
    }

    // Invalid transition => error
	currentStatus = Status::error;
  }

  Status getCurrentStatus() const { return currentStatus; }

private:
  Status currentStatus;
};

#endif
