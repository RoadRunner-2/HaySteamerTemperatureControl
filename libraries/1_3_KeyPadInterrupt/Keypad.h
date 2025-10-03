#include "r_keymatrix_api.h"
#include "api/Common.h"
#ifndef Keypad_h
#define Keypad_h

#include <Wire.h>
#include <I2CKeyPad.h>
#include <Sensor.h>

class Keypad : public Sensor<char>
{
  public:
    Keypad(const int& pin, const uint8_t& deviceAddress) 
      : pin_input(pin)
      , i2c_keypad(deviceAddress)
      , keys("123A456B789C*0#DNF")
    { };

    void setup_keypad(voidFuncPtr keyChanged_ISR)
    {
      pinMode(pin_input, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(pin_input), keyChanged_ISR, FALLING);
      Serial.println("\nSetup\n");

      if (i2c_keypad.begin() == false)
      {
        Serial.println("\nERROR: cannot communicate to keypad.\nPlease reboot.\n");
      }
      i2c_keypad.loadKeyMap(keys);
      i2c_keypad.setDebounceThreshold(10);
    };

    char read() override
    {
      uint8_t index = i2c_keypad.getKey();
      return keys[index];
    };

  private:
    const int pin_input;

    I2CKeyPad i2c_keypad;
    char keys[19];
};

#endif