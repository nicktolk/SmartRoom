/*
 * Project SmartRoom-museum
 * Description: 2 Locations (museum and insurance) communicate via Particle.publish() and Particle.subscible
 *              Location museum reports spikes in temperature and partiulates, and recieves security lockdown instructions
 *              Location insurance receives environment data and issues lockdown demands
 * Author:      Nick Tolk
 * Date:        04-MAR-2023
 */

SYSTEM_MODE(SEMI_AUTOMATIC) // Commented out in all circumstances except debugging

// Setting "LIVE" to false discourages connecting/publishing
#define LIVE false

// To discourage warning from Adafruit_BME280.h
#ifndef ARDUINO
#define ARDUINO 0
#endif

#include "SmartRoom-museum.h"
#include "IoTClassroom_CNM.h"

// https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
#include "Adafruit_SSD1306.h"
#include "Adafruit_BME280.h"
// http://www.pjrc.com/teensy/td_libs_Encoder.html
#include "Encoder.h"

static Adafruit_SSD1306 display(OLED_RESET);  // OLED_RESET in "SmartRoom.h"
Adafruit_BME280 bme;

//static Encoder myEnc(PINA, PINB); // PINA and PINB in "SmartRoom.h"
//int encPos;                       // Encoder position

enum SwitchColors{SW_RED, SW_GREEN, SW_BLUE}; // red:lockdown; green:open; blue:override
//void setSwColor(SwitchColors color);          // Sets color on switch
void setLED(SwitchColors color);              // Sets color on red and green LEDs

int publishTick = 0, switchTick = 0, displayTick = 0, hueTick = 0;
int displayOffset = 0;            // for tracking display scrolling

double tempC;
float pressPA, humidRH;    // BME280 variables
float tempF, pressHg;
String PublishString;
String DisplayString;

Servo myServo;

const int numChars = 10;  // How many characters fit on screen
unsigned int startChar, endChar;

bool status;    // for I2C status
bool override = false;  // when true, museum overrides automatic shutdown handling
bool lockdown = true;  // true when museum is locked

int r, g, b, hue = 0, lastHue = -1, lastR = -1;

void lockDoor(bool lockedIn); // true sets servo to locked position

void setup() {
  initPins();
  Serial.begin(9600);

  WiFi.on();
  WiFi.setCredentials("IoTNetwork");

  Serial.printf("About to attempt to connect...\n");
  WiFi.connect();
  while(WiFi.connecting()) {
    Serial.printf(".");
    delay(250);
  }

  if (LIVE){
    Particle.connect();
  }

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

const int BULB = 5;
void loop() {
  tempC = bme.readTemperature();  // deg C
  humidRH = bme.readHumidity();   // %RH

  tempF = map(tempC, 0.0, 100.0, 32.0, 212.0);

  if (millis() - hueTick > hueDelay){
    hueTick = millis();
    b = 0, g = 0;
    hue = (g - b) / 255.0;
    r = map((int)(tempF*100), minTemp*100, maxTemp*100, 32, 255);
    if (hue != lastHue || r != lastR){
      lastR = r;
      lastHue = hue;
      setHue(BULB,true,hue,r,255);
    }
  }

  if (millis() - publishTick > publishDelay){   // check for publish delay
    publishTick = millis();                    // reset timer


    PublishString = String::format("%.2f;%.2f", tempF, humidRH);

    if (!LIVE){
      Serial.printf("Not live: %s\n", PublishString.c_str());
    } else if (!Particle.connected()){
      Serial.printf("Not connected, but would publish: %s\n", PublishString.c_str());
    } else {
      Serial.printf("Publishing: %s\n", PublishString.c_str());
    }
// This publishes EVERY delay tick
// Publish environment conditions. Values separated by ";".
// PRIVATE is implied here. NO_ACK benefits only the malicious museum, for taking the blame for missing packets while they're hiding data.
    if (LIVE){
      Particle.publish("tempC", String::format("%.2f", tempF), NO_ACK);
    }
  }

  // Check for override
  if (millis() - switchTick > switchDelay){   // check for publish delay
    if (digitalRead(BUTTPIN)){    // if the switch is pressed,
      switchTick = millis();
      override = !override;
      if (override){
        lockdown = false;
      }
    }
  }

  if (millis() - displayTick > displayDelay){
    if (lockdown){
      DisplayString = String::format("Closed...\nT: %.2fF\nRH:%.2f%% ", tempF, humidRH);
    } else {
      if (override){
        DisplayString = String::format("Open(shh)\nT: %.2fF\nRH:%.2f%% ", tempF, humidRH);
      } else {
        DisplayString = String::format("Open!\nT: %.2fF\nRH:%.2f%% ", tempF, humidRH);
      }
    }
    display.clearDisplay();
    display.setCursor(0, 5);
    display.printf("%s", DisplayString.c_str());
    display.display();
  }

  if (override){
    lockdown = false;
    setLED(SW_BLUE);
  } else if (tempF < fireTemp){
    lockdown = false;
    setLED(SW_GREEN);
  } else {
    lockdown = true;
    setLED(SW_RED);
  }
  lockDoor(lockdown);

}

void setLED(SwitchColors color){
  switch(color){
    case SW_RED:
      digitalWrite(LEDPINR, LEDON);    // turn on the red light,
      digitalWrite(LEDPING, LEDOFF);   // turn off the green one,
      digitalWrite(LEDPINB, LEDOFF);  // and turn off the blue one
      break;
    case SW_GREEN:
      digitalWrite(LEDPINR, LEDOFF);
      digitalWrite(LEDPING, LEDON);
      digitalWrite(LEDPINB, LEDOFF);
      break;
    default:                            // blue
      digitalWrite(LEDPINR, LEDOFF);
      digitalWrite(LEDPING, LEDOFF);
      digitalWrite(LEDPINB, LEDON);
      break;
  }
}

// sets pin states for current breadboard wiring
void initPins(){
  pinMode(LEDPINR, OUTPUT);
  pinMode(LEDPING, OUTPUT);  
  pinMode(LEDPINB, OUTPUT);
  pinMode(BUTTPIN, INPUT_PULLDOWN);
  setLED(SW_GREEN);
  myServo.attach(SERVO_PIN);
}

void lockDoor(bool lockedIn){
  if (lockedIn){
    myServo.write(LOCKED);
  } else {
    myServo.write(UNLOCKED);
  }
}