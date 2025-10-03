#define ARDUINO 1

#include <StatusLED.h>
#include <Communication.h>
#include <Relay.h>
#include <PushButton.h>
#include <Arduino.h>
#include <TempProbe.h>
#include <Display.h>
#include <Keypad.h>

#include "TaskScheduler.h"

Communication com;
NTP_Time clk;
TempProbe temp(5, 7, 4, 5, 8, 4);
Keypad keypad(3, 0x20);
volatile bool key_change_pending = false;

PushButton start_button(2);

Relay relay(12);
StatusLED led(9, 10, 11);
Display display(6);

CyclicCaller cyclic_logic(&clk, &temp, &keypad, &display, &relay, &led);

#define DEBUG 1

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Wire.begin();

  Serial.print("setup begin");
  Serial.flush();

  // connects to WLAN and sets up the NTP Clock
  com.setup_communication();
  clk.setup_clock();
  keypad.setup_keypad(keyChanged);

  start_button.setup_push_button();

  display.setup();
  relay.setup();
  led.setup();

  Serial.print("setup done");

  cyclic_logic.initializeTasks();

  if (DEBUG) Serial.print("initialize done");
}

void loop() {
  Serial.print("loop");
  if (key_change_pending)
  {
    key_change_pending = false;
    cyclic_logic.enableFastInputTask();
  }
  cyclic_logic.startTimer = start_button.is_pressed();

  cyclic_logic.executeCyclicTasks();

  cyclic_logic.disableFastInputTask();
}

void keyChanged() // IRQ
{
  key_change_pending = true;
}
