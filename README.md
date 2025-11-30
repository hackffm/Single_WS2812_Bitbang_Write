# WS2812 Bitbang — Single LED driver

Library that provides the simple function ws2812Write(pin, color) to set the color of **a single** WS2812 LED without prior initialization and without any special peripherial hardware requirements (no Timer, RMT, IRQ, DMA...) by using bitbang output. Currently ESP MCUs are supported. 

## Features
- No initalization of library or ws2812 LED required
- Simple api: `ws2812Write(uint8_t pin, uint32_t color_rgb)` Color 0xRRGGBB
- Extended api: `ws2812Write(uint8_t pin, uint32_t color, ws2812_write_led_type_t led_type)` if you need other color orders
- Minimal Footprint but time sensitive, works only for certain MCUs and clock frequencies

## API
```c
void ws2812Write(uint8_t pin, uint32_t color_rgb, ws2812_write_led_type_t led_type = WS2812WRITE_GRB);
