#ifndef _LED_DISPLAY_H_
#define _LED_DISPLAY_H_

#include <FastLED.h>
#include <freertos/FreeRTOS.h>

#define NUM_LEDS 1
#define DATA_PIN 35

class LED_DisPlay {
   private:
    CRGB _ledbuff[NUM_LEDS];
    uint8_t _numberled;

    uint8_t _mode;
    uint16_t _am_speed;
    uint8_t _am_mode;
    int32_t _count_x, _count_y;
    int32_t _am_count = -1;
    uint8_t *_am_buffptr;
    uint16_t _yRows    = 5;
    uint16_t _xColumns = 6;

    SemaphoreHandle_t _xSemaphore = NULL;

   public:
    uint8_t Brightness = 40;

    /* data */
   public:
    LED_DisPlay();
    ~LED_DisPlay();

    void begin(uint8_t LEDNumber = NUM_LEDS);

    void setBrightness(uint8_t brightness);
    void drawpix(CRGB Color);
    void clear();

    void show();
};

#endif
