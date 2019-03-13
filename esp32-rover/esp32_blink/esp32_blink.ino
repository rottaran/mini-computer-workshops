#define LED_BUILTIN 2

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println("Bonjour!");

  pinMode(15, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(4, OUTPUT);

  digitalWrite(15, LOW);
  digitalWrite(2, LOW);
  digitalWrite(0, LOW);
  digitalWrite(4, LOW);

}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(15, HIGH);
  digitalWrite(2, HIGH);
  digitalWrite(0, HIGH);
  digitalWrite(4, HIGH);
  delay(1000);
  digitalWrite(15, LOW);
  digitalWrite(2, LOW);
  digitalWrite(0, LOW);
  digitalWrite(4, LOW);
  delay(2000);
}
