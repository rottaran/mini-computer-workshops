#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(4, INPUT_PULLUP);
}

void loop() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hallo");
  lcd.setCursor(0,1);
  lcd.print("Ich bin ...");

  while (digitalRead(4) != LOW) {}
  delay(10);
  while(digitalRead(4) != HIGH) {}
  delay(10);

  lcd.clear();
  
  while (digitalRead(4) != LOW) {}
  delay(10);
  while(digitalRead(4) != HIGH) {}
  delay(10);
}
