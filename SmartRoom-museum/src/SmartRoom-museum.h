#ifndef _SMARTROOM_MUSEUM_
#define _SMARTROOM_MUSEUM_

#define LEDON HIGH
#define LEDOFF LOW

#ifndef ARDUINO // to discourage persistent warning
#define ARDUINO 0
#endif  // ARDUINO

// gameplay constants
const int publishDelay = 30000; // minimum ms between publishes
const int switchDelay = 500;
const int displayDelay = 10;    // display refresh
const int hueDelay = 1000;      // hue light delay

// pin assignments and addresses
const uint8_t displayAddress = 0x3C;
const uint8_t bmeAddress = 0x76;

const int minTemp = 72, maxTemp = 78;
const float fireTemp = 75;  // Temp in F to indicate fire
const int LOCKED = 90;
const int UNLOCKED = 0;

// Encoder pins
const int LEDPING = D3;
const int LEDPINR = D4;
const int LEDPINB = D5;
const int BUTTPIN = D6;
const int OLED_RESET = D9;  // Not actually wired right now and unused

const int SERVO_PIN = A5;

#endif      // _SMARTROOM_MUSEUM_