#ifndef StatusLED_h
#define StatusLED_h

#include <Status.h>
#include <Actor.h>

class StatusLED: public Actor<Status>
{
  public:
    StatusLED(const int& red_in, const int& green_in, const int& blue_in)
      : pin_red(red_in)
      , pin_green(green_in)
      , pin_blue(blue_in)
    {
    
    };

    void setup() override;

    void write(Status state) override;

  private:
    const int pin_red;
    const int pin_green;
    const int pin_blue;
};
#endif