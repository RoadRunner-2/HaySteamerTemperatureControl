#ifndef StateMachine_h
#define StateMachine_h

#include <Arduino.h>
#include <functional>
#include <map>
#include <Status.h>

class HaySteamerStateMachine {
public:
  // A callback type that receives the "from" Status and "to" Status.
  using Callback = std::function<void(Status, Status)>;

  // Constructor: initialize with the default Status (Idle).
  HaySteamerStateMachine() : currentStatus(Status::idle) {}

  // Register a callback for a specific Status transition.
  void registerCallback(Status from, Status to, Callback cb) 
  {
    TransitionKey key { from, to };
    transitions[key] = cb;
  }

  // Change the Status of the state machine.
  void changeStatus(Status newStatus) 
  {
    Status oldStatus = currentStatus;
    if (oldStatus == newStatus) return; // No Status change

    currentStatus = newStatus;
    // Look-up the registered callback for this Status transition, if any.
    TransitionKey key { oldStatus, newStatus };
    auto it = transitions.find(key);
    if (it != transitions.end()) {
      // Call the callback, passing the old and new Status.
      it->second(oldStatus, newStatus);
    }
  }

  // This method is meant to be called whenever the push button is pressed.
  void start_button_pressed() 
  {
    // Determine the next state based on the current state.
    switch (currentStatus) {
      case Status::idle:
        changeStatus(Status::ready);
        break;
      case Status::error:
        changeStatus(Status::idle);
        break;
    }
  }

  // Retrieve the current Status of the state machine.
  Status getCurrentStatus() const { return currentStatus; }

private:
  // Helper struct used as a key in the transition callback map.
  struct TransitionKey {
    Status from;
    Status to;

    // Operator for ordering keys in std::map.
    bool operator<(const TransitionKey &other) const {
      if (static_cast<int>(from) != static_cast<int>(other.from))
        return static_cast<int>(from) < static_cast<int>(other.from);
      return static_cast<int>(to) < static_cast<int>(other.to);
    }
  };

  // Map to hold callback functions for specific state transitions.
  std::map<TransitionKey, Callback> transitions;
    
  // The current Status of the finite state machine.
  Status currentStatus;
};

#endif
