#ifndef Connect_Wlan_h
#define Connect_Wlan_h

#include <WiFiS3.h>

class Wlan_Connection{
  friend class Communication;
  protected:
    Wlan_Connection();

    void setup_wlan_connection();

    void printWifiData();
    void printCurrentNet();
    void printMacAddress(byte mac[]);

  private:
    int status = WL_IDLE_STATUS; // WiFi status
};
#endif