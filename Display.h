#ifndef Display_h
#define Display_h

#include "Services.h"
#include <U8g2lib.h>

class Display
{
  public:
  Display()
    : u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE)
  {};

  void setup_display()
  {
    u8g2.begin();
  };

  void update_current_time()
  {
    sprintf(current_time, "%02d:%02d", hour(), minute());
  };

  void update_line_1(Status state, const int& time = 0)
  {
    if (state == Status::heating) 
    {
      sprintf(line1, "started: %02d:%02d", convert_to_hour(time), convert_to_minutes(time));
    }
    else if (state == Status::holding_temperature || state == Status::done) 
    {
      sprintf(line1, "done: %02d:%02d", convert_to_hour(time), convert_to_minutes(time));
    }
    else
    {
      sprintf(line1,"");
    }
  };

  void update_line_2(const int& temp1, const int& temp2)
  {
    sprintf(line2, "%3d°C %3d°C", temp1, temp2);
  };

  void update_line_3(Status state, char* error_message = 0)
  {
    if (state == Status::idle) {
      sprintf(line3, "IDLE");
    }
    else if (state == Status::ready) {
      sprintf(line3, "READY");
    }
    else if (state == Status::heating || state == Status::holding_temperature) {
      sprintf(line3, "ACTIVE");
    }
    else if (state == Status::done) {
      sprintf(line3, "DONE");
    }
    else if (state == Status::error){
      sprintf(line3, error_message);
    }
  };

  void update_line_4(const int& start_time_hour, const int& start_time_minute, const int& min_temp, const int& min_time, const InputMode& mode = InputMode::none)
  {
    if (mode == InputMode::none)
    {
      sprintf(line4, "%2d:%02d, %3d°C, %3dmin", start_time_hour, start_time_minute, min_temp, min_time);
    }
  };

  void update_line_4_hour(const int& start_time_hour, const int& start_time_minute, const int& min_temp, const int& min_time)
  {
    blinking = second() % 2;
    if (blinking){
      sprintf(line4, "%02d:%02d, %3d°C, %3dmin", start_time_hour, start_time_minute, min_temp, min_time);
    }
    else
    {
      sprintf(line4, "  :%02d, %3d°C, %3dmin", start_time_minute, min_temp, min_time);
    }
    Serial.print("92 start time hour ");
    Serial.println(start_time_hour);
  };

  void update_line_4_minute(const int& start_time_hour, const int& start_time_minute, const int& min_temp, const int& min_time)
  {
    blinking = second() % 2;
    if (blinking){
      sprintf(line4, "%02d:%02d, %3d°C, %3dmin", start_time_hour, start_time_minute, min_temp, min_time);
    }
    else
    {
      sprintf(line4, "%02d:  , %3d°C, %3dmin", start_time_hour, min_temp, min_time);
    }
    Serial.print("106 start time minute ");
    Serial.println(start_time_minute);
  };

  void draw(Status state)
  {
    u8g2.firstPage();
    do {
      // line 1
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.drawStr(0, 13, line1);
      u8g2.drawStr(95, 13, current_time);

      // line 2
      u8g2.setFont(u8g2_font_10x20_tf);
      u8g2.drawUTF8(0,30,line2);

      // line 3
      if (state == Status::error)
      {
        u8g2.setFont(u8g2_font_6x12_tf);
        u8g2.drawUTF8(0,47,line3);
      }
      else
      {
        u8g2.setFont(u8g2_font_10x20_tf);
        u8g2.drawUTF8(30,47,line3);
      }

      // line 4
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.drawUTF8(0,60,line4);

    } while ( u8g2.nextPage() );
  };


  private:
    U8G2_SSD1309_128X64_NONAME0_1_HW_I2C u8g2;

    // line 1
    char line1[25];
    char current_time[25];
    // line 2
    char line2[25];
    // line 3
    char line3[25];
    // line 4
    char line4[25];
    int blinking = 1;
};

#endif