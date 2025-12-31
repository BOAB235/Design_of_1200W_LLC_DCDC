// PB12 open-drain with external 10k pull-up
void setup() {
  pinMode(PB12, OUTPUT_OPEN_DRAIN);

  // Release the line (high via external pull-up)
  digitalWrite(PB12, HIGH);
  delayMicroseconds(1);

  // Pull the line low (sink)
  digitalWrite(PB12, LOW);
  delayMicroseconds(2);

  while (1) { /* stop */ }
}

void loop() {
  // nothing
}