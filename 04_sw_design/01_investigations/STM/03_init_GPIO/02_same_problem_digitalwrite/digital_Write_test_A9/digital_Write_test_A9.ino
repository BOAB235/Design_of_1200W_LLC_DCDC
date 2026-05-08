void setup() {
  pinMode(PA9, OUTPUT);

  digitalWrite(PA9, HIGH);
  delayMicroseconds(1);

  digitalWrite(PA9, LOW);
  delayMicroseconds(2);

  while (1); // stop
}

void loop() {
  // nothing
}
