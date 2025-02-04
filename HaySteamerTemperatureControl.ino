#include "Connect_Wlan.h"
#include "NPTClock.h"
#include "DiagramDataSet.h"
#include "Parameter.h"
#include "TempProbe.h"
#include "PushButton.h"
#include "Relay.h"
#include "SignalLED.h"
#include "Keypad.h"
#include "Display.h"
#include <Arduino.h>

Wlan_Connection wlan;
Parameter param;
TempProbe temp(5, 7, 4, 5, 8, 4);
PushButton push_button(2);
Keypad keypad(3, 0x20);
Relay relay(12);
SignalLED led(9, 10, 11);
Display display;


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  display.setup_display();
  keypad.setup_keypad(keyChanged);
  push_button.setup_push_button(push_button_is_pressed);
  relay.setup_relay();
  led.setup_signal_led();
  wlan.setup_wlan_connection();
  setup_npt_clock();

  display.update_line_1(param.hay_steaming_status);
  display.update_current_time();
  temp.update_temp();
  display.update_line_2(temp.read1(), temp.read2());
  display.update_line_3(param.hay_steaming_status);
  display.update_line_4(convert_to_hour(param.start_time), convert_to_minutes(param.start_time), param.minimum_temperature, param.wait_time, InputMode::none);
}

void loop() {

  if (timeStatus() == timeNotSet) {
      return;
  }

  // handle keypad interrupts
  if (keypad.is_key_pending())
  {
    // get active input mode, then process key stroke accordingly
    InputMode mode = keypad.get_input_mode();
    
    switch (mode) 
    {
      case InputMode::none:
        mode = keypad.process_key_pending_none();
        if (mode == InputMode::start_hour){
          keypad.set_start_time(param.start_time);
        }
      break;
      case InputMode::start_hour:
        mode = keypad.process_key_pending_start_hour();
      break;
      case InputMode::start_minute:
        mode = keypad.process_key_pending_start_minute();
        if (mode == InputMode::none)
        {
          Serial.print("hour: ");
          Serial.print(keypad.get_start_hour());
          Serial.print(" minute: ");
          Serial.println(keypad.get_start_minute());
          param.start_time = keypad.get_start_hour() * 60 + keypad.get_start_minute();
        }
      break;
      case InputMode::min_temp:

      break;
      case InputMode::min_time:

      break;
      default:
      break;
    }
    
  }

  if ((millis() % 500) == 250)
  {
    InputMode mode = keypad.get_input_mode();
    Serial.print("95 mode ");
    Serial.println(mode);
     if (mode == InputMode::start_hour)
    {
      display.update_line_4_hour(keypad.get_start_hour(), convert_to_minutes(param.start_time), param.minimum_temperature, param.wait_time);
      display.draw(param.hay_steaming_status);
    }
    else if (mode == InputMode::start_minute)
    {
      display.update_line_4_minute(keypad.get_start_hour(), keypad.get_start_minute(), param.minimum_temperature, param.wait_time);
      display.draw(param.hay_steaming_status);
    }
    else if (mode == InputMode::min_temp)
    {

    }
    else if (mode == InputMode::min_time)
    {

    }
    else
    {
      display.update_line_4(convert_to_hour(param.start_time), convert_to_minutes(param.start_time), param.minimum_temperature, param.wait_time, InputMode::none);
      display.draw(param.hay_steaming_status);
    }
  }
 

  if (push_button.is_state_ready_pending()) 
  {
    param.hay_steaming_status = Status::ready;
    display.update_line_3(param.hay_steaming_status);
    display.draw(param.hay_steaming_status);
    led.status_led(param.hay_steaming_status);
  }
  else if (push_button.is_reset_pending()) 
  {
    sprintf(param.error_message, ""); // clear error message
    param.hay_steaming_status = Status::idle; // reset state
    display.update_line_3(param.hay_steaming_status);
    display.draw(param.hay_steaming_status);
    led.status_led(param.hay_steaming_status);
  }
  
  if ((millis() % 5000) == 0)
  {
    // read & update temperature
    temp.update_temp();
    display.update_line_2(temp.read1(), temp.read2());
    // update current time
    display.update_current_time();

    switch (param.hay_steaming_status) {
      case Status::ready:
        if (time_of_day_in_minutes() >= param.start_time) 
        {
          relay.switch_on();
          param.hay_steaming_status = Status::heating;
          param.actual_start_time = time_of_day_in_minutes();
          display.update_line_1(param.hay_steaming_status, param.actual_start_time);
          display.update_line_3(param.hay_steaming_status);
        }
        break;
      case Status::heating:
        if (temp.read_min_temp() >= param.minimum_temperature) 
        {
          param.reached_minimum_temperature = time_of_day_in_minutes();
          param.hay_steaming_status = Status::holding_temperature;
        }
        if (time_of_day_in_minutes() - param.actual_start_time > param.heating_timeout)
        {
          relay.switch_off();
          sprintf(param.error_message, "timeout after %d min", param.heating_timeout);
          param.hay_steaming_status = Status::error;
          display.update_line_1(param.hay_steaming_status, param.actual_start_time);
          display.update_line_3(param.hay_steaming_status, param.error_message);
        }
        break;
      case Status::holding_temperature:
        if (time_of_day_in_minutes() - param.reached_minimum_temperature >= param.wait_time) 
        {
          relay.switch_off();
          param.time_when_done = time_of_day_in_minutes();
          param.hay_steaming_status = Status::done;
          display.update_line_1(param.hay_steaming_status, param.time_when_done);
          display.update_line_3(param.hay_steaming_status);
        }
        if (temp.read_min_temp() < param.minimum_temperature - param.holding_temperature_drop)
        {
          relay.switch_off();
          sprintf(param.error_message, "temp drop > %d deg", param.holding_temperature_drop);
          param.hay_steaming_status = Status::error;
          display.update_line_1(param.hay_steaming_status, param.actual_start_time);
          display.update_line_3(param.hay_steaming_status, param.error_message);
        }
        break;
      case Status::done:
        // signal done for an hour, the go back to idle
        if (time_of_day_in_minutes() - param.time_when_done >= 1) 
        {
          param.hay_steaming_status = Status::idle;
          display.update_line_1(param.hay_steaming_status);
          display.update_line_3(param.hay_steaming_status);
        }
        break;
      case Status::error:
        relay.switch_off();
        break;
    }

    param.print_status();
    display.draw(param.hay_steaming_status);
    led.status_led(param.hay_steaming_status);
  }
}

void push_button_is_pressed() // IRQ
{
  if (param.hay_steaming_status == Status::idle) {
    push_button.state_ready_pending();
  }
  else if (param.hay_steaming_status == Status::error) {
    push_button.reset_pending();
  }
}

void keyChanged() // IRQ
{
  keypad.key_changed();
}