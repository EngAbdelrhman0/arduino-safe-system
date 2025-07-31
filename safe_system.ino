// Define libraries
#include "Keypad.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "Servo.h"

// Define the 4x4 keypad pins
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; // Connect to the column pinouts of the keypad

// Initialize the keypad, LCD, and servo
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD I2C address and dimensions
Servo myServo; // Create a Servo object

// Set password and other variables
String username = ""; // Store the username
String password = ""; // Store the password
String enteredPassword = ""; // Store the entered password
bool safeOpen = false; // Store the state of the safe (open or closed)
bool isFirstTime = true; // Check if it's the first time using the safe

// Buzzer pin
const int buzzerPin = 11; // Connect buzzer to pin 11

// Variables for failed attempts and system lock
int failedAttempts = 0; // Number of failed attempts
const int MAX_ATTEMPTS = 3; // Maximum allowed attempts
unsigned long lockTime = 60000; // Initial lock time (60 seconds)
bool systemLocked = false; // System lock state
unsigned long lockStartTime = 0; // Time when the system was locked

// Password length
const int MAX_PASSWORD_LENGTH = 5; // Change this to increase password length

// Commands inside void setup run once
void setup() {
  Serial.begin(9600); // Initialize serial communication
  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the LCD backlight
  myServo.attach(10); // Attach the servo to pin 10
  myServo.write(180); // Initialize the servo to closed position
  pinMode(buzzerPin, OUTPUT); // Set buzzer pin as output
  digitalWrite(buzzerPin, LOW); // Ensure buzzer is off initially

  // Display splash screen
  lcd.setCursor(1, 0);
  lcd.print("G20 Team"); // Display team name on the first line
  lcd.setCursor(1, 1);
  lcd.print("Security Safe"); // Display project name on the second line
  delay(2500); // Wait for 2.5 seconds

  // Clear the screen after splash screen
  lcd.clear();
}

// Function to play success tone
void playSuccessTone() {
  tone(buzzerPin, 1000); // High pitch tone
  delay(500);
  noTone(buzzerPin);
}

// Function to play fail tone
void playFailTone() {
  tone(buzzerPin, 500); // Low pitch tone
  delay(1000);
  noTone(buzzerPin);
}

// Function to play warning tone
void playWarningTone() {
  for (int i = 0; i < 3; i++) {
    tone(buzzerPin, 800); // Beep tone
    delay(300);
    noTone(buzzerPin);
    delay(200);
  }
}

// Function to play keypress tone
void playKeypressTone() {
  tone(buzzerPin, 800); // Medium pitch tone
  delay(100); // Short beep
  noTone(buzzerPin);
}

// Function to enter username and password for the first time
void setupUsernameAndPassword() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Username:");
  while (username.length() < 5) { // Username length is 5 characters
    char key = keypad.getKey();
    if (key != NO_KEY) {
      if (key == '#') break; // Skip if # is pressed
      playKeypressTone(); // Play keypress tone
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
      if (key == '#') break; // Skip if # is pressed
      playKeypressTone(); // Play keypress tone
      password += key;
      lcd.setCursor(0, 1);
      for (int i = 0; i < password.length(); i++) {
        lcd.print("*"); // Display password as asterisks
      }
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setup Complete!");
  delay(2000);
  isFirstTime = false; // Mark setup as complete
}

// Commands inside void loop run forever
void loop() {
  static bool showEnterPassword = false;
  static unsigned long prevMillis = 0;
  const unsigned long delayTime = 5000; // 5 seconds
  static bool showSafeClosed = false;
  static unsigned long safeClosedMillis = 0;
  const unsigned long safeClosedDelayTime = 2000; // 2 seconds

  // First-time setup
  if (isFirstTime) {
    setupUsernameAndPassword();
  }

  // Check if the system is locked
  if (systemLocked) {
    unsigned long remainingTime = lockTime - (millis() - lockStartTime); // Calculate remaining time
    if (remainingTime <= 0) {
      systemLocked = false; // Unlock the system
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Password:");
      failedAttempts = 0; // Reset failed attempts
    } else {
      lcd.setCursor(0, 0);
      lcd.print("System Locked!");
      lcd.setCursor(0, 1);
      lcd.print("Time left: " + String(remainingTime / 1000) + "s"); // Display remaining time
      return; // Skip the rest of the loop if the system is locked
    }
  }

  // Show "Enter Password" message after 5 seconds
  if (!showEnterPassword && millis() - prevMillis >= delayTime) {
    showEnterPassword = true;
    prevMillis = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("User: " + username); // Display username
    lcd.setCursor(0, 1);
    lcd.print("Enter Password:");
  }

  // Read keypad input and handle password entry
  char key = keypad.getKey();
  if (showEnterPassword && key != NO_KEY) {
    playKeypressTone(); // Play keypress tone for any key pressed

    if (key == '#') { // Check if enter key is pressed
      if (enteredPassword == password) { // Compare entered password with the correct password
        if (safeOpen) { // If safe is already open, close it and ask to enter password again
          safeOpen = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Press C to close");
          enteredPassword = ""; // Clear entered password
        } else { // If safe is closed, open it and ask to close it
          safeOpen = true;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Safe is open");
          lcd.setCursor(0, 1);
          lcd.print("Press C to close");
          myServo.write(0); // Open the safe by turning the servo to 0 degrees
          playSuccessTone(); // Play success tone
          enteredPassword = ""; // Clear entered password
        }
        failedAttempts = 0; // Reset failed attempts
      } else { // Incorrect password entered
        failedAttempts++; // Increment failed attempts
        if (failedAttempts >= MAX_ATTEMPTS) {
          systemLocked = true; // Lock the system
          lockStartTime = millis(); // Record the lock start time
          lockTime *= 2; // Double the lock time
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("System Locked!");
          playWarningTone(); // Play warning tone
          delay(1000);
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wrong Password!");
          lcd.setCursor(0, 1);
          lcd.print("Attempts left: " + String(MAX_ATTEMPTS - failedAttempts));
          playFailTone(); // Play fail tone
          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("User: " + username); // Display username
          lcd.setCursor(0, 1);
          lcd.print("Enter Password:");
        }
        enteredPassword = ""; // Clear entered password
      }
    } else if (key == '*') { // Check if clear key is pressed
      enteredPassword = ""; // Clear entered password
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("User: " + username); // Display username
      lcd.setCursor(0, 1);
      lcd.print("Enter Password:");
    } else if (key == 'C' || key == 'c') { // Check if C key is pressed
      if (safeOpen) { // If safe is open, close it
        safeOpen = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Safe is closed");
        myServo.write(180); // Close the safe by turning the servo to 180 degrees
        showSafeClosed = true;
        safeClosedMillis = millis();
        enteredPassword = ""; // Clear entered password
      }
    } else if (key == 'D' || key == 'd') { // Check if D key is pressed
      if (enteredPassword.length() > 0) { // If password is not empty, delete last character
        enteredPassword = enteredPassword.substring(0, enteredPassword.length() - 1);
        lcd.setCursor(0, 1);
        lcd.print("            "); // Clear the password field on the LCD screen
        lcd.setCursor(0, 1);
        // Print asterisks instead of the actual password characters
        for (int i = 0; i < enteredPassword.length(); i++) {
          lcd.print("*");
        }
      }
    } else { // Append entered key to the entered password
      if (enteredPassword.length() < MAX_PASSWORD_LENGTH && !safeOpen) { // Limit password to MAX_PASSWORD_LENGTH
        enteredPassword += key;
        lcd.setCursor(0, 1);
        // Print asterisks instead of the actual password characters
        for (int i = 0; i < enteredPassword.length(); i++) {
          lcd.print("*");
        }
      }
    }
  }

  // Show "Safe Closed" message for 2 seconds after the safe is closed
  if (showSafeClosed && millis() - safeClosedMillis >= safeClosedDelayTime) {
    showSafeClosed = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("User: " + username); // Display username
    lcd.setCursor(0, 1);
    lcd.print("Enter Password:");
  }
}