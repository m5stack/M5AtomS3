/**
 * @file AtomicMotion.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5AtomS3 Atomic Motion Test
 * @version 0.1
 * @date 2024-01-24
 *
 *
 * @Hardwares: M5AtomS3 + Atomic Motion
 * @Platform Version: Arduino M5Stack Board Manager v2.0.9
 * @Dependent Library:
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5AtomS3: https://github.com/m5stack/M5AtomS3
 * M5AtomicMotion: https://github.com/m5stack/M5Atomic-Motion
 */

#include "M5AtomS3.h"
#include "M5AtomicMotion.h"

M5AtomicMotion AtomicMotion;

void setup() {
    auto cfg = M5.config();
    AtomS3.begin(cfg);

    AtomS3.Display.setTextColor(GREEN);
    AtomS3.Display.setTextDatum(middle_center);
    AtomS3.Display.setTextSize(2);
    AtomS3.Display.drawString("Atomic Init", AtomS3.Display.width() / 2,
                              AtomS3.Display.height() / 2);
    while (
        !AtomicMotion.begin(&Wire, M5_ATOMIC_MOTION_I2C_ADDR, 38, 39, 100000)) {
        AtomS3.Display.clear();
        AtomS3.Display.drawString("Init Fail", AtomS3.Display.width() / 2,
                                  AtomS3.Display.height() / 2);
        Serial.println("Atomic Motion begin failed");
        delay(1000);
    }
    AtomS3.Display.clear();
    AtomS3.Display.drawString("Motion", AtomS3.Display.width() / 2,
                              AtomS3.Display.height() / 2);

    Serial.println("Atomic Motion Test");
}

void loop() {
    for (int ch = 0; ch < 2; ch++) {
        AtomicMotion.setMotorSpeed(ch, 127);
        Serial.printf("Motor Channel %d: %d \n", ch,
                      AtomicMotion.getMotorSpeed(ch));
    }
    delay(1000);
    for (int ch = 0; ch < 2; ch++) {
        AtomicMotion.setMotorSpeed(ch, -127);
        Serial.printf("Motor Channel %d: %d \n", ch,
                      AtomicMotion.getMotorSpeed(ch));
    }
    delay(1000);
    for (int ch = 0; ch < 2; ch++) {
        AtomicMotion.setMotorSpeed(ch, 0);
        Serial.printf("Motor Channel %d: %d \n", ch,
                      AtomicMotion.getMotorSpeed(ch));
    }
    delay(1000);
    for (int ch = 0; ch < 4; ch++) {
        AtomicMotion.setServoAngle(ch, 180);
        Serial.printf("Servo Channel %d: %d \n", ch,
                      AtomicMotion.getServoAngle(ch));
    }
    delay(1000);
    for (int ch = 0; ch < 4; ch++) {
        AtomicMotion.setServoAngle(ch, 0);
        Serial.printf("Servo Channel %d: %d \n", ch,
                      AtomicMotion.getServoAngle(ch));
    }
    delay(1000);
}
