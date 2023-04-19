#ifndef _M5ATOMS3_H_
#define _M5ATOMS3_H_

#if defined(ESP32)

#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
#include "M5Display.h"

#include "utility/MPU6886.h"
#include "utility/Button.h"
#include "utility/LED_DisPlay.h"

class M5AtomS3 {
   public:
    M5AtomS3(/* args */);
    ~M5AtomS3();

    MPU6886 IMU;
    // LCD
    M5Display Lcd = M5Display();

    LED_DisPlay dis;

    Button Btn = Button(41, 25, true, true);

    void begin(bool LCDEnable = true, bool SerialEnable = true,
               bool I2CEnable = false, bool LEDEnable = false);
    void update();
};

extern M5AtomS3 M5;
#define m5  M5
#define lcd Lcd
#define imu IMU
#define Imu IMU

#else
#error “This library only supports boards with ESP32 processor.”
#endif
#endif
