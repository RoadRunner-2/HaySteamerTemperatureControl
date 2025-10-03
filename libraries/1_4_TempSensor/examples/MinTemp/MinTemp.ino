#include "TempProbe.h"

TempProbe temp(5, 7, 4, 5, 8, 4);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  int minTemp = temp.read();

  Serial.println(minTemp + "°C");
  delay(1000);
}
