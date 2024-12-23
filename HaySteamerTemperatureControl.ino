#include "Connect_Wlan.h"
#include "NPTClock.h"
#include "DiagramDataSet.h"
#include "Parameter.h"
#include "TempProbe.h"
#include <Arduino.h>
#include <U8g2lib.h>

Wlan_Connection wlan;
Parameter param;
TempProbe temp(3, 7, 4, 3, 8, 4);
U8G2_SSD1309_128X64_NONAME2_1_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);

volatile bool ready_button_was_pressed = false;
volatile bool reset_button_was_pressed = false;

const int PIN_RED   = 9;
const int PIN_GREEN = 10;
const int PIN_BLUE  = 11;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  u8g2.begin(); 

  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), push_button_ISR, CHANGE);

  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);

  wlan.setup_wlan_connection();

  setup_npt_clock();
}

void loop() {

  if (timeStatus() == timeNotSet) {
      return;
  }
  
  // read input signals
  if ((millis() % 500) == 0)
  {
    temp.update_temp();
    if (ready_button_was_pressed) {
      ready_button_was_pressed = false;
      param.hay_steaming_status = Parameter::Status::ready;
    }
    if (reset_button_was_pressed) {
      reset_button_was_pressed = false;
      sprintf(param.error_message, ""); // clear error message
      param.hay_steaming_status = Parameter::Status::idle; // reset state
    }
  }

  if ((millis() % 5000) == 0)
  {
    switch (param.hay_steaming_status) {
      case Parameter::Status::ready:
        if (time_of_day_in_minutes() >= param.start_time) 
        {
          // switch_relay(on);
          param.hay_steaming_status = Parameter::Status::heating;
          param.actual_start_time = time_of_day_in_minutes();
        }
        break;
      case Parameter::Status::heating:
        if (temp.read_min_temp() >= param.minimum_temperature) 
        {
          param.reached_minimum_temperature = time_of_day_in_minutes();
          param.hay_steaming_status = Parameter::Status::holding_temperature;
        }
        if (time_of_day_in_minutes() - param.actual_start_time > param.heating_timeout)
        {
          // switch_relay(off);
          sprintf(param.error_message, "timeout after %d min", param.heating_timeout);
          param.hay_steaming_status = Parameter::Status::error;
        }
        break;
      case Parameter::Status::holding_temperature:
        if (time_of_day_in_minutes() - param.reached_minimum_temperature >= param.wait_time) 
        {
          // switch_relay(off);
          param.time_when_done = time_of_day_in_minutes();
          param.hay_steaming_status = Parameter::Status::done;
        }
        if (temp.read_min_temp() < param.minimum_temperature - param.holding_temperature_drop)
        {
          // switch_relay(off);
          sprintf(param.error_message, "temp drop > %d deg", param.holding_temperature_drop);
          param.hay_steaming_status = Parameter::Status::error;
        }
        break;
      case Parameter::Status::done:
        // signal done for an hour, the go back to idle
        if (time_of_day_in_minutes() - param.time_when_done >= 1) 
        {
          param.hay_steaming_status = Parameter::Status::idle;
        }
        break;
      case Parameter::Status::error:
        // switch_relay(off);
        break;
    }
    param.print_status();
  }

  // write output signals
  if ((millis() % 500) == 0)
  {
    update_display(param, temp.read1(), temp.read2());
    status_led(param.hay_steaming_status);
  }
 
}

