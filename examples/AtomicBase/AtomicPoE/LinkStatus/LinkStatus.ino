/**
 * @file LinkStatus.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5AtomS3 Atomic PoE Base LinkStatus Test
 * @version 0.1
 * @date 2023-12-13
 *
 *
 * @Hardwares: M5AtomS3 + Atomic PoE Base
 * @Platform Version: Arduino M5Stack Board Manager v2.0.9
 * @Dependent Library:
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5AtomS3: https://github.com/m5stack/M5AtomS3
 * M5_Ethernet: https://github.com/m5stack/M5-Ethernet
 */

#include "M5AtomS3.h"
#include <SPI.h>
#include <M5_Ethernet.h>

#define SCK  5
#define MISO 7
#define MOSI 8
#define CS   6

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x89};

void setup() {
    auto cfg = M5.config();
    AtomS3.begin(cfg);

    AtomS3.Display.setTextColor(GREEN);
    AtomS3.Display.setTextDatum(middle_center);
    AtomS3.Display.setFont(&fonts::Orbitron_Light_24);
    AtomS3.Display.setTextSize(1);
    AtomS3.Display.drawString("PoE", AtomS3.Display.width() / 2, 20);

    SPI.begin(SCK, MISO, MOSI, -1);
    Ethernet.init(CS);
}

void loop() {
    auto link = Ethernet.linkStatus();
    Serial.print("Link status: ");
    switch (link) {
        case Unknown:
            AtomS3.Display.clear();
            AtomS3.Display.drawString("Unknown", AtomS3.Display.width() / 2,
                                      AtomS3.Display.height() / 2);
            Serial.println("Unknown");
            break;
        case LinkON:
            AtomS3.Display.clear();
            AtomS3.Display.drawString("ON", AtomS3.Display.width() / 2,
                                      AtomS3.Display.height() / 2);
            Serial.println("ON");
            break;
        case LinkOFF:
            AtomS3.Display.clear();
            AtomS3.Display.drawString("OFF", AtomS3.Display.width() / 2,
                                      AtomS3.Display.height() / 2);
            Serial.println("OFF");
            break;
    }
    delay(500);
}
