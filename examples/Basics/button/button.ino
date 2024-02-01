/**
 * @file button.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5AtomS3 Button Test
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

void setup() {
    auto cfg = M5.config();
    AtomS3.begin(cfg);

    AtomS3.Display.setTextColor(GREEN);
    AtomS3.Display.setTextDatum(middle_center);
    AtomS3.Display.setFont(&fonts::Orbitron_Light_24);
    AtomS3.Display.setTextSize(1);
    AtomS3.Display.drawString("Click!", AtomS3.Display.width() / 2,
                              AtomS3.Display.height() / 2);
    Serial.println("Click BtnA to Test");
}

void loop() {
    AtomS3.update();
    if (AtomS3.BtnA.wasPressed()) {
        AtomS3.Display.clear();
        AtomS3.Display.drawString("Pressed", AtomS3.Display.width() / 2,
                                  AtomS3.Display.height() / 2);
        Serial.println("Pressed");
    }
    if (AtomS3.BtnA.wasReleased()) {
        AtomS3.Display.clear();
        AtomS3.Display.drawString("Released", AtomS3.Display.width() / 2,
                                  AtomS3.Display.height() / 2);
        Serial.println("Released");
    }
}
