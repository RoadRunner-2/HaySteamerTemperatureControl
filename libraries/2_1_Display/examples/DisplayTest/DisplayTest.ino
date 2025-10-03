#include "Display.h"


#define OLED_RESET_PIN 6
Display display(OLED_RESET_PIN);
String test_array[4];
int loop_cnt  = 0;

void setup() {
  Serial.begin(115200);
 
  display.setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  test_array[0] = "Test String 1";
  test_array[1] = "Test: " + String(loop_cnt);
  test_array[2] = "Test String 3";
  test_array[3] = "Test String 4";

  display.write(test_array);

  Serial.print ("loop count: "); 
  Serial.println (loop_cnt);
  loop_cnt++;

  delay(2000);
}