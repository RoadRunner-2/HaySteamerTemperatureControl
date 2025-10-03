#ifndef Display_h
#define Display_h

#include <U8g2lib.h>
#include <Actor.h>
#include <Arduino.h>
#include <String.h>
#include <Wire.h>

class Display : public Actor<String[4]>
{
  public:
  Display(int reset_pin)
    : reset_pin(reset_pin)
    , u8g2(U8G2_R0, reset_pin)
  {};

  void setup() override
  {
    Wire.begin();

    pinMode(reset_pin, OUTPUT);
  
    reset();
    
    u8g2.setI2CAddress(0x3D * 2); 
    u8g2.begin();
    u8g2.clearDisplay();
  };

  void write(String input_value[4]) override
  {
    strncpy(line1, input_value[0].c_str(), 24);
    strncpy(line2, input_value[1].c_str(), 24);
    strncpy(line3, input_value[2].c_str(), 24);
    strncpy(line4, input_value[3].c_str(), 24);

    draw();
  }

private:
  void reset()
  {
    digitalWrite(reset_pin, LOW);
    delay(1);
    digitalWrite(reset_pin, HIGH);
    delay(50);
  }

  void draw()
  {
    u8g2.firstPage();
    do {
      // line 1
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.drawStr(0, 13, line1);

      // line 2
      u8g2.setFont(u8g2_font_10x20_tf);
      u8g2.drawUTF8(0,30,line2);

      // line 3
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.drawUTF8(0,47,line3);
        
      // line 4
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.drawUTF8(0,60,line4);
    } while (u8g2.nextPage());

  };

    int reset_pin;
    U8G2_SSD1309_128X64_NONAME0_1_HW_I2C u8g2;

    char line1[25];
    char line2[25];
    char line3[25];
    char line4[25];
};

#endif