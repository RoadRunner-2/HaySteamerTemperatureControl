#define ARDUINO 1

#include <StatusLED.h>
#include <Communication.h>
#include <Relay.h>
#include <PushButton.h>
#include <Arduino.h>

#include "StateMachine.h"
#include "DiagramDataSet.h"
#include "Parameter.h"
#include "TempProbe.h"

#include "Keypad.h"
#include "Display.h"


Communication com;
NTP_Time clk;

Parameter param;
TempProbe temp(5, 7, 4, 5, 8, 4);
PushButton start_button(2);
Keypad keypad(3, 0x20);
Relay relay(12);
StatusLED led(9, 10, 11);
Display display;

HaySteamerStateMachine fsm;


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // connects to WLAN and sets up the NTP Clock
  com.setup_communication();
  display.setup_display();
  keypad.setup_keypad(keyChanged);
  start_button.setup_push_button();
  relay.setup_relay();
  // initializes the status LED
  led.setup_status_led();

  display.update_line_1(param.hay_steaming_status);
  display.update_current_time();
  temp.update_temp();
  display.update_line_2(temp.read1(), temp.read2());
  display.update_line_3(param.hay_steaming_status);
  display.update_line_4(convert_to_hour(param.start_time), convert_to_minutes(param.start_time), param.minimum_temperature, param.wait_time, InputMode::none);
}

void loop() {
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

  if (was_button_pressed)
  {
    fsm.start_button_pressed();
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
        if (clk.get_time_of_day_minutes() >= param.start_time) 
        {
          fsm.changeStatus(Status::heating);
        }
        break;
      case Status::heating:
        if (temp.read_min_temp() >= param.minimum_temperature) 
        {
          fsm.changeStatus(Status::holding_temperature);
        }
        if (clk.get_time_of_day_minutes() - param.actual_start_time > param.heating_timeout)
        {
          fsm.changeStatus(Status::error);
        }
        break;
      case Status::holding_temperature:
        if (clk.get_time_of_day_minutes() - param.reached_minimum_temperature >= param.wait_time) 
        {
          fsm.changeStatus(Status::done);
        }
        if (temp.read_min_temp() < param.minimum_temperature - param.holding_temperature_drop)
        {
          fsm.changeStatus(Status::error);
        }
        break;
      case Status::done:
        // signal done for an hour, the go back to idle
        if (clk.get_time_of_day_minutes() - param.time_when_done >= 1) 
        {
          fsm.changeStatus(Status::idle);
        }
        break;
      case Status::error:
        relay.switch_off();
        break;
    }

    param.print_status();
    display.draw(param.hay_steaming_status);
    led.signal_status(param.hay_steaming_status);
  }
}

void keyChanged() // IRQ
{
  keypad.key_changed();
}

void set_state_machine_callbacks() 
{
  fsm.registerCallback(Status::error, Status::idle, [](Status from, Status to) 
  {
    sprintf(param.error_message, ""); // clear error message

    display.update_line_3(Status::idle);
    display.draw(Status::idle);

    led.signal_status(Status::idle);

    Serial.println("Callback: Transition from error to idle.");
  });

  fsm.registerCallback(Status::idle, Status::ready, [](Status from, Status to) 
  {
    display.update_line_3(Status::ready);
    display.draw(Status::ready);

    led.signal_status(Status::ready);

    Serial.println("Callback: Transition from idle to ready.");
  });

  fsm.registerCallback(Status::ready, Status::heating, [](Status from, Status to) 
  {
    relay.switch_on();

    param.actual_start_time = clk.get_time_of_day_minutes();

    display.update_line_1(Status::heating, param.actual_start_time);
    display.update_line_3(Status::heating);
    Serial.println("Callback: Transition from ready to heating.");
  });

  fsm.registerCallback(Status::heating, Status::holding_temperature, [](Status from, Status to) 
  {
    param.reached_minimum_temperature = clk.get_time_of_day_minutes();

    Serial.println("Callback: Transition from heating to holding_temperature.");
  });

  fsm.registerCallback(Status::holding_temperature, Status::done, [](Status from, Status to) 
  {
    relay.switch_off();
    
    param.time_when_done = clk.get_time_of_day_minutes();

    display.update_line_1(Status::done, param.time_when_done);
    display.update_line_3(Status::done);

    Serial.println("Callback: Error occurred transitioning from holding_temperature to done!");
  });

  fsm.registerCallback(Status::done, Status::idle, [](Status from, Status to) 
  {
    display.update_line_1(Status::idle);
    display.update_line_3(Status::idle);

    Serial.println("Callback: Transition from Done to Idle.");
  });

  fsm.registerCallback(Status::heating, Status::error, [](Status from, Status to) 
  {
    relay.switch_off();

    sprintf(param.error_message, "timeout after %d min", param.heating_timeout);

    display.update_line_1(Status::error, param.actual_start_time);
    display.update_line_3(Status::error, param.error_message);

    Serial.println("Callback: Transition from heating to error.");
  });

  fsm.registerCallback(Status::holding_temperature, Status::error, [](Status from, Status to) 
  {
    relay.switch_off();
          
    sprintf(param.error_message, "temp drop > %d deg", param.holding_temperature_drop);

    display.update_line_1(Status::error, param.actual_start_time);
    display.update_line_3(Status::error, param.error_message);

    Serial.println("Callback: Transition from holding_temperature to error.");
  });
}
