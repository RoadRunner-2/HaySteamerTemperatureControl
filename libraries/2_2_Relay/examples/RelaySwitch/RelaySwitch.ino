#include "Relay.h"

Relay relay(12);

void setup() {
   //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  relay.setup();
}

void loop() {
  relay.write(HIGH);
  Serial.print("Relay State: ");
  Serial.println(relay.is_relay_on());
  
  delay(2000);
  
  relay.write(LOW);
  Serial.print("Relay State: ");
  Serial.println(relay.is_relay_on());

  delay(2000);
}
