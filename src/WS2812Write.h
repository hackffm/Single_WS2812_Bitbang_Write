#pragma once 
#include <stdint.h>

/**
 * @brief Write a color to a single WS2812 LED on a pin.
 * 
 * No initialization of library or pin is required.
 * Uses internal bit-banging, no MCU peripherals required.
 * Color format: 0xRRGGBB (24-bit)
 * Hint: Most WS2812 require GRB, use the led_type parameter to change color order if needed.
 *
 * @param pin GPIO number 
 * @param color_rgb 24-bit color format 0xRRGGBB
 * @param led_count Number of LEDs to write the same color to (default: 1)
 * @param led_type Color order type (default: WS2812WRITE_GRB)
 */


// ws_settings for different LEDs
typedef enum {
  WS2812WRITE_RGB,
  WS2812WRITE_BGR, 
  WS2812WRITE_BRG,
  WS2812WRITE_RBG,
  WS2812WRITE_GBR,
  WS2812WRITE_GRB   /* Most WS2812 types */
} ws2812_write_led_type_t;

void ws2812Write(uint8_t pin, uint32_t color_rgb, uint8_t led_count = 1, ws2812_write_led_type_t led_type = WS2812WRITE_GRB);
