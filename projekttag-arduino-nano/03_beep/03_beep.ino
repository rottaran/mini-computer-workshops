void setup() {
  pinMode(8, OUTPUT);
}

void loop() {
  tone(8, 200);
  delay(500);
  noTone(8);
  delay(500);
}
