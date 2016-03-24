#include <Wire.h> 
#include <LiquidCrystal_SI2C.h>

#define MOTION_DETECTION_PIN 2
#define MOTION_DETECTION_GRACE_PERIOD_MS 30000
volatile boolean motionDetected = false;
unsigned long msSinceLastMotionDetected = 0;

boolean backlightOn = false;

// Software I2C for the LCD
// SCL: Arduino pin 8
// SDA: Arduino pin 9
LiquidCrystal_SI2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup() {
  lcd.begin(20, 4);
  
  lcd.noBacklight();

  lcd.home();
  lcd.print("12345678901234567890");  
  lcd.setCursor(0, 1);
  lcd.print("12345678901234567890");
  lcd.setCursor(0, 2);
  lcd.print("12345678901234567890");
  lcd.setCursor(0, 3);
  lcd.print("12345678901234567890");
  
  attachInterrupt(digitalPinToInterrupt(MOTION_DETECTION_PIN), interruptServiceRoutine, RISING);
}

void interruptServiceRoutine() {
  motionDetected = true;
}

void loop() {
  if (motionDetected) {
    motionDetected = false;
    msSinceLastMotionDetected = millis();
    lcd.backlight();
    backlightOn = true;
  }
  
  if (backlightOn && millis() - msSinceLastMotionDetected > MOTION_DETECTION_GRACE_PERIOD_MS) {
    lcd.noBacklight();
    backlightOn = false;
  }
}

// TODO
// [x] Hello world
// [x] Enable backlight with motion detector
// [ ] Make this a hardware I2C slave
// [ ] Design I2C protocol
