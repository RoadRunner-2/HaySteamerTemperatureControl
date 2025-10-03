#ifndef TempProbe_h
#define TempProbe_h

#include <max6675.h>
#include "Sensor.h"

class TempProbe : public Sensor<int>
{
  public:
  TempProbe(const int& sck_pin1, const int& cs_pin1, const int& so_pin1, const int& sck_pin2, const int& cs_pin2, const int& so_pin2)
    : thermocouple1(sck_pin1, cs_pin1, so_pin1)
    , thermocouple2(sck_pin2, cs_pin2, so_pin2)
  {};

  int read() override
  {
      temp1 = thermocouple1.readCelsius();
      temp2 = thermocouple2.readCelsius();
      return min(temp1, temp2);
  };

  private:
    MAX6675 thermocouple1;
    int temp1 = 0;
    MAX6675 thermocouple2;
    int temp2 = 0;
};

#endif