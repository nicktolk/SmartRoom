/*
 * Project SmartRoom-museum
 * Description: 2 Locations (museum and insurance) communicate via Particle.publish() and Particle.subscible
 *              Location museum reports spikes in temperature and partiulates, and recieves security lockdown instructions
 *              Location insurance receives environment data and issues lockdown demands
 * Author:      Nick Tolk
 * Date:        04-MAR-2023
 */

SYSTEM_MODE(SEMI_AUTOMATIC) // Commented out in all circumstances except debugging

// To discourage warning from Adafruit_BME280.h
#ifndef ARDUINO
#define ARDUINO 0
#endif

#include "SmartRoom-museum.h"

// https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
#include "Adafruit_SSD1306.h"
#include "Adafruit_BME280.h"
// http://www.pjrc.com/teensy/td_libs_Encoder.html
#include "Encoder.h"

static Adafruit_SSD1306 display(OLED_RESET);  // OLED_RESET in "SmartRoom.h"
Adafruit_BME280 bme;

static Encoder myEnc(PINA, PINB); // PINA and PINB in "SmartRoom.h"
int encPos;                       // Encoder position

enum SwitchColors{SW_RED, SW_GREEN, SW_BLUE}; // red:lockdown; green:open; blue:override
void setSwColor(SwitchColors color);          // Sets color on switch

int publishTick = 0, switchTick = 0, displayTick = 0;
int displayOffset = 0;            // for tracking display scrolling

double tempC;
float pressPA, humidRH;    // BME280 variables
float tempF, pressHg;
String PublishString;
String DisplayString;

const int numChars = 10;  // How many characters fit on screen
unsigned int startChar, endChar;

bool status;    // for I2C status
bool override;  // when true, museum overrides automatic shutdown handling

int getTemp(String strIn);

void setup() {
  initPins();
  Serial.printf("About to attempt to connect...\n");

  Serial.begin(9600);
  Particle.variable("tempC", tempC);
  Particle.function("getTemp", getTemp);
  Particle.connect();

  Wire.begin();
  status = bme.begin (bmeAddress);
  if (status == false) {
    Serial.printf ("BME280 at address 0x%02X failed to start", bmeAddress) ;
  }

  display.begin(SSD1306_SWITCHCAPVCC, displayAddress);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.clearDisplay();
  display.display();

//  while (!Particle.connected()){
    // Just keep trying. Connect now; publish() in loop();.
//    Serial.printf("\rConnecting...");
//  }
  if (Particle.connected()){
    Serial.printf("Connected!\n");
  }
}


void loop() {
   if (millis() - publishTick > publishDelay){   // check for publish delay
    publishTick = millis();                    // reset timer

    tempC = bme.readTemperature();  // deg C
    humidRH = bme.readHumidity();   // %RH

    tempF = map(tempC, 0.0, 100.0, 32.0, 212.0);

    PublishString = String::format("%.2f;%.2f", tempF, humidRH);
    if (millis() - displayTick > displayDelay){
      DisplayString = String::format("T: %.2fF\nRH:%.2f%% ", tempF, humidRH);
    //  DisplayString = String::format("12345678901234567890");
      display.clearDisplay();
      display.setCursor(0, 5);
/*      display.setCursor(displayOffset, 5);
      startChar = ((displayOffset / numChars) + DisplayString.length()) % DisplayString.length();
//      startChar = (startChar < 0) ? 0 : startChar;
      endChar = (startChar + numChars > DisplayString.length()) ? DisplayString.length() : startChar + numChars;
      if (endChar <= DisplayString.length()){
        DisplayString = DisplayString.substring(startChar, endChar);
      } else {
        DisplayString = DisplayString.substring(startChar, endChar) + DisplayString.substring(DisplayString.length() - endChar, DisplayString.length());
        DisplayString = DisplayString.substring(0, numChars);
      }*/
//      Serial.printf("%u : %u\n", startChar, endChar);
//      Serial.printf("%s\n", DisplayString.c_str());
      display.printf("%s", DisplayString.c_str());

      display.display();
//      displayOffset -= 1;
//      displayOffset = (displayOffset < -display.width()) ? 0: displayOffset;
    }

    if (!Particle.connected()){
      Serial.printf("Not connected, but trying to publish: ");
    } else {
      Serial.printf("Publishing: ");
    }
   Serial.printf("PublishString: %s\n", PublishString.c_str());
// This publishes EVERY delay tick
// Publish environment conditions. Values separated by ";".
// PRIVATE is implied here. NO_ACK benefits only the malicious museum, for taking the blame for missing packets while they're hiding data.
    Particle.publish("tempC", String::format("%.2f", tempF), NO_ACK);
  }

  // Check for override
  if (millis() - switchTick > switchDelay){   // check for publish delay
    if (digitalRead(PINSWITCH)){    // if the switch is pressed,
      switchTick = millis();
      override = !override;
      if (override){
        setSwColor(SW_BLUE);
      } else {
        setSwColor(SW_GREEN);
      }
    }
  }
}

int getTemp(String strIn){
  return ((int)tempC * 100);
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