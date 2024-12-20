#ifndef TempProbe_h
#define TempProbe_h

#include <max6675.h>


class TempProbe
{
  public:
  TempProbe(const int& sck_pin1, const int& cs_pin1, const int& so_pin1, const int& sck_pin2, const int& cs_pin2, const int& so_pin2)
    : thermocouple1(sck_pin1, cs_pin1, so_pin1)
    , thermocouple2(sck_pin2, cs_pin2, so_pin2)
  {};

  void update_temp()
  {
      temp1 = thermocouple1.readCelsius();
      temp2 = thermocouple2.readCelsius();

  };

  int read_min_temp()
  {
      Serial.print("1: C = ");
      Serial.println(temp1);

      Serial.print("2: C = "); 
      Serial.println(temp2);

      return min(temp1, temp2);
  };

  int read1()
  {
    return temp1;
  };

  int read2()
  {
    return temp2;
  };

  private:
    MAX6675 thermocouple1;
    int temp1 = 0;
    MAX6675 thermocouple2;
    int temp2 = 0;
};







#endif