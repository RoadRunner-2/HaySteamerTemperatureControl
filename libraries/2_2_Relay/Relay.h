#ifndef Relay_h
#define Relay_h

#include <Actor.h>

class Relay: public Actor<byte>
{
  public:
    Relay(const int& pin)
      :pin_output(pin)
    { };

    void setup() override
    {
      pinMode(pin_output, OUTPUT);
      digitalWrite(pin_output, relayState);
    };

    void write(byte relay_state) override
    {
        relayState = relay_state;
        digitalWrite(pin_output, relayState);
    };

    bool is_relay_on()
    {
      return (digitalRead(pin_output) == HIGH);
    };

  private:
    const int pin_output;
    volatile byte relayState = LOW;
};
#endif