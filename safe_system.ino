#include "Keypad.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "Servo.h"
#include <math.h>

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

String username = "";
String password = "";
String enteredPassword = "";
bool safeOpen = false;
bool isFirstTime = true;

const int buzzerPin = 11;

int failedAttempts = 0;
const int MAX_ATTEMPTS = 3;
unsigned long lockTime = 10000;
int lockCount = 0;
bool systemLocked = false;
unsigned long lockStartTime = 0;

const int MAX_PASSWORD_LENGTH = 5;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  myServo.attach(10);
  myServo.write(180);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  lcd.setCursor(1, 0);
  lcd.print("G20 Team");
  lcd.setCursor(1, 1);
  lcd.print("Security Safe");
  delay(2500);
  lcd.clear();
}

void playSuccessTone() {
  tone(buzzerPin, 1000);
  delay(500);
  noTone(buzzerPin);
}

void playFailTone() {
  tone(buzzerPin, 500);
  delay(1000);
  noTone(buzzerPin);
}

void playWarningTone() {
  for (int i = 0; i < 3; i++) {
    tone(buzzerPin, 800);
    delay(300);
    noTone(buzzerPin);
    delay(200);
  }
}

void playKeypressTone() {
  tone(buzzerPin, 800);
  delay(100);
  noTone(buzzerPin);
}

void setupUsernameAndPassword() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Username:");
  while (username.length() < 5) {
    char key = keypad.getKey();
    if (key != NO_KEY) {
      if (key == '#') break;
      playKeypressTone();
      username += key;
      lcd.setCursor(0, 1);
      lcd.print(username);
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
  while (password.length() < MAX_PASSWORD_LENGTH) {
    char key = keypad.getKey();
    if (key != NO_KEY) {
      if (key == '#') break;
      playKeypressTone();
      password += key;
      lcd.setCursor(0, 1);
      for (int i = 0; i < password.length(); i++) {
        lcd.print("*");
      }
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setup Complete!");
  delay(2000);
  isFirstTime = false;
}

void loop() {
  static bool showEnterPassword = false;
  static unsigned long prevMillis = 0;
  const unsigned long delayTime = 5000;
  static bool showSafeClosed = false;
  static unsigned long safeClosedMillis = 0;
  const unsigned long safeClosedDelayTime = 2000;

  if (isFirstTime) {
    setupUsernameAndPassword();
  }

  if (systemLocked) {
    unsigned long remainingTime = lockTime - (millis() - lockStartTime);
    if ((long)remainingTime <= 0) {
      systemLocked = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Password:");
      failedAttempts = 0;
    } else {
      lcd.setCursor(0, 0);
      lcd.print("System Locked!");
      lcd.setCursor(0, 1);
      int sec = remainingTime / 1000;
      String timeStr = (sec < 10 ? "0" : "") + String(sec) + "s";
      lcd.print("Time left: " + timeStr);
      return;
    }
  }

  if (!showEnterPassword && millis() - prevMillis >= delayTime) {
    showEnterPassword = true;
    prevMillis = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("User: " + username);
    lcd.setCursor(0, 1);
    lcd.print("Enter Password:");
  }

  char key = keypad.getKey();
  if (showEnterPassword && key != NO_KEY) {
    playKeypressTone();

    if (key == '#') {
      if (enteredPassword == password) {
        if (safeOpen) {
          safeOpen = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Press C to close");
          enteredPassword = "";
        } else {
          safeOpen = true;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Safe is open");
          lcd.setCursor(0, 1);
          lcd.print("Press C to close");
          myServo.write(0);
          playSuccessTone();
          enteredPassword = "";
        }
        failedAttempts = 0;
      } else {
        failedAttempts++;
        if (failedAttempts >= MAX_ATTEMPTS) {
          systemLocked = true;
          lockCount++;
          lockTime = 10000 * pow(2, lockCount);
          lockStartTime = millis();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("System Locked!");
          playWarningTone();
          delay(1000);
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wrong Password!");
          lcd.setCursor(0, 1);
          lcd.print("Attempts left: " + String(MAX_ATTEMPTS - failedAttempts));
          playFailTone();
          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("User: " + username);
          lcd.setCursor(0, 1);
          lcd.print("Enter Password:");
        }
        enteredPassword = "";
      }
    } else if (key == '*') {
      enteredPassword = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("User: " + username);
      lcd.setCursor(0, 1);
      lcd.print("Enter Password:");
    } else if (key == 'C' || key == 'c') {
      if (safeOpen) {
        safeOpen = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Safe is closed");
        myServo.write(180);
        showSafeClosed = true;
        safeClosedMillis = millis();
        enteredPassword = "";
      }
    } else if (key == 'D' || key == 'd') {
      if (enteredPassword.length() > 0) {
        enteredPassword = enteredPassword.substring(0, enteredPassword.length() - 1);
        lcd.setCursor(0, 1);
        lcd.print("            ");
        lcd.setCursor(0, 1);
        for (int i = 0; i < enteredPassword.length(); i++) {
          lcd.print("*");
        }
      }
    } else {
      if (enteredPassword.length() < MAX_PASSWORD_LENGTH && !safeOpen) {
        enteredPassword += key;
        lcd.setCursor(0, 1);
        for (int i = 0; i < enteredPassword.length(); i++) {
          lcd.print("*");
        }
      }
    }
  }

  if (showSafeClosed && millis() - safeClosedMillis >= safeClosedDelayTime) {
    showSafeClosed = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("User: " + username);
    lcd.setCursor(0, 1);
    lcd.print("Enter Password:");
  }
}
