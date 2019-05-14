void setup() {
  pinMode(5, OUTPUT);
}

void loop() {
  digitalWrite(5, HIGH);
  delay(500);
  digitalWrite(5, LOW);
  delay(500);
}
