#include "Communication.h"
#include <TimeLib.h>

Communication com;
NTP_Time ntp_clock;

void setup() {
//Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  com.setup_communication();
  ntp_clock.setup_clock();
}

void loop() {
  // put your main code here, to run repeatedly:
  time_t clockTime = ntp_clock.read();

  digitalClockDisplay(clockTime);
  delay(60000);
}

void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void digitalClockDisplay(time_t time){
  // digital clock display of the time
  Serial.print(hour(time));
  printDigits(minute(time));
  Serial.print(" ");
  Serial.print(day(time));
  Serial.print(".");
  Serial.print(month(time));
  Serial.print(".");
  Serial.print(year(time)); 
  Serial.println(); 
}
