// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#include "M5AtomS3.h"

M5AtomS3::M5AtomS3() {
}

M5AtomS3::~M5AtomS3() {
}

void M5AtomS3::begin(bool LCDEnable, bool USBSerialEnable, bool I2CEnable,
                     bool LEDEnable) {
    if (I2CEnable) {
        Wire1.begin(38, 39, 100000UL);
    }

    if (USBSerialEnable) {
        USBSerial.begin(115200);
        USBSerial.flush();
        delay(1200);
        USBSerial.println("M5AtomS3 initializing...OK");
    }

    Btn.begin();

    if (LCDEnable) {
        Lcd.begin();
        Lcd.clear();
        Lcd.setCursor(1, 2);
    }

    if (LEDEnable) {
        dis.begin();
    }
}

void M5AtomS3::update() {
    Btn.read();
}

M5AtomS3 M5;
