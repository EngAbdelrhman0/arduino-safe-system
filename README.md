# Password-Protected Safe using Arduino

An academic project that demonstrates a secure electronic safe system using an Arduino Uno.  
The system features user authentication via a keypad, feedback through an LCD and buzzer, and a servo-based locking mechanism.

## Features

- Set up a custom username and password on first use  
- Hide password input using asterisks `*`  
- Lock/unlock the safe using a servo motor  
- Display feedback on an I2C LCD screen  
- Audio feedback using a buzzer for:
  - Successful login
  - Wrong password
  - System lock warning
- System lockout after 3 failed attempts, with increasing lock time
- Delete characters with `D`, clear all with `*`, close the safe with `C`

## Tools & Components

- Arduino Uno  
- Keypad (4x4)  
- I2C LCD Display (16x2)  
- Buzzer  
- Servo Motor  
- Arduino IDE (C++)  

## How It Works

1. On first boot, the user sets a username and password (each up to 5 characters)  
2. On every restart, the system asks for the saved password to unlock  
3. After 3 wrong attempts, the system locks for 60 seconds and doubles the lock time with each subsequent lockout  
4. The user can:
   - Clear input with `*`
   - Delete last character with `D`
   - Close the safe using `C` if it’s open

## Prototype

You can explore a prototype of the user interface interactions using Adobe XD:  
[View Adobe XD Prototype](ضع-هنا-الرابط)

## Author

**Abdelrhman Ashraf**  
Electrical Engineering Student – Helwan University  
[LinkedIn](https://www.linkedin.com/in/abdelrhman-ashraf1)
