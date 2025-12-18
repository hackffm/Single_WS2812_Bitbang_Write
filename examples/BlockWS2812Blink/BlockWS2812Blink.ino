/*
  BlockWS2812Blink.ino - Example sketch to blink a line of 8 WS2812 LEDs
  Created by Lutz Lisseck, December 2025.

  Colors the WS2812 LEDs in pink for one second,
  then turns it green for one second in an endless loop. 

  LEDs should be a WS2812 or WS2812B type.
  
  Add a fourth argument to ws2812Write() if your LED uses a 
  different color order then GRB (as most WS2812 types do).

  This example code is in the public domain.
*/

#include <WS2812Write.h>
#ifndef WS2812_LED_PIN    // can be defined in platformio.ini
#define WS2812_LED_PIN 6  // Pin where the WS2812 LED is connected
#endif

#define LED_BLOCK_COUNT 8  // Number of WS2812 LEDs in the block


// the setup function runs once when you press reset or power the board
void setup() {
  // no need to initialize the ws2812 library or pin before :-)
  //Serial.begin(74880); // Start serial for debugging (optional)
  //Serial.println("Single WS2812 Blink Example");
}

// the loop function runs over and over again forever
void loop() {
  ws2812Write(WS2812_LED_PIN, 0xFF00FF, LED_BLOCK_COUNT);  // turn the LEDs pink (RGB: 255, 0, 255)
  delay(1000);                            // wait for a second
  ws2812Write(WS2812_LED_PIN, 0x00FF00, LED_BLOCK_COUNT);  // turn the LEDs green (RGB: 0, 255, 0)
  delay(1000);                            // wait for a second
}
