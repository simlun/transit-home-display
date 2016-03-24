#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup() {
  lcd.begin(20, 4);
  lcd.backlight();

  lcd.home();
  lcd.print("12345678901234567890");  
  lcd.setCursor(0, 1);
  lcd.print("12345678901234567890");
  lcd.setCursor(0, 2);
  lcd.print("12345678901234567890");
  lcd.setCursor(0, 3);
  lcd.print("12345678901234567890");
  
  delay(2500);
  lcd.noBacklight();
}

void loop() {}

// TODO
// [x] Hello world
// [ ] Enable backlight with motion detector
// [ ] Make this an I2C slave
// [ ] Design I2C protocol
