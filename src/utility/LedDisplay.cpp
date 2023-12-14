
#include "LedDisplay.h"

void LedDisplay::begin() {
    _ledPin = ATOMS3_LITE_LEDS_DATA_PIN;
    _ledNum = ATOMS3_LITE_LEDS_NUM;
    FastLED.addLeds<NEOPIXEL, ATOMS3_LITE_LEDS_DATA_PIN>(leds,
                                                         ATOMS3_LITE_LEDS_NUM);
}

void LedDisplay::drawpix(CRGB color) {
    leds[0] = color;
}

void LedDisplay::setBrightness(uint8_t brightness) {
    brightness  = (brightness > 100) ? 100 : brightness;
    brightness  = (40 * brightness / 100);
    _brightness = brightness;
    FastLED.setBrightness(_brightness);
}

void LedDisplay::clear() {
    memset(leds, 0, sizeof(CRGB) * _ledNum);
}

void LedDisplay::show() {
    FastLED.show();
}
