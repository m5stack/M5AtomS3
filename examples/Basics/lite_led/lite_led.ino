/**
 * @file Led.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5AtomS3 Lite LED Test
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
 * FastLED: https://github.com/FastLED/FastLED
 */

#include <M5AtomS3.h>

void setup() {
    AtomS3.begin(true);  // Init M5AtomS3Lite.
    AtomS3.dis.setBrightness(100);
}

void loop() {
    AtomS3.dis.drawpix(0xff0000);
    AtomS3.update();
    delay(500);
    AtomS3.dis.drawpix(0x00ff00);
    AtomS3.update();
    delay(500);
    AtomS3.dis.drawpix(0x0000ff);
    AtomS3.update();
    delay(500);
}
