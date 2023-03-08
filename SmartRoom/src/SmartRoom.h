#ifndef _SMARTROOM_
#define _SMARTROOM_

#define LEDON LOW
#define LEDOFF HIGH

#ifndef ARDUINO // to discourage persistent warning
#define ARDUINO 0
#endif  // ARDUINO

// gameplay constants
float maxTemp = 22, minRH = 20;
const int updateDelay = 30000;     // minimum ms between publishes

// pin assignments and addresses
const uint8_t displayAddress = 0x3C;


// Encoder pins
const int PINSWITCH = D3;
const int PINBUTTR = D8;
const int PINBUTTG = D7;
const int PINBUTTB = D6;
const int PINA = D5;
const int PINB = D4;
const int OLED_RESET = D9;  // Not actually wired right now and unused

const int encMax = 95;      // Encoder limit

#endif      // _SMARTROOM_