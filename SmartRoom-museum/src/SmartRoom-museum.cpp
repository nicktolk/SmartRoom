/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/nickt/Documents/IoT/SmartRoom/SmartRoom-museum/src/SmArtRoom-museum.ino"
/*
 * Project SmartRoom-museum
 * Description: 2 Locations (museum and insurance) communicate via Particle.publish() and Particle.subscribe()
 *              This piece, Smartroom-museum, periodically publishes the float "tempF" to be handled by Smartroom-insurance
 *              It also handles several local peripheral devices:
 *              A BME280 capable of reading local temperature, pressure, and relative humidity is used for temperature readings
 *              A SSD1306 display is used for displaying local temperature and relative humitity, as well as messages reflecting museums "open" status
 *              3 LEDs are used to show whether the temperature is above a certain cutoff (red), safe (green), or in a local security override state (blue)
 *              A simple button is used to toggle between automatic handling of security measures, or a forced-safe override state
 *              A servo attached to a flag indicates that the museum is open; actually mounted within a lock frame
 *              https://www.hackster.io/nicktolk/smartroom-2635f6
 * Author:      Nick Tolk
 * Date:        04-MAR-2023
 */

 // When publishing is enabled, it'll stall waiting for a connection. While that's off for debugging, SEMI_AUTOMATIC speeds up resets.
void setup();
void loop();
void initPins();
#line 17 "c:/Users/nickt/Documents/IoT/SmartRoom/SmartRoom-museum/src/SmArtRoom-museum.ino"
SYSTEM_MODE(SEMI_AUTOMATIC)

// Setting "LIVE" to false discourages connecting/publishing while debugging
#define LIVE true

// Constants, pin numbers, and addresses for BME280 and SSD1306
#include "SmartRoom-museum.h"

// https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
#include "Adafruit_SSD1306.h"
// https://www.adafruit.com/product/2652
#include "Adafruit_BME280.h"

// I2C declarations
static Adafruit_SSD1306 display(OLED_RESET);  // OLED_RESET in "SmartRoom-museum.h"
Adafruit_BME280 bme;

enum LEDColors{LED_RED, LED_GREEN, LED_BLUE};  // red:lockdown; green:open; blue:override
void setLED(LEDColors color);                  // sets on/off state on red, green, and blue LEDs

int publishTick = 0, buttonTick = 0, displayTick = 0; // compared against millis() and corresponding const delay for timing

float tempC, humidRH;   // read from BME280
float tempF;            // converted temperature value to be published
String DisplayString;   // string to display on marqius

Servo myServo;          // used to raise/lower flag to indicate "open" status

bool override = false;      // when true, museum overrides automatic shutdown handling and does not publish tempF
bool lockdown = true;       // true when museum is locked
bool lockdownPush = false;  // last lockdown state pushed

void lockDoor(bool lockedIn); // true sets servo to locked position

void setup() {
  bool status;    // for I2C status

  initPins();           // set pin modes
  Serial.begin(9600);   // kick off the serial connection, but don't bother waiting for it

  if (LIVE){            // if (LIVE==false), Smartroom-museum won't connect nor attempt to publish tempF
    Particle.connect(); // a delay during this operation will likely be long enough for the Serial conection to establish in case it's being monitored
  }

  Wire.begin();         // init I2C bus
// Attempt BME280 connection, and potentially report failue over Serial
// (Note: Since there's no delay to wait for the Serial connection to establish, it's probably only initialized here if Particle.connect() caused a delay)
  status = bme.begin (bmeAddress);
  if (status == false) {
    Serial.printf ("BME280 at address 0x%02X failed to start", bmeAddress) ;
  }

// Display setup. A textSize of 2 fits about 10 characters across with 3 rows on the SSD1306.
  display.begin(SSD1306_SWITCHCAPVCC, displayAddress);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.clearDisplay();
  display.display();

// Preserved but intentionally commented code would refuse to proceed without a live Particle connection
  /*if (LIVE){
    while (!Particle.connected()){
      // Just keep trying. Connect now; publish() in loop().
      Serial.printf("\rConnecting...");
    }
    if (Particle.connected()){
      Serial.printf("Connected!\n");
    }
  }*/
}

