#include "Communication.h"
#include "NTPClock.h"

Communication::Communication()
{}

void Communication::setup_communication()
{
  wlan.setup_wlan_connection();
}

void NTP_Time::setup_clock()
{
  rtc.begin();
  rtc.setHourMode(CLOCK_H12);
  setup_ntp_clock();
}

time_t NTP_Time::read()
{
  if (update_time()) {
    rtc.setEpoch(time);
    return time;
  }

  return rtc.getEpoch();
}

bool NTP_Time::update_time()
{
  if (timeStatus() == timeNotSet) 
  {
    return false;
  }

  time = getNtpTime();

  return true;
}
