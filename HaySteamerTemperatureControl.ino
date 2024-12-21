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

volatile bool push_button_was_pressed = false;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  u8g2.begin(); 

  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), push_button_ISR, CHANGE);

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
    if (push_button_was_pressed) {
      push_button_was_pressed = false;
      param.hay_steaming_status = Parameter::Status::ready;
    }
  }

  if ((millis() % 5000) == 0)
  {
    switch (param.hay_steaming_status) {
      case Parameter::Status::ready:
        if (time_of_day_in_minutes() >= param.start_time) {
          // switch_relay(on);
          param.hay_steaming_status = Parameter::Status::heating;
        }
        break;
      case Parameter::Status::heating:
        if (temp.read_min_temp() >= param.minimum_temperature) {
            param.reached_minimum_temperature = time_of_day_in_minutes();
            param.hay_steaming_status = Parameter::Status::holding_temperature;
          }
        break;
      case Parameter::Status::holding_temperature:
        if (time_of_day_in_minutes() - param.reached_minimum_temperature >= param.wait_time) {
          // switch_relay(off);
          param.time_when_done = time_of_day_in_minutes();
          param.hay_steaming_status = Parameter::Status::done;
        }
        break;
      case Parameter::Status::done:
        // signal done for an hour, the go back to idle
        if (time_of_day_in_minutes() - param.time_when_done >= 60) {
          param.hay_steaming_status = Parameter::Status::idle;
        }
        break;
    }
    param.print_status();
  }

  // write output signals
  if ((millis() % 500) == 0)
  {
    update_display(param, temp.read1(), temp.read2());
  }
 
}

void update_display(Parameter param, int temp1, int temp2)
{
  u8g2.firstPage();
  do {
    char line1[25];
    sprintf(line1, "%02d:%02d", hour(), minute());

    char line1_active[25];
    int done_min = (param.reached_minimum_temperature + param.wait_time) % 60;
    int done_hour = (param.reached_minimum_temperature + param.wait_time - done_min) / 60;
    int remaining_minutes = (done_hour * 60 + done_min) - time_of_day_in_minutes();
    sprintf(line1_active, "done: %02d:%02d (%dmin)", done_hour, done_min, remaining_minutes);

    u8g2.setFont(u8g2_font_6x12_tf);
    if (param.hay_steaming_status != Parameter::Status::holding_temperature) {
      u8g2.drawStr(90, 13, line1);
    }
    else {
      u8g2.drawStr(0, 13, line1_active);
    }

    char line2[25];
    sprintf(line2, "%3d°C %3d°C", temp1, temp2);

    u8g2.setFont(u8g2_font_10x20_tf);
    u8g2.drawUTF8(0,30,line2);

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

    char line4[25];
    int start_time_min = param.start_time % 60;
    int start_time_hour = (param.start_time - start_time_min) / 60;
    sprintf(line4, "(%02d:%02d, %d°C, %dmin)", start_time_hour, start_time_min, param.minimum_temperature, param.wait_time);

    u8g2.setFont(u8g2_font_6x12_tf);
    u8g2.drawUTF8(0,60,line4);
    
  } while ( u8g2.nextPage() );
}

void push_button_ISR()
{
  if (param.hay_steaming_status == Parameter::Status::idle) {
    push_button_was_pressed = true;
  }
}