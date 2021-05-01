
// Displays a rainbow pattern when educoin lies level
// When tilted the 'down' position is lit up, again using rainbow colors 
// for eduCoin V0 prototype.

#include <FastLED.h>

// Adafruit LIS3DH accelerometer libs
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>


// Which pin is connected to the NeoPixels?
#define DATA_PIN 3
// How many NeoPixels are attached 
#define NUM_LEDS 12
// led array
CRGB leds[NUM_LEDS];

// LIS3DH 
// eduCoin uses I2C adr 0x19
#define LIS3DH_I2C_ADR 0x19
// IC is mounted with an angle in respect to first led
#define LIS3DH_ROTATION 10

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

void setup(void) 
{
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(50); // 0..255
    
    Serial.begin(115200);  
    Serial.println("LIS3DH connect...");  
    if (!lis.begin(LIS3DH_I2C_ADR)) 
    {   
        Serial.println("Couldnt start");
        while (1) yield();
    }
    Serial.println("LIS3DH found!");
  
    lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!
  
    Serial.print("Range = "); Serial.print(2 << lis.getRange());
    Serial.println("G");
  
    lis.setDataRate(LIS3DH_DATARATE_10_HZ);
    Serial.print("Data rate set to: ");
    switch (lis.getDataRate()) 
    {
        case LIS3DH_DATARATE_1_HZ: Serial.println("1 Hz"); break;
        case LIS3DH_DATARATE_10_HZ: Serial.println("10 Hz"); break;
        case LIS3DH_DATARATE_25_HZ: Serial.println("25 Hz"); break;
        case LIS3DH_DATARATE_50_HZ: Serial.println("50 Hz"); break;
        case LIS3DH_DATARATE_100_HZ: Serial.println("100 Hz"); break;
        case LIS3DH_DATARATE_200_HZ: Serial.println("200 Hz"); break;
        case LIS3DH_DATARATE_400_HZ: Serial.println("400 Hz"); break;
    
        case LIS3DH_DATARATE_POWERDOWN: Serial.println("Powered Down"); break;
        case LIS3DH_DATARATE_LOWPOWER_5KHZ: Serial.println("5 Khz Low Power"); break;
        case LIS3DH_DATARATE_LOWPOWER_1K6HZ: Serial.println("16 Khz Low Power"); break;
    }
}

int current_pos = -1;
long first_level_millis = -1;

void loop() 
{  
    lis.read();      // get X Y and Z data at once
  
    /* get a new sensor event, normalized */
    sensors_event_t event;
    lis.getEvent(&event);
    
    /* Display the results (acceleration is measured in m/s^2) */
    /*
    Serial.print(" \tX: "); Serial.print(event.acceleration.x);
    Serial.print(" \tY: "); Serial.print(event.acceleration.y);
    Serial.print(" \tZ: "); Serial.print(event.acceleration.z);
    Serial.println(" m/s^2 ");
    */
  
    Tick(event, millis());
    FastLED.show();
    FastLED.delay(50);
}

void Tick(sensors_event_t &event, long millis)
{
    int hue = (millis/10)%256;
    bool calc_new_current_pos = true;
    
    if (abs(event.acceleration.x) < 1.5 && abs(event.acceleration.y) < 1.5)
    {
        // device is hold level
        if (first_level_millis<0) first_level_millis = millis;
        if (millis - first_level_millis < 600)
        {
            // delay, use old current_pos
            calc_new_current_pos = false;
        }
        else
        {
            // use level pattern
            current_pos = -1;
            
            // modified fill_rainbow( leds, NUM_LEDS, hue, 256/NUM_LEDS);
            CHSV hsv;
            hsv.hue = hue;
            hsv.sat = 240;
            for( int i = 0; i < NUM_LEDS; ++i) 
            {
                // slowly increase brightness
                hsv.val = 50;
                leds[i] += hsv;
                // clamp at 200 max
                hsv.val = 200;
                leds[i] &= hsv;
                hsv.hue += 256/NUM_LEDS;
            }   
            return;
        }
    }
    Serial.print("current_pos: "); Serial.print(current_pos);

    if (calc_new_current_pos)
    {
        // calculate angle
        double tangent = atan2(event.acceleration.x, event.acceleration.y);
        int angle = (180+ LIS3DH_ROTATION + (int) floor(360.0 + tangent * 180.0 / M_PI)) % 360;
        int pos = angle / (360/NUM_LEDS);
        Serial.print("\t angle: "); Serial.print(angle);
        Serial.print("\t pos: "); Serial.print(pos);
    
        if (current_pos < 0)
        {
            current_pos = pos;
        }
        else
        {
            int diff = ((pos + NUM_LEDS + NUM_LEDS/2 - current_pos) % NUM_LEDS) - NUM_LEDS/2;
            if (diff > 0) current_pos++;
            if (diff < 0) current_pos--;
            current_pos = (current_pos + NUM_LEDS) % NUM_LEDS;
            //Serial.print("\t diff: "); Serial.print(diff);
            //Serial.print("\t newpos: "); Serial.print(current_pos);
        }   
        first_level_millis = -1;    
    }
    leds[current_pos] = CHSV( hue, 255, 192);
    fadeToBlackBy(leds, NUM_LEDS, 64);    
    Serial.println();
}
