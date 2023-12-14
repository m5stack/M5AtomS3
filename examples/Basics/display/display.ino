/**
 * @file display.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5AtomS3 Display Test
 * @version 0.1
 * @date 2023-12-13
 *
 *
 * @Hardwares: M5AtomS3
 * @Platform Version: Arduino M5Stack Board Manager v2.0.9
 * @Dependent Library:
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5AtomS3: https://github.com/m5stack/M5AtomS3
 */

#include "M5AtomS3.h"

void draw_function(LovyanGFX* gfx) {
    int x      = rand() % gfx->width();
    int y      = rand() % gfx->height();
    int r      = (gfx->width() >> 4) + 2;
    uint16_t c = rand();
    gfx->fillRect(x - r, y - r, r * 2, r * 2, c);
}

void setup() {
    auto cfg = M5.config();
    AtomS3.begin(cfg);

    int textsize = AtomS3.Display.height() / 60;
    if (textsize == 0) {
        textsize = 1;
    }
    AtomS3.Display.setTextSize(textsize);
}

void loop() {
    int x      = rand() % AtomS3.Display.width();
    int y      = rand() % AtomS3.Display.height();
    int r      = (AtomS3.Display.width() >> 4) + 2;
    uint16_t c = rand();
    AtomS3.Display.fillCircle(x, y, r, c);
    draw_function(&AtomS3.Display);
}