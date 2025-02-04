#ifndef SignalLED_h
#define SignalLED_h

class SignalLED
{
  public:
    SignalLED(const int& red_in, const int& green_in, const int& blue_in)
      : pin_red(red_in)
      , pin_green(green_in)
      , pin_blue(blue_in)
    {
    
    };

    void setup_signal_led()
    {
      pinMode(pin_red,   OUTPUT);
      pinMode(pin_green, OUTPUT);
      pinMode(pin_blue,  OUTPUT);
    };

    void status_led(Status state)
    {
      switch (state) {
        case Status::ready:
          analogWrite(pin_red,   0);
          analogWrite(pin_green, 51);
          analogWrite(pin_blue,  153);        
          break;
        case Status::heating:
          analogWrite(pin_red,   0);
          analogWrite(pin_green, 102);
          analogWrite(pin_blue,  102);        
          break;
        case Status::holding_temperature:
          analogWrite(pin_red,   51);
          analogWrite(pin_green, 153);
          analogWrite(pin_blue,  51);
          break;
        case Status::done:
          analogWrite(pin_red,   51);
          analogWrite(pin_green, 255);
          analogWrite(pin_blue,  25);
          break;
        case Status::error:
          analogWrite(pin_red,   255);
          analogWrite(pin_green, 0);
          analogWrite(pin_blue,  0);
          break;
        default:
          analogWrite(pin_red,   0);
          analogWrite(pin_green, 0);
          analogWrite(pin_blue,  0);
      }
    };

  private:
    const int pin_red;
    const int pin_green;
    const int pin_blue;
};
#endif