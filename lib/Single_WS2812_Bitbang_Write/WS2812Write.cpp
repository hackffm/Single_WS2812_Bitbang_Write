/**
   Write a color to a single WS2812 LED on a pin via bit-banging.
  
  The MIT License (MIT)
    Copyright (c) 2025 by Hackerspace-FFM e.V., Lutz Lisseck

  WS2812B Timing:
    0: H_400ns L_850ns (+/-150ns)
    1: H_800ns L_450ns (+/-150ns)  
*/
#include "WS2812Write.h"
#include "Arduino.h"

#if defined(ARDUINO_ARCH_ESP32) 
  #include "soc/gpio_reg.h"
  #include "driver/gpio.h"
  #include "freertos/FreeRTOS.h"
  #include "freertos/portmacro.h" 

  #define GET_CPU_CYCLES() esp_cpu_get_cycle_count()

  portMUX_TYPE mySpinlock = portMUX_INITIALIZER_UNLOCKED;

  void IRAM_ATTR setColor(uint32_t color, uint8_t pin, uint8_t time1H, uint8_t time1L, uint8_t time0H, uint8_t time0L) {
    uint32_t bitmask = 0x800000;
    volatile uint8_t count = 24;
    uint32_t start;
    uint32_t tc_reg = GPIO_OUT_W1TC_REG;
    uint32_t ts_reg = GPIO_OUT_W1TS_REG;
    uint32_t cycles;

    if(pin > 32) {
      tc_reg = GPIO_OUT1_W1TC_REG;
      ts_reg = GPIO_OUT1_W1TS_REG;
      pin -= 32;
    }
    uint32_t reg_val = (1UL << (pin));

    vPortEnterCritical(&mySpinlock);  // Interrupts off on current core

    while(count--) {
      cycles = (color & bitmask) ? time1H : time0H;
      WRITE_PERI_REG(ts_reg, reg_val);
      start = GET_CPU_CYCLES();
      while (GET_CPU_CYCLES() - start < cycles) { __asm__ __volatile__("nop");  } 

      cycles = (color & bitmask) ? time1L : time0L;
      bitmask = bitmask >> 1;
      WRITE_PERI_REG(tc_reg, reg_val); 
      start = GET_CPU_CYCLES();
      while (GET_CPU_CYCLES() - start < cycles) { __asm__ __volatile__("nop");  } 
    }

    vPortExitCritical(&mySpinlock);   // Interrupts on

  }

#elif defined(ARDUINO_ARCH_ESP8266)
  #include "ets_sys.h"
  #include "eagle_soc.h"
  #include "gpio.h"
  #include <user_interface.h>

  #define GET_CPU_CYCLES() esp_get_cycle_count()

  void IRAM_ATTR setColor(uint32_t color, uint8_t pin, uint8_t time1H, uint8_t time1L, uint8_t time0H, uint8_t time0L) {
    uint32_t bitmask = 0x800000;
    volatile uint8_t count = 24;
    uint32_t start;
    uint32_t cycles;

    if(pin > 32) return;
    uint32_t reg_val = (1UL << (pin));

    ETS_INTR_LOCK();  // Interrupts off

    while(count--) {
      cycles = (color & bitmask) ? time1H : time0H;
      GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, reg_val);
      start = GET_CPU_CYCLES();
      while (GET_CPU_CYCLES() - start < cycles) { __asm__ __volatile__("nop");  } 

      cycles = (color & bitmask) ? time1L : time0L;
      bitmask = bitmask >> 1;
      GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, reg_val); 
      start = GET_CPU_CYCLES();
      while (GET_CPU_CYCLES() - start < cycles) { __asm__ __volatile__("nop");  } 
    }

    ETS_INTR_UNLOCK();   // Interrupts on
  }

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
  uint8_t time1H, time1L, time0H, time0L;

  // WS2812B: 1: H_800ns L_450ns (Send a '1' bit)
  //          0: H_400ns L_850ns (Send a '0' bit)

  #ifdef ARDUINO_ARCH_ESP32
  Freq = getCpuFrequencyMhz();  
    #if defined(CONFIG_IDF_TARGET_ESP32) 
    // Tested for ESP32
    time1H = (800 * Freq) / 1000 - 11; 
    time0H = (400 * Freq) / 1000 - 15; 
    time1L = (450 * Freq) / 1000 - 25; 
    time0L = (850 * Freq) / 1000 - 28; 
    if(Freq <= 80) {
      // Adjust timing for 80 MHz
      time1L -= 4;
      time0L -= 4;
    } 
    #else
    // Tested for ESP32-S3 80/160/240 MHz
    time1H = (800 * Freq) / 1000 - 11; 
    time0H = (400 * Freq) / 1000 - 15; 
    time1L = (450 * Freq) / 1000 - 18; 
    time0L = (850 * Freq) / 1000 - 26; 
    #endif
  #elif defined(ARDUINO_ARCH_ESP8266)
    Freq = (uint32_t)system_get_cpu_freq();  
    // Tested ESP8266 at 80 and 160 MHz
    time1H = (800 * Freq) / 1000 - 11; 
    time0H = (400 * Freq) / 1000 - 15; 
    time1L = (450 * Freq) / 1000 - 21; 
    time0L = (850 * Freq) / 1000 - 24; 
  #else
    return; // Unsupported architecture
  #endif

 //Serial.printf("ws2812Write: %u MHz, time1H=%u, time1L=%u, time0H=%u, time0L=%u\r\n", 
 //   (unsigned int)Freq, (unsigned int)time1H, (unsigned int)time1L, 
 //   (unsigned int)time0H, (unsigned int)time0L);
 setColor(tx_color, pin, time1H, time1L, time0H, time0L);

  


}

