#include <Wire.h> 
#include <LiquidCrystal_SI2C.h>

/**********************************************************************
 * Data
 **********************************************************************/

// LCD:

boolean backlightOn = false;

// Software I2C for the LCD
// SCL: Arduino pin 8
// SDA: Arduino pin 9
LiquidCrystal_SI2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);


// Motion Detection:

#define MOTION_DETECTION_PIN 2
#define MOTION_DETECTION_GRACE_PERIOD_MS 30000
volatile boolean motionDetected = false;
unsigned long msSinceLastMotionDetected = 0;




/**********************************************************************
 * Functions
 **********************************************************************/

/**
 * LCD
 **/

void setupLCD() {
  lcd.begin(20, 4);
  
  lcd.home();
  lcd.print("12345678901234567890");  
  lcd.setCursor(0, 1);
  lcd.print("12345678901234567890");
  lcd.setCursor(0, 2);
  lcd.print("12345678901234567890");
  lcd.setCursor(0, 3);
  lcd.print("12345678901234567890");

  delay(2000);
  lcd.clear();
  delay(500);
  lcd.noBacklight();
}

void maybeDisableLCDBacklight() {
  if (backlightOn && millis() - msSinceLastMotionDetected > MOTION_DETECTION_GRACE_PERIOD_MS) {
    lcd.noBacklight();
    backlightOn = false;
  }
}


/**
 * Motion Detection
 **/

void setupMotionDetection() {
  attachInterrupt(digitalPinToInterrupt(MOTION_DETECTION_PIN), interruptServiceRoutine, RISING);  
}

void interruptServiceRoutine() {
  motionDetected = true;
}

void motionDetection() {
  if (motionDetected) {
    motionDetected = false;
    msSinceLastMotionDetected = millis();
    lcd.backlight();
    backlightOn = true;
  }
}


/**
 * I2C
 **/

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}

void setupI2CSlave() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);
}


/**
 * Arduino
 **/

void setup() {
  setupLCD();
  setupMotionDetection();
  setupI2CSlave();
}

void loop() {
  motionDetection();
  maybeDisableLCDBacklight();
}



// TODO
// [x] Hello world
// [x] Enable backlight with motion detector
// [x] Make this a hardware I2C slave
// [ ] Design I2C protocol
