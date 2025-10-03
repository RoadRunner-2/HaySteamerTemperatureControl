#include <Arduino.h>
#include "PushButton.h"

PushButton push_button(2);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  push_button.setup_push_button();
}

void loop() {
  if (was_button_pressed)
  {
    Serial.println("button was pressed");
    was_button_pressed = false;
  }
  else
  {
    Serial.println("button was not pressed");
  }

  delay(5000);
}