void update_display(Parameter param, int temp1, int temp2)
{
  u8g2.firstPage();
  do {
    char line1_curr_time[25];
    sprintf(line1_curr_time, "%02d:%02d", hour(), minute());

    char line1_heating[25];
    int actual_start_time_min = (param.actual_start_time) % 60;
    int actual_start_time_hour = (param.actual_start_time - actual_start_time_min) / 60;
    sprintf(line1_heating, "started: %02d:%02d", actual_start_time_hour, actual_start_time_min);

    char line1_holding_temperature[25];
    int done_min = (param.reached_minimum_temperature + param.wait_time) % 60;
    int done_hour = (param.reached_minimum_temperature + param.wait_time - done_min) / 60;
    int remaining_minutes = (done_hour * 60 + done_min) - time_of_day_in_minutes();
    sprintf(line1_holding_temperature, "done: %02d:%02d (%dmin)", done_hour, done_min, remaining_minutes);

    char line1_done[25];
    sprintf(line1_done, "done: %02d:%02d", done_hour, done_min);

    u8g2.setFont(u8g2_font_6x12_tf);
    if (param.hay_steaming_status == Parameter::Status::heating) {
      u8g2.drawStr(0, 13, line1_heating);
      u8g2.drawStr(90, 13, line1_curr_time);
    }
    else if (param.hay_steaming_status == Parameter::Status::holding_temperature) {
      u8g2.drawStr(0, 13, line1_holding_temperature);
    }
    else if (param.hay_steaming_status == Parameter::Status::done) {
      u8g2.drawStr(0, 13, line1_done);
      u8g2.drawStr(90, 13, line1_curr_time);
    }
    else {
      u8g2.drawStr(90, 13, line1_curr_time);
    }

    char line2[25];
    sprintf(line2, "%3d°C %3d°C", temp1, temp2);

    u8g2.setFont(u8g2_font_10x20_tf);
    u8g2.drawUTF8(0,30,line2);


    if (param.hay_steaming_status == Parameter::Status::error)
    {
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.drawUTF8(0,47,param.error_message);
    }
    else
    {
      char line3[25];
      if (param.hay_steaming_status == Parameter::Status::idle) {
        sprintf(line3, "IDLE");
      }
      else if (param.hay_steaming_status == Parameter::Status::ready) {
        sprintf(line3, "READY");
      }
      else if (param.hay_steaming_status == Parameter::Status::heating | param.hay_steaming_status == Parameter::Status::holding_temperature) {
        sprintf(line3, "ACTIVE");
      }
      else if (param.hay_steaming_status == Parameter::Status::done) {
        sprintf(line3, "DONE");
      }
      u8g2.drawUTF8(30,47,line3);
    }
    
    char line4[25];
    int start_time_min = param.start_time % 60;
    int start_time_hour = (param.start_time - start_time_min) / 60;
    sprintf(line4, "(%02d:%02d, %d°C, %dmin)", start_time_hour, start_time_min, param.minimum_temperature, param.wait_time);

    u8g2.setFont(u8g2_font_6x12_tf);
    u8g2.drawUTF8(0,60,line4);
    
  } while ( u8g2.nextPage() );
}

void status_led(const Parameter::Status& state)
{
  switch (param.hay_steaming_status) {
      case Parameter::Status::ready:
        analogWrite(PIN_RED,   0);
        analogWrite(PIN_GREEN, 51);
        analogWrite(PIN_BLUE,  153);        
        break;
      case Parameter::Status::heating:
        analogWrite(PIN_RED,   0);
        analogWrite(PIN_GREEN, 102);
        analogWrite(PIN_BLUE,  102);        
        break;
      case Parameter::Status::holding_temperature:
        analogWrite(PIN_RED,   51);
        analogWrite(PIN_GREEN, 153);
        analogWrite(PIN_BLUE,  51);
        break;
      case Parameter::Status::done:
        analogWrite(PIN_RED,   51);
        analogWrite(PIN_GREEN, 201);
        analogWrite(PIN_BLUE,  51);
        break;
      case Parameter::Status::error:
        analogWrite(PIN_RED,   255);
        analogWrite(PIN_GREEN, 0);
        analogWrite(PIN_BLUE,  0);
        break;
      default:
        analogWrite(PIN_RED,   0);
        analogWrite(PIN_GREEN, 0);
        analogWrite(PIN_BLUE,  0);
    }
}

void push_button_ISR()
{
  if (param.hay_steaming_status == Parameter::Status::idle) {
    ready_button_was_pressed = true;
  }
  else if (param.hay_steaming_status == Parameter::Status::error) {
    reset_button_was_pressed = true;
  }
}