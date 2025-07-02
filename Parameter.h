#ifndef Parameter_h
#define Parameter_h

#include "Services.h"

class Parameter{
  public:
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
      else if (hay_steaming_status == done) {
        Serial.println("Hay Steamer Status = DONE");
      }
      else if (hay_steaming_status == error) {
        Serial.println("Hay Steamer Status = ERROR");
      }
      return;
    };

    Parameter(){};

    // parameter set for the steaming process
    unsigned int start_time = 600; // daily start time in minutes
    int minimum_temperature = 30; // minimum temperature in °C
    unsigned int wait_time = 1; // wait time after minimum temperature was reached in minutes

    // status/progress of the steaming process
    volatile Status hay_steaming_status = idle;
    unsigned int actual_start_time = 0; // time when the heating actually started
    unsigned int reached_minimum_temperature = 0; // time of day when the minimum temperature was reached, in minutes
    unsigned int time_when_done = 0; // time in minutes when the steaming process was done

    // timeouts (will cause an error)
    int heating_timeout = 2; // minimum temperature has to have been reached after this time
    int holding_temperature_drop = 5; // temperature cannot drop more than this amount after the minimum temperature was reached
    char error_message[20]; 
};

#endif