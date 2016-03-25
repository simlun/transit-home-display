#include <Wire.h> 
#include <LiquidCrystal_SI2C.h>

#define DISP_I2C_SLAVE_ADDRESS 0x17

/**********************************************************************
 * Data
 **********************************************************************/

// LCD:

boolean backlightOn = true;

// Software I2C for the LCD
// SCL: Arduino pin 8
// SDA: Arduino pin 9
LiquidCrystal_SI2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

char lcdBuffer[4][20 + 1];


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

void lcdRefresh() {
  lcd.home();
  for (byte row = 0; row <= 3; row++) {
    for (byte column = 0; column <= 19; column++) {
      lcd.setCursor(column, row);
      lcd.print(lcdBuffer[row][column]);
      
      if (column < 19) {
        lcd.setCursor(column + 1, row);
        lcd.print('.');
      }
      if (column < 18) {
        lcd.setCursor(column + 2, row);
        lcd.print('.');
      }
      if (column < 17) {
        lcd.setCursor(column + 3, row);
        lcd.print('.');
      }
      if (row < 3) {
        lcd.setCursor(column, row + 1);
        lcd.print('.');
      }
    }
  }
}

void setupLCD() {
  // Init
  lcd.begin(20, 4);

  // Fill buffer with whitespace
  for (byte row = 0; row <= 3; row++) {
    for (byte column = 0; column <= 19; column++) {
      lcdBuffer[row][column] = ' ';
    }
  }
  
  // Make all rows proper null-terminated strings
  lcdBuffer[0][20] = '\0';
  lcdBuffer[1][20] = '\0';
  lcdBuffer[2][20] = '\0';
  lcdBuffer[3][20] = '\0';
  
  // Write buffer to LCD
  lcdRefresh();
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

byte I2C_PING = 0x01;
byte I2C_CLEAR = 0x02;
byte I2C_DATA = 0x03;
byte I2C_REFRESH = 0x04;

byte bufferRow = 0;
byte bufferCol = 0;

void pong() {
  lcd.clear();
  lcd.home();
  lcd.print("Pong!");
  delay(2000);
}

void lcdClear() {
  lcd.clear();
  lcd.home();
  bufferRow = 0;
  bufferCol = 0;
  
  // Fill buffer with whitespace
  for (byte row = 0; row <= 3; row++) {
    for (byte column = 0; column <= 19; column++) {
      lcdBuffer[row][column] = ' ';
    }
  }
}

void error(char * msg) {
  lcd.clear();
  lcd.home();
  lcd.print("ERROR:");
  lcd.setCursor(0, 1);
  lcd.print(msg);
  while (true);
}

void receiveEvent(int byteCount) {
  byte b1 = Wire.read();
  if (byteCount == 1) {
    if (b1 == I2C_PING) {
      pong();
    } else if (b1 == I2C_CLEAR) {
      lcdClear();
    } else if (b1 == I2C_REFRESH) {
      lcdRefresh();
    } else {
      error("Unexpected byte");
    }
  } else if (byteCount == 2) {
    if (b1 != I2C_DATA) {
      error("Expected I2C_DATA");
    } else {
      char c = Wire.read();
      
      if (bufferCol == 20 || c == '\n') {
        bufferCol = 0;
        bufferRow++;
      }
      if (bufferRow == 4) {
        // There's nothing past the 4th row
        return;
      }
      if (c == '\n') {
        return;
      }
      
      lcdBuffer[bufferRow][bufferCol] = c;
      bufferCol++;
    }
  } else {
    error("Unexpected byteCount");
  }
}

void setupI2CSlave() {
  Wire.begin(DISP_I2C_SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
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
