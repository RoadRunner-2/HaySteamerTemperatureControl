#ifndef Communication_h
#define Communication_h

#include "Connect_Wlan.h"
#include <I2C_RTC.h>
#include <Sensor.h>

class Communication
{
  public:
    Communication();

    // call once in setup()
    void setup_communication();

  private:
    Wlan_Connection wlan;
};

class NTP_Time : public Sensor<time_t>
{
  public:
  time_t read() override;

  void setup_clock();

  private:
    // call cyclically in loop()
    bool update_time();

    time_t time;
    DS3231 rtc;
};
#endif