#ifndef StateMachine_h
#define StateMachine_h


#ifdef SANDBOX_ENVIRONMENT
#include <string> // Include the standard string library
using String = std::string; // Define "String" as an alias for std::string
// Define toString(int) as std::to_string
using Fptr = std::string(*)(int);
constexpr Fptr toString = &std::to_string;

#include "Sandbox/Status.h" 
#include <map>
#include <functional>
#endif

#ifdef ARDUINO
// Define toString(int) as String(int)
using Fptr = std::string(*)(int);
constexpr Fptr toString = String;

#include <Arduino.h>
#include <functional>
#include <map>
#include <Status.h>
#endif

class HaySteamerStateMachine {
public:
  using Callback = std::function<void(Status, Status)>;

  HaySteamerStateMachine() 
      : currentStatus(Status::idle) 
  {
    // Register all valid transitions with dummy callbacks
    registerCallback(Status::idle,    Status::ready,   emptyTransitionFunction);
    registerCallback(Status::ready,   Status::heating, emptyTransitionFunction);
    registerCallback(Status::heating, Status::holding, emptyTransitionFunction);
    registerCallback(Status::holding, Status::done,    emptyTransitionFunction);
    // error transitions
    registerCallback(Status::idle,    Status::error,   emptyTransitionFunction);
    registerCallback(Status::ready,   Status::error,   emptyTransitionFunction);
    registerCallback(Status::heating, Status::error,   emptyTransitionFunction);
    registerCallback(Status::holding, Status::error,   emptyTransitionFunction);
    registerCallback(Status::done,    Status::error,   emptyTransitionFunction);
    registerCallback(Status::error,   Status::idle,    emptyTransitionFunction);
  }

  void registerCallback(Status from, Status to, Callback cb) {
    TransitionKey key{ from, to };
    transitions[key] = cb;
  }

  void changeStatus(Status newStatus) {
    Status oldStatus = currentStatus;
    if (oldStatus == newStatus) return; // No Status change

    TransitionKey key{ oldStatus, newStatus };
    auto it = transitions.find(key);
    if (it == transitions.end()) {
      // Invalid transition, do nothing
      return;
    }
    currentStatus = newStatus;
    it->second(oldStatus, newStatus);
  }

  Status getCurrentStatus() const { return currentStatus; }

private:
  struct TransitionKey {
    Status from;
    Status to;
    bool operator<(const TransitionKey& other) const {
      if (static_cast<int>(from) != static_cast<int>(other.from))
        return static_cast<int>(from) < static_cast<int>(other.from);
      return static_cast<int>(to) < static_cast<int>(other.to);
    }
  };

  static void emptyTransitionFunction(Status /*from*/, Status /*to*/) {}

  std::map<TransitionKey, Callback> transitions;
  Status currentStatus;
};

#endif
