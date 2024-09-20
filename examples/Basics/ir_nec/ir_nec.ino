/**
 * @file ir_nec.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5AtomS3 IR NEC test
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
 * IRremote: https://github.com/Arduino-IRremote/Arduino-IRremote
 */

#define DISABLE_CODE_FOR_RECEIVER  // Disables restarting receiver after each
                                   // send. Saves 450 bytes program memory and
                                   // 269 bytes RAM if receiving functions are
                                   // not used.
#define SEND_PWM_BY_TIMER
#define ATOMS3_IR_TX_PIN 4
// #define ATOMS3R_IR_TX_PIN 47

#include "M5AtomS3.h"
#include <IRremote.hpp>  // include the library

uint8_t sCommand = 0x34;
uint8_t sRepeats = 0;

void setup() {
    auto cfg = M5.config();
    AtomS3.begin(cfg);
    AtomS3.Display.setTextColor(GREEN);
    AtomS3.Display.setTextDatum(middle_center);
    AtomS3.Display.setFont(&fonts::FreeMono9pt7b);
    AtomS3.Display.setTextSize(1);

    IrSender.begin(DISABLE_LED_FEEDBACK);  // Start with IR_SEND_PIN as send pin
    IrSender.setSendPin(ATOMS3_IR_TX_PIN);
}

void loop() {
    Serial.println();
    Serial.print(F("Send now: address=0x1111, command=0x"));
    Serial.print(sCommand, HEX);
    Serial.print(F(", repeats="));
    Serial.print(sRepeats);
    Serial.println();

    AtomS3.Display.clear();
    AtomS3.Display.drawString("IR NEC SEND", AtomS3.Display.width() / 2, AtomS3.Display.height() / 2 - 40);

    AtomS3.Display.drawString("ADDR:0x1111", AtomS3.Display.width() / 2, AtomS3.Display.height() / 2);

    AtomS3.Display.drawString("CMD:0x" + String(sCommand, HEX), AtomS3.Display.width() / 2,
                              AtomS3.Display.height() / 2 + 40);

    Serial.println(F("Send standard NEC with 16 bit address"));

    AtomS3.Display.fillCircle(10, 105, 8, GREEN);
    IrSender.sendNEC(0x1111, sCommand, sRepeats);
    // IrSender.sendOnkyo(0x1111, 0x2223, sRepeats);
    /*
     * Increment send values
     */
    sCommand += 1;
    delay(500);
    AtomS3.Display.fillCircle(10, 105, 8, YELLOW);
    delay(500);
}
