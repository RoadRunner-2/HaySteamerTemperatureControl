#include "Keypad.h"

Keypad keypad(3, 0x20);
volatile bool key_change_pending = false;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Wire.begin();

  keypad.setup_keypad(keyChanged);
}

void loop() {
  if (key_change_pending)
  {
    key_change_pending = false;
    Serial.println(keypad.read());
  }
}

void keyChanged() // IRQ
{
  key_change_pending = true;
}