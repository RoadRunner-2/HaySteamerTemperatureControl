#ifndef Parameter_h
#define Parameter_h

#include <TimeLib.h>

class Parameter{
  public:
    enum Status{
      idle = 0,
      ready,
      heating,
      holding_temperature,
      done
    };

    void print_status()
    {
      if (hay_steaming_status == idle) {
        Serial.println("Hay Steamer Status = IDLE");
      }
      else if (hay_steaming_status == ready) {
        Serial.println("Hay Steamer Status = READY");
      }
      else if (hay_steaming_status == heating) {
        Serial.println("Hay Steamer Status = HEATING");
      }
      else if (hay_steaming_status == holding_temperature) {
        Serial.println("Hay Steamer Status = HOLDING TEMPERATURE");
      }
      else if (hay_steaming_status == done) {
        Serial.println("Hay Steamer Status = DONE");
      }
      return;
    };

    Parameter(){};

    Status hay_steaming_status = idle;
    unsigned int start_time = 1368; // daily start time in minutes
    int minimum_temperature = 30; // minimum temperature in °C
    unsigned int reached_minimum_temperature = 0; // time of day when the minimum temperature was reached, in minutes
    unsigned int wait_time = 1; // wait time after minimum temperature was reached in minutes
    unsigned int time_when_done = 0; // time in minutes when the steaming process was done
};

#endif