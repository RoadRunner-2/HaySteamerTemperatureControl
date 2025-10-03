#include <arduino.h>
#include "StatusLED.h"

StatusLED led(9, 10, 11);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  led.setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Status::idle");
  led.write(Status::idle);
  delay(10000);
  Serial.println("Status::ready");
  led.write(Status::ready);
  delay(10000);
  Serial.println("Status::heating");
  led.write(Status::heating);
  delay(10000);
  Serial.println("Status::holding_temperature");
  led.write(Status::holding_temperature);
  delay(10000);
  Serial.println("Status::done");
  led.write(Status::done);
  delay(10000);
  Serial.println("Status::error");
  led.write(Status::error);
  delay(10000);
}
