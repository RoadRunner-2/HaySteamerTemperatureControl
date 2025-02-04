#include "r_keymatrix_api.h"
#include "api/Common.h"
#ifndef Keypad_h
#define Keypad_h

#include "Services.h"
#include <Wire.h>
#include <I2CKeyPad.h>

class Keypad 
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
      attachInterrupt(digitalPinToInterrupt(pin_input), keyChanged_ISR, CHANGE);
      mode = InputMode::none;

      if (i2c_keypad.begin() == false)
      {
        Serial.println("\nERROR: cannot communicate to keypad.\nPlease reboot.\n");
      }
      i2c_keypad.loadKeyMap(keys);
    };

    InputMode process_key_pending_none()
    {
      uint8_t index = i2c_keypad.getKey();
      
      //  only after keyChange is handled it is time reset the flag
      key_change_pending = false;
      if (index == 16) {
        return mode;
      }

      Serial.print("press: ");
      Serial.println(index);
      
      if (index == 3){
        digit_position = 1;
        mode = InputMode::start_hour;
      }
      else if (index == 7){
        digit_position = 2;
        mode = InputMode::min_temp;
      }
      else if (index == 11) {
        digit_position = 2;
        mode = InputMode::min_time;
      }

      return mode;
    };

    InputMode process_key_pending_start_hour()
    {
      uint8_t index = i2c_keypad.getKey();
      
      //  only after keyChange is handled it is time reset the flag
      key_change_pending = false;
      if (index == 3 || index == 7 || index == 11 || index == 12 || index >= 14) {
        return mode;
      }
      
      Serial.print("press: ");
      Serial.println(index);

      if (digit_position == 1)
      {
        new_start_hour = 10 * digits[index];
        digit_position--;
      }
      else if (digit_position == 0)
      {
        new_start_hour = new_start_hour + digits[index];
        digit_position = 1;
        mode = InputMode::start_minute;
      }
      
      Serial.print("87 mode: ");
      Serial.println(mode);
      return mode;
    };

    InputMode process_key_pending_start_minute()
    {
      uint8_t index = i2c_keypad.getKey();
      
      //  only after keyChange is handled it is time reset the flag
      key_change_pending = false;
      if (index == 3 || index == 7 || index == 11 || index == 12 || index >= 14) {
        return mode;
      }
      
      Serial.print("press: ");
      Serial.println(index);
      
      if (digit_position == 1)
      {
        new_start_minute = 10 * digits[index];
        digit_position--;
      }
      else if (digit_position == 0)
      {
        new_start_minute = new_start_minute + digits[index];
        digit_position = 1;
        mode = InputMode::none;
      }

      return mode;
    };

    void key_changed()
    {
      key_change_pending = true;
    };

    bool is_key_pending()
    {
      return key_change_pending;
    };

    InputMode get_input_mode()
    {
      return mode;
    };

    void set_start_time(const int& start_time)
    {
      new_start_hour = convert_to_hour(start_time);
      new_start_minute = convert_to_minutes(start_time);
    }

    int get_start_hour()
    {
      return new_start_hour;
    };

    int get_start_minute()
    {
      return new_start_minute;
    };

  private:
    const int pin_input;
    volatile bool key_change_pending = false;

    InputMode mode;
    int new_start_hour;
    int new_start_minute;
    int new_min_temp;
    int new_min_time;
    int digit_position;

    I2CKeyPad i2c_keypad;
    char keys[19];
    int digits[19] = {1, 2, 3, 0, 4, 5, 6, 0, 7, 8, 9, 0, 0, 0, 0, 0 , 0, 0, 0};
};

#endif