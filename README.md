# Password-Protected Safe using Arduino

An academic project that demonstrates a secure electronic safe system using an Arduino Uno.  
The system features user authentication via a keypad, feedback through an LCD and buzzer, and a servo-based locking mechanism.

## Features

- Username and password setup on first use  
- Hidden password input  
- Safe controlled by servo motor  
- LCD for feedback  
- Buzzer for login status  
- Locks after 3 wrong attempts (lock time doubles)  
- Supports `*` (clear), `D` (delete), `C` (close), `#` (enter).
## Circuit Details and Connections

### Components Used

- Arduino Uno  
- Keypad (4x4 Membrane)  
- Servo Motor  
- LCD 1602 I2C  
- Buzzer  
- Breadboard  
- Male/Female Jumper Wires  

### Power Connections

- Arduino 5V → Breadboard positive rail  
- Arduino GND → Breadboard negative rail  

### Servo Motor

- Red (VCC) → Breadboard positive rail  
- Brown/Black (GND) → Breadboard negative rail  
- Orange/Yellow (Signal) → Digital Pin 10  

### Keypad (4x4)

- Pin 1 (Row 1) → D2  
- Pin 2 (Row 2) → D3  
- Pin 3 (Row 3) → D4  
- Pin 4 (Row 4) → D5  
- Pin 5 (Col 1) → D6  
- Pin 6 (Col 2) → D7  
- Pin 7 (Col 3) → D8  
- Pin 8 (Col 4) → D9  

### LCD 1602 I2C

- VCC → Breadboard positive rail  
- GND → Breadboard negative rail  
- SDA → A4  
- SCL → A5  

### Buzzer

- Positive → D11  
- Negative → Breadboard negative rail  

## Modifications and Additions

- Project name and team name displayed on startup for 2.5 seconds  
- First-time setup prompts the user to enter a username and password  
- System stores the entered username/password temporarily in memory  
- Success and failure tones added to enhance interactivity  
- Keypress sound plays with every keypad input  
- System locks after 3 failed attempts, and lock duration doubles each failure cycle  

## References

- Wokwi simulation project:  
  [https://wokwi.com/projects/420621814809104385](https://wokwi.com/projects/420621814809104385)  

- Wokwi Documentation:  
  - [Buzzer](https://docs.wokwi.com/parts/wokwi-buzzer)  
  - [LCD 1602 I2C](https://docs.wokwi.com/parts/wokwi-lcd1602)  
  - [Keypad](https://docs.wokwi.com/parts/wokwi-membrane-keypad)  
  - [Servo Motor](https://docs.wokwi.com/parts/wokwi-servo)  

## Additional Notes

- The system was tested and verified using the Wokwi online simulator  
- Documentation and logic flow were refined with the help of AI tools for clarity  
