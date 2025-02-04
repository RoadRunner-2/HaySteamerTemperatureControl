#ifndef Relay_h
#define Relay_h

class Relay
{
  public:
    Relay(const int& pin)
      :pin_output(pin)
    {

    };

    void setup_relay()
    {
      pinMode(pin_output, OUTPUT);
      digitalWrite(pin_output, relayState);
    };

    void switch_on()
    {
        relayState = HIGH;
        digitalWrite(pin_output, relayState);
    };

    void switch_off()
    {
      relayState = LOW;
      digitalWrite(pin_output, relayState);
    };

    bool is_relay_on()
    {
      return (relayState == HIGH);
    };

  private:
  const int pin_output;
  volatile byte relayState = LOW;
};
#endif