/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/nickt/Documents/IoT/SmartRoom/src/Smartroom.ino"
/*
 * Project SmartRoom
 * Description: 2 Locations (museum and insurance) communicate via Particle.publish() and Particle.subscible
 *              Location museum reports spikes in temperature and partiulates, and recieves security lockdown instructions
 *              Location insurance receives environment data and issues lockdown demands
 * Author:      Nick Tolk
 * Date:        04-MAR-2023
 */

//  SYSTEM_MODE(SEMI_AUTOMATIC) // Commented out in all circumstances except debugging

#include "SmartRoom.h" // Includes constants and functions needed

// https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
#include "Adafruit_SSD1306.h"
#include "Adafruit_BME280.h"
// http://www.pjrc.com/teensy/td_libs_Encoder.html
#include "Encoder.h"



void setup();
void loop();
void initPins();
#line 22 "c:/Users/nickt/Documents/IoT/SmartRoom/src/Smartroom.ino"
static Adafruit_SSD1306 display(OLED_RESET);  // OLED_RESET in "SmartRoom.h"
Adafruit_BME280 bme;

static Encoder myEnc(PINA, PINB); // PINA and PINB in "SmartRoom.h"
int encPos;                       // Encoder position

enum SwitchColors{SW_RED, SW_GREEN, SW_BLUE}; // red:lockdown; green:open; blue:override
void setSwColor(SwitchColors color);          // Sets color on switch

int lastTick = 0;         // timer tracking

float tempC, pressPA, humidRH;    // BME280 variables
String Prs, Hum;          // Strings to hold BME280 value
//String Temp, Prs, Hum;          // Strings to hold BME280 value

void setup() {
  Serial.begin(9600);
  initPins();             // set pin states according to constants in "SmartRoom.h"

  display.begin(SSD1306_SWITCHCAPVCC, displayAddress);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.clearDisplay();
  display.display();

}


void loop() {
  encPos = myEnc.read();  // check encoder position
//  myEnc.write(encPos);  // store Encoder setting

  tempC = bme.readTemperature();  // deg C
  pressPA = bme.readPressure();   // pascals
  humidRH = bme.readHumidity();   // %RH


  display.clearDisplay();   // clear SSD1306
  display.display();        // and refresh it


  if (millis() - lastTick > updateDelay){   // check for publish delay
    lastTick = millis();                    // reset timer
// This publishes EVERY delay tick
//    Temp = String( tempC );
    Prs = String( pressPA ) ;
    Hum = String( humidRH ) ;
    Particle.publish("tC", String::format("%.2f", tempC), PRIVATE);
//    Particle.publish (" Temperature ", Temp , PRIVATE );
    Particle.publish (" Pressure ", Prs , PRIVATE );
    Particle.publish (" Humidity ", Hum , PRIVATE );
  }

  // Check for override
  if (digitalRead(PINSWITCH)){    // if the switch is pressed,
    setSwColor(SW_BLUE);
  }
}

// Sets switch color to either SW_GREEN or SW_RED.
void setSwColor(SwitchColors color){
  switch(color){
    case SW_RED:
      digitalWrite(PINBUTTR, LEDON);    // turn on the red light,
      digitalWrite(PINBUTTG, LEDOFF);   // turn off the green one,
      digitalWrite(PINBUTTB, LEDOFF);   // and turn off the blue one
      break;
    case SW_GREEN:
      digitalWrite(PINBUTTR, LEDOFF);
      digitalWrite(PINBUTTG, LEDON);
      digitalWrite(PINBUTTB, LEDOFF);
      break;
    case SW_BLUE:
    default:
      digitalWrite(PINBUTTR, LEDOFF);
      digitalWrite(PINBUTTG, LEDOFF);
      digitalWrite(PINBUTTB, LEDON);
      break;
  }
}

// sets pin states for current breadboard wiring
void initPins(){
    pinMode(PINBUTTR, OUTPUT);
    pinMode(PINBUTTG, OUTPUT);
    pinMode(PINBUTTB, OUTPUT);
    pinMode(PINSWITCH, INPUT_PULLDOWN);
    setSwColor(SW_GREEN);
}