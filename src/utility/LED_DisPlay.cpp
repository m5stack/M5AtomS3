#include "LED_DisPlay.h"

LED_DisPlay::LED_DisPlay() {
}

LED_DisPlay::~LED_DisPlay() {
}

void LED_DisPlay::begin(uint8_t LEDNumber) {
    FastLED.addLeds<WS2812, DATA_PIN>(_ledbuff, LEDNumber);
    // FastLED.addLeds<WS2812, DATA_PIN>(leds, NUM_LEDS);
}

void LED_DisPlay::setBrightness(uint8_t brightness) {
    brightness = (brightness > 100) ? 100 : brightness;
    brightness = (40 * brightness / 100);
    Brightness = brightness;
    FastLED.setBrightness(Brightness);
}

void LED_DisPlay::drawpix(CRGB Color) {
    _ledbuff[0] = Color;
}

void LED_DisPlay::clear() {
    _ledbuff[0] = 0;
}

void LED_DisPlay::show() {
    FastLED.show();
}