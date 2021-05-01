
// example using FastLED library
// led pattern taken from
// https://github.com/fablab-muenchen/TWANG/blob/master/ScrSvrPulseFlashes.h

#include <FastLED.h>

// Which pin is connected to the NeoPixels?
#define DATA_PIN 3
// How many NeoPixels are attached 
#define NUM_LEDS 12
// led array
CRGB leds[NUM_LEDS];

// setup() function -- runs once at startup --------------------------------

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(50); // 0..255
}


void loop() {
  Tick(millis());
  FastLED.show();
  FastLED.delay(50);
}

void Tick(long millis)
{
    fadeToBlackBy(leds, NUM_LEDS, 128);    
    
    randomSeed(millis);            
    int b = millis%800;
    int n = 1;
    if (b < 240) 
    {
        n = 121 - b/2;
    } 
    for(int i = 0; i<NUM_LEDS; i++)
    {
        if ((random8() <= n)) 
        {
            leds[i] = CRGB::White;
        }
    }
}
