#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(4, INPUT_PULLUP);

  lcd.setCursor(0,1);
  lcd.print("drücken zum Wiederholen");

  pinMode(5, OUTPUT);
}

void loop() {
  while (digitalRead(4) != LOW) {}
  delay(10);
  while(digitalRead(4) != HIGH) {}
  delay(10);

  lcd.clear();
  digitalWrite(5, HIGH);
  
  long start;
  start = millis();

  while (digitalRead(4) != LOW) {
    lcd.setCursor(0,0);
    lcd.print( (millis() - start) / 1000.0 );  
  }
  delay(10);
  while(digitalRead(4) != HIGH) {}
  delay(10);

  digitalWrite(5, LOW);
  long ende;
  ende = millis();

  lcd.setCursor(0,0);
  lcd.print( (millis() - start) / 1000.0 );  
  lcd.print("s");
  lcd.setCursor(0,1);
  lcd.print("drücken zum Wiederholen");
}