void loop() {
// temperature and humidity read locally, within enclosure
  tempC = bme.readTemperature();  // local deg C
  humidRH = bme.readHumidity();   // %RH

  tempF = map(tempC, 0.0, 100.0, 32.0, 212.0);  // conversion to F

  if (millis() - publishTick > publishDelay){   // check for publish delay
    publishTick = millis();                    // reset timer

// Serial feedback prior to publishing
    if (!LIVE || override){
      Serial.printf("Not publishing: ");
    } else if (!Particle.connected()){
      Serial.printf("Not connected, but would publish: ");
    } else {
      Serial.printf("Publishing tempF: ");
    }
    Serial.printf("%.2f\n", tempF);

// This attempts to publish tempF EVERY publishDelay ms, unless override is on
// PRIVATE is implied here, as Particle has disallowed PUBLIC publishing. 
// NO_ACK benefits only the museum, because maybe they could blame it for missing data during override.
    if (LIVE && !override){
      Particle.publish("tempF", String::format("%.2f", tempF), NO_ACK);
    }
  }

  // Check for override
  if (millis() - buttonTick > buttonDelay){ // pause to prevent double-clicks
    if (digitalRead(BUTTPIN)){                // see if the button is pressed,
      override = !override;                   // toggle override state,
      buttonTick = millis();                  // and reset the timer
    }
  }

// Handle LEDs according to current override and temperature conditions
  if (override){                // see if we're in an override state
    lockdown = false;             // never lock down during override
    setLED(LED_BLUE);             // and blue indicates state

  } else if (tempF < fireTemp){ // check current temperature against limit
    lockdown = false;             // things are cool enough to operate - no lockdown
    setLED(LED_GREEN);            // and green lets us know that

  } else {                      // things are too hot
    lockdown = true;              // so lock things down
    setLED(LED_RED);              // and light up red
  }

// Update SSD1306 display - Open/closed state and environment conditions on marquis
  if (millis() - displayTick > displayDelay){
  // First generate a string for the display
    if (lockdown){
      DisplayString = String::format("Closed...\nT: %.2fF\nRH:%.2f%% ", tempF, humidRH);    // during a lockdown state, announce closure
    } else {
      if (override){
        DisplayString = String::format("Open(shh)\nT: %.2fF\nRH:%.2f%% ", tempF, humidRH);  // announce a hush-hush override opening
      } else {
        DisplayString = String::format("Open!\nT: %.2fF\nRH:%.2f%% ", tempF, humidRH);      // regular opening
      }
    }
  // Then clear the display, load the message, and display it
    display.clearDisplay();
    display.setCursor(0, 5);
    display.printf("%s", DisplayString.c_str());
    display.display();
  }

// As this was designed to implement a lock, lockDoor() actually lowers a flag using the Servo when things are closed
  lockDoor(lockdown);

}

// Sets one LED on at a time. Constants are in "Smartroom-museum.h"
void setLED(LEDColors color){
  switch(color){
    case LED_RED:
      digitalWrite(LEDPINR, LEDON);   // turn on the red light,
      digitalWrite(LEDPING, LEDOFF);  // turn off the green one,
      digitalWrite(LEDPINB, LEDOFF);  // and turn off the blue one
      break;
    case LED_GREEN:                   // green
      digitalWrite(LEDPINR, LEDOFF);
      digitalWrite(LEDPING, LEDON);
      digitalWrite(LEDPINB, LEDOFF);
      break;
    case LED_BLUE:                    // blue
    default:                          
      digitalWrite(LEDPINR, LEDOFF);
      digitalWrite(LEDPING, LEDOFF);
      digitalWrite(LEDPINB, LEDON);
      break;
  }
}

// sets pin states for current breadboard wiring, intializes Servo, and turns on green LED
void initPins(){
  pinMode(LEDPINR, OUTPUT);
  pinMode(LEDPING, OUTPUT);  
  pinMode(LEDPINB, OUTPUT);
  pinMode(BUTTPIN, INPUT_PULLDOWN);
  myServo.attach(SERVO_PIN);
  setLED(LED_GREEN);
}

// Lowers flag when lockedIn==true
void lockDoor(bool lockedIn){
  if (lockedIn){
    myServo.write(LOCKED);
  } else {
    myServo.write(UNLOCKED);
  }
}