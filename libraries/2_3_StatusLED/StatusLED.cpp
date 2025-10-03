#include "StatusLED.h"
#include <Arduino.h>

void StatusLED::setup()
{
  pinMode(pin_red,   OUTPUT);
  pinMode(pin_green, OUTPUT);
  pinMode(pin_blue,  OUTPUT);
};

void StatusLED::write(Status state)
{
  switch (state) {
    case Status::ready:
      analogWrite(pin_red,   255);
      analogWrite(pin_green, 200);
      analogWrite(pin_blue,  100);        
      break;
    case Status::heating:
      analogWrite(pin_red,   255);
      analogWrite(pin_green, 155);
      analogWrite(pin_blue,  155);        
      break;
    case Status::holding:
      analogWrite(pin_red,   255);
      analogWrite(pin_green, 100);
      analogWrite(pin_blue,  200);
      break;
    case Status::done:
      analogWrite(pin_red,   255);
      analogWrite(pin_green, 0);
      analogWrite(pin_blue,  255);
      break;
    case Status::error:
      analogWrite(pin_red,   0);
      analogWrite(pin_green, 255);
      analogWrite(pin_blue,  255);
      break;
    default:
      analogWrite(pin_red,   255);
      analogWrite(pin_green, 255);
      analogWrite(pin_blue,  255);
  }
};