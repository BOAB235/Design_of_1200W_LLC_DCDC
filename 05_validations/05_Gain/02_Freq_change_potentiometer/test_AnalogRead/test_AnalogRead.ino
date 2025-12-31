#include "stm32f1xx.h"
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
// Set ADC resolution to 12 bits (0 to 4095)
 analogReadResolution(12);
}

void loop() {
  // put your main code here, to run repeatedly:

  int valA6 = analogRead(A6); 
Serial.println(valA6);
delay(1000);
/*
4095
4002
3894
3848
3846
3807
3720
3506
3214
2690
2249
1730
1223
600
18
4
3
4

*/

}
