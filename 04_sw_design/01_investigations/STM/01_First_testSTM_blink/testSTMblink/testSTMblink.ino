// Blink LED on PC13 (Blue Pill onboard LED)
void setup() {
  pinMode(PC13, OUTPUT);   // Built-in LED on PC13
}

void loop() {
  digitalWrite(PC13, LOW);   // LED ON (inverted logic)
  delay(500);
  digitalWrite(PC13, HIGH);  // LED OFF
  delay(500);
}
