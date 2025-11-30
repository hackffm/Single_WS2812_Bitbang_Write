/**
   Write a color to a single WS2812 LED on a pin via bit-banging.
  
  The MIT License (MIT)
    Copyright (c) 2025 by Hackerspace-FFM e.V., Lutz Lisseck
*/
#include "WS2812Write.h"

#include "Arduino.h"

#ifdef ARDUINO_ARCH_ESP32
#endif

#if defined(CONFIG_IDF_TARGET_ESP32S3)
  
  #include "soc/gpio_reg.h"
  #include "driver/gpio.h"
  #include "freertos/FreeRTOS.h"
  #include "freertos/portmacro.h" 

  portMUX_TYPE mySpinlock = portMUX_INITIALIZER_UNLOCKED;

  void IRAM_ATTR setColor(uint32_t color, uint8_t pin, uint8_t time1H, uint8_t time1L, uint8_t time0H, uint8_t time0L) {
    uint32_t bitmask = 0x800000;
    uint8_t count = 24;
    uint8_t time;
    uint32_t tc_reg = GPIO_OUT_W1TC_REG;
    uint32_t ts_reg = GPIO_OUT_W1TS_REG;

    if(pin > 32) {
      tc_reg = GPIO_OUT1_W1TC_REG;
      ts_reg = GPIO_OUT1_W1TS_REG;
      pin -= 32;
    }
    uint32_t reg_val = (1UL << (pin));

    vPortEnterCritical(&mySpinlock);  // Interrupts maskieren auf aktuellem Core

    while(count--) {
      if(color & bitmask) {
        // Send 1 H_800ns L_450ns
        time = time1H; while(time--) WRITE_PERI_REG(ts_reg, reg_val);
        time = time1L; while(time--) WRITE_PERI_REG(tc_reg, reg_val); 
      } else {
        // Send 0 H_400ns L_850ns
        time = time0H; while(time--) WRITE_PERI_REG(ts_reg, reg_val); 
        time = time0L; while(time--) WRITE_PERI_REG(tc_reg, reg_val);  
      }
      bitmask = bitmask >> 1;
    }

    vPortExitCritical(&mySpinlock);   // Interrupts wieder aktivieren

  }


#elif defined(CONFIG_IDF_TARGET_ESP32)
  #error Target ESP32 is not yet supported
#else
  #error Target CONFIG_IDF_TARGET is not supported
#endif




void ws2812Write(uint8_t pin, uint32_t color_rgb, ws2812_write_led_type_t led_type)
{
  pinMode(pin, OUTPUT);

  uint8_t r = (uint8_t)((color_rgb >> 16) & 0xff);
  uint8_t g = (uint8_t)((color_rgb >> 8) & 0xff);
  uint8_t b = (uint8_t)((color_rgb >> 0) & 0xff);
  uint32_t tx_color;
  switch (led_type) {
    case WS2812WRITE_RGB:
      tx_color = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
      break;
    case WS2812WRITE_BGR:
      tx_color = ((uint32_t)b << 16) | ((uint32_t)g << 8) | r;
      break;
    case WS2812WRITE_BRG:
      tx_color = ((uint32_t)b << 16) | ((uint32_t)r << 8) | g;
      break;
    case WS2812WRITE_RBG:
      tx_color = ((uint32_t)r << 16) | ((uint32_t)b << 8) | g;
      break;
    case WS2812WRITE_GBR:
      tx_color = ((uint32_t)g << 16) | ((uint32_t)b << 8) | r;
      break;
    default:  // GRB
      tx_color = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
      break;
  }

  uint32_t Freq;
  Freq = getCpuFrequencyMhz();

  // WS2812B: 1: H_800ns L_450ns , 0: H_400ns L_850ns (Hand-Optimized for ESP32S3)
  switch(Freq) {
    case 240: 
      setColor(tx_color, pin, 13, 7, 6, 14);
      break;
    case 160: 
      setColor(tx_color, pin, 12, 7, 6, 13);
      break;
    case 80: 
      setColor(tx_color, pin, 9, 2, 4, 6); 
      break;            
  }
  


}

