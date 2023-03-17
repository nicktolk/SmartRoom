#ifndef _SMARTROOM_MUSEUM_
#define _SMARTROOM_MUSEUM_

// For current configuration. Inverted if using button switch.
#define LEDON HIGH
#define LEDOFF LOW

// This just discourages a persistent compilation warning from Adafruit_BME280.h
#ifndef ARDUINO
#define ARDUINO 0
#endif  // ARDUINO

// gameplay constants
const int publishDelay = 30000; // minimum ms between publishes
const int buttonDelay = 500;    // between button clicks
const int displayDelay = 10;    // display refreshes

// I2C addresses
const uint8_t displayAddress = 0x3C;
const uint8_t bmeAddress = 0x76;

const float fireTemp = 76.5;    // temp in F to indicate "too hot"
const int LOCKED = 90;          // setting for lowered flag
const int UNLOCKED = 0;         // setting for raised flag

// Encoder pins
const int LEDPING = D3;     // green LED pin
const int LEDPINR = D4;     // red LED pin
const int LEDPINB = D5;     // blue LED pin
const int BUTTPIN = D6;     // button pin
const int SERVO_PIN = A5;   // Servo pin
const int OLED_RESET = D9;  // not actually wired right now and unused here

#endif      // _SMARTROOM_MUSEUM_