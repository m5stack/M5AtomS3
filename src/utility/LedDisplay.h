#ifndef _LED_DISPLAY_H_
#define _LED_DISPLAY_H_

#include <FastLED.h>
#include <freertos/FreeRTOS.h>

#define ATOMS3_LITE_LEDS_NUM      1
#define ATOMS3_LITE_LEDS_DATA_PIN 35

class LedDisplay {
   private:
    uint8_t _ledPin;
    uint8_t _ledNum;
    uint8_t _brightness;
    CRGB leds[ATOMS3_LITE_LEDS_NUM];
    /* data */
   public:
    void begin();
    void drawpix(CRGB Color);
    void setBrightness(uint8_t brightness);
    void clear();
    void show();
};

#endif
