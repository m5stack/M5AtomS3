/**
 * @file AtomicHDriver.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5AtomS3 Atomic H-Driver Base Test
 * @version 0.1
 * @date 2024-01-19
 *
 *
 * @Hardwares: M5AtomS3 + Atomic H-Driver Base
 * @Platform Version: Arduino M5Stack Board Manager v2.1.0
 * @Dependent Library:
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5AtomS3: https://github.com/m5stack/M5AtomS3
 */

#include "M5AtomS3.h"

const int IN1_PIN = 6;
const int IN2_PIN = 7;
int freq          = 10000;
int ledChannel1   = 0;
int ledChannel2   = 1;
int resolution    = 10;
bool direction    = true;
int VIN_PIN       = 8;
int FAULT_PIN     = 5;

void setup() {
    auto cfg = M5.config();
    AtomS3.begin(cfg);

    AtomS3.Display.setTextColor(GREEN);
    AtomS3.Display.setTextDatum(middle_center);
    AtomS3.Display.setFont(&fonts::Orbitron_Light_24);
    AtomS3.Display.setTextSize(1);
    AtomS3.Display.drawString("H-Driver", AtomS3.Display.width() / 2,
                              AtomS3.Display.height() / 2);

    ledcSetup(ledChannel1, freq, resolution);
    ledcSetup(ledChannel2, freq, resolution);
    ledcAttachPin(IN1_PIN, ledChannel1);
    ledcAttachPin(IN2_PIN, ledChannel2);
    pinMode(VIN_PIN, INPUT);
    pinMode(FAULT_PIN, INPUT);
    ledcWrite(ledChannel1, 0);
    ledcWrite(ledChannel2, 0);
}

void loop() {
    if (M5.BtnA.pressedFor(2000)) {
        ledcWrite(ledChannel1, 0);
        ledcWrite(ledChannel2, 0);
    }

    if (M5.BtnA.wasPressed()) {
        if (direction) {
            ledcWrite(ledChannel1, 1000);
            ledcWrite(ledChannel2, 0);
        } else {
            ledcWrite(ledChannel1, 0);
            ledcWrite(ledChannel2, 1000);
        }
        direction = !direction;
    }

    M5.update();
    Serial.println("VIN IN: " +
                   String((analogRead(VIN_PIN) * 10.0 / 4095.0) * 3.6));
    if (digitalRead(FAULT_PIN) == 0) {
        Serial.println("FAULT!");
    }
}
