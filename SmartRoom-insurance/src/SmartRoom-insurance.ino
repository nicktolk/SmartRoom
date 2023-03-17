/*
 * Project SmartRoom-insurance
 * Description: Demos triggering a handler based on a published event from another Particle Argon
 *              Represents temperature received by subscibing to published float "tempF" on an array of neopixels
 *              https://www.hackster.io/nicktolk/smartroom-2635f6
 * Author:      Nick Tolk
 * Date:        04-MAR-2023
 */

#include "neopixel.h"
const int PIXEL_PIN = D2;
const int PIXEL_COUNT = 30;
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel pixel ( PIXEL_COUNT , PIXEL_PIN , PIXEL_TYPE ); // declare object

// Used for determining how many neopixels to light and in what color. In F.
const float minTemp = 72.0, maxTemp = 77.0;

const int pixelBrightness = 15;

// sets neopixels range to specified color
void pixelFill(int startPixel, int endPixel, uint32_t colorIn);

// handler for subscription trigger. sets neopixels according to data.
void setTower(const char *event, const char *data);

// connects, subscribes to tempF with callback function, and sets brightness on neopixels
void setup() {
  Particle.connect();
  Particle.subscribe("tempF", setTower);
  pixel.setBrightness(pixelBrightness);
}

// do nothing unless subscription is triggered
void loop() {
  delay(100);
}

// sets neopixel range to colorIn
void pixelFill(int startPixel, int endPixel, uint32_t colorIn){
  for (int i = startPixel; i < endPixel; i++){
    pixel.setPixelColor(i, colorIn);
  }
}

void setTower(const char *event, const char *data)
{
  float tempF;      // set from data
  int numLights;    // number of neopixels currently lit
  /// Not needed for this application, rgb values for red-max hue lights can be calculated by scaling (g-b). 
  /// Write-up here: https://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/
  int r, g, b;      // calculated according to number of neopixels lit
  uint32_t color;   // from r, g, b for neopixels
  
  tempF = atof(data); // atof() returns float from character array
// number of neopixels lit represents temperature
  numLights = (int)map(tempF, minTemp, maxTemp, 0.0, (float)PIXEL_COUNT);

// as temperature goes up, neopixels are less blue
  b = (int)map(tempF, minTemp, maxTemp, 255.0, 0.0);
  b = (b < 0) ? 0 : (b > 0xff) ? 0xff : b;        // bounds check
  g = 0;                                          // leave g out for this
  r = 0xff - b;                                   // less blue for hot, and more red
  color = ((r << 8) | g ) << 8 | b;               // make a neopixel-friendly value from rgb
  
  pixel.clear();                    // first clear
  pixelFill(0, numLights, color);   // then send new info
  pixel.show();                     // then show the new colors
}