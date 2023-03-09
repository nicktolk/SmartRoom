/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/nick/Documents/IoT/SmartRoom/SmartRoom-insurance/src/SmartRoom-insurance.ino"
/*
 * Project SmartRoom-insurance
 * Description:
 * Author:
 * Date:
 */

#include "neopixel.h"

void setTower(const char *event, const char *data);
void setup();
void loop();
#line 10 "c:/Users/nick/Documents/IoT/SmartRoom/SmartRoom-insurance/src/SmartRoom-insurance.ino"
const int minTemp = 73, maxTemp = 77;

const int PIXEL_PIN = D2;
const int PIXEL_COUNT = 46;
#define PIXEL_TYPE WS2812B

//const String accessToken = "ca7e850a1df7632615141b1c1e51c9de8e347da9";
//String tempAccess = "https://api.particle.io/v1/devices/e00fce6879a39f9da7886944/temp?access_token=ca7e850a1df7632615141b1c1e51c9de8e347da9";

int numLights;
int r, g, b;
uint32_t color;

Adafruit_NeoPixel pixel ( PIXEL_COUNT , PIXEL_PIN , PIXEL_TYPE ); // declare object

void pixelFill(int startPixel, int endPixel, uint32_t colorIn);

float tempF;
void setTower(const char *event, const char *data)
{
  tempF = atof(data);
  numLights = map((int)tempF*100, minTemp*100, maxTemp*100, 0, PIXEL_COUNT);

  b = map((int)tempF*100, minTemp*100, maxTemp*100, 0xff, 0);
  b = (b < 0) ? 0 : (b > 0xff) ? 0xff : b;
  g = 0;
  r = 0xff - b;
  color = ((r << 8) | g) << 8 | b;
  
  pixel.clear();
  pixelFill(0, numLights, color);
  pixel.show();
}


void setup() {
  Particle.connect();
  Particle.subscribe("tempF", setTower);
  pixel.setBrightness(15);
}

void loop() {
  delay(1000);
}

void pixelFill(int startPixel, int endPixel, uint32_t colorIn){
  for (int i = startPixel; i < endPixel; i++){
    pixel.setPixelColor(i, colorIn);
  }
}
