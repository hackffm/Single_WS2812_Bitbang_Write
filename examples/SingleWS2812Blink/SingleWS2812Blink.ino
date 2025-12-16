/*
  SingleWS2812Blink.ino - Example sketch to blink a single WS2812 LED
  Created by Lutz Lisseck, December 2025.

  Colors the WS2812 LED in pink for one second,
  then turns it green for one second in an endless loop. 

  LED should be a WS2812 or WS2812B type.
  
  Add a third argument to ws2812Write() if your LED uses a 
  different color order then GRB (as most WS2812 types do).

  This example code is in the public domain.
*/

#include "WS2812Write.h"
#ifndef WS2812_LED_PIN
#define WS2812_LED_PIN 6  // Pin where the WS2812 LED is connected
#endif

// the setup function runs once when you press reset or power the board
void setup() {
  // no need to initialize the ws2812 library or pin before :-)
  //Serial.begin(74880); // Start serial for debugging (optional)
  //Serial.println("Single WS2812 Blink Example");
}

// the loop function runs over and over again forever
void loop() {
  ws2812Write(WS2812_LED_PIN, 0xFF00FF);  // turn the LED pink (RGB: 255, 0, 255)
  delay(1000);                            // wait for a second
  ws2812Write(WS2812_LED_PIN, 0x00FF00);  // turn the LED green (RGB: 0, 255, 0)
  delay(1000);                            // wait for a second
}
