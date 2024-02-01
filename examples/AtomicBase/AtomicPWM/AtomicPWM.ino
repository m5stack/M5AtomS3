/**
 * @file AtomicPWM.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5AtomS3 Atomic PWM Base Test
 * @version 0.1
 * @date 2023-12-13
 *
 *
 * @Hardwares: M5AtomS3 + Atomic PWM Base
 * @Platform Version: Arduino M5Stack Board Manager v2.0.9
 * @Dependent Library:
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5AtomS3: https://github.com/m5stack/M5AtomS3
 */

#include "M5AtomS3.h"
#include <Arduino.h>
#include "driver/ledc.h"

#define SIGNAL 5

int freq        = 10000;
int ledChannel1 = 0;
int resolution  = 10;

void setup() {
    auto cfg = M5.config();
    AtomS3.begin(cfg);

    AtomS3.Display.setTextColor(GREEN);
    AtomS3.Display.setTextDatum(middle_center);
    AtomS3.Display.setFont(&fonts::Orbitron_Light_24);
    AtomS3.Display.setTextSize(1);
    AtomS3.Display.drawString("PWM", AtomS3.Display.width() / 2,
                              AtomS3.Display.height() / 2);

    ledcSetup(ledChannel1, freq, resolution);
    ledcAttachPin(SIGNAL, ledChannel1);
}

void loop() {
    for (int i = 0; i < 500; i++) {
        ledcWrite(ledChannel1, i);
        delay(2);
    }

    for (int i = 500; i > 0; i--) {
        ledcWrite(ledChannel1, i);
        delay(2);
    }
}
