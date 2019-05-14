#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(4, INPUT_PULLUP);

  lcd.setCursor(0,1);
  lcd.print("druecken...");

  pinMode(5, OUTPUT);
  pinMode(8, OUTPUT);

  digitalWrite(5, HIGH);
  delay(500);
  digitalWrite(5, LOW);
}

void loop() {
  while (digitalRead(4) != LOW) {}
  delay(10);
  while(digitalRead(4) != HIGH) {}
  delay(10);

  lcd.clear();

  delay( random(500, 10000) );
  
  digitalWrite(5, HIGH);
  tone(8, random(100, 1000) );
  
  long start;
  start = micros();

  while (digitalRead(4) != LOW) {
    lcd.setCursor(0,0);
    lcd.print( (micros() - start) / 1000.0 );  
    lcd.print("ms");
  }
  delay(10);
  while(digitalRead(4) != HIGH) {}
  delay(10);

  digitalWrite(5, LOW);
  noTone(8);
  long ende;
  ende = micros();

  lcd.setCursor(0,0);
  lcd.print( (micros() - start) / 1000.0, 3);
  lcd.print("ms");
  lcd.setCursor(0,1);
  lcd.print("druecken...");
}
