/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file PlayMP3FromSD.ino
 * @brief M5AtomS3 Atomic SPK SDCard MP3 Player
 * @version 0.1
 * @date 2024-11-22
 *
 *
 * @Hardwares: M5AtomS3 + Atomic SPK Base + MicroSD Card
 * @Platform Version: Arduino M5Stack Board Manager v2.1.2
 * @Dependent Library:
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5AtomS3: https://github.com/m5stack/M5AtomS3
 * ESP8266Audio: https://github.com/earlephilhower/ESP8266Audio
 * MP3 file link: http://gitfile.oss-cn-beijing.aliyuncs.com/11-fanfare.mp3
 */

#include "M5AtomS3.h"
#include <WiFi.h>

#include "SPIFFS.h"
#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceID3.h"

AudioGeneratorMP3 *mp3;
AudioFileSourceSD *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

#define SCK  7
#define MISO 8
#define MOSI 6

void StatusCallback(void *cbData, int code, const char *string)
{
    const char *ptr = reinterpret_cast<const char *>(cbData);
    char s1[64];
    strncpy_P(s1, string, sizeof(s1));
    s1[sizeof(s1) - 1] = 0;
    Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
    Serial.flush();
}

void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
    (void)cbData;
    Serial.printf("ID3 callback for: %s = '", type);

    if (isUnicode) {
        string += 2;
    }

    while (*string) {
        char a = *(string++);
        if (isUnicode) {
            string++;
        }
        Serial.printf("%c", a);
    }
    Serial.printf("'\n");
    Serial.flush();
}

void setup()
{
    AtomS3.begin();
    Serial.begin(115200);
    SPI.begin(SCK, MISO, MOSI, -1);
    if (!SD.begin()) {
        Serial.println("Card Mount Failed");
        return;
    }
    audioLogger = &Serial;
    file        = new AudioFileSourceSD("/11-fanfare.mp3");
    id3         = new AudioFileSourceID3(file);
    id3->RegisterMetadataCB(MDCallback, (void *)"ID3TAG");
    out = new AudioOutputI2S();

    out->SetPinout(5, 39, 38);

    mp3 = new AudioGeneratorMP3();
    mp3->RegisterStatusCB(StatusCallback, (void *)"mp3");
    mp3->begin(id3, out);
}

void loop()
{
    if (mp3->isRunning()) {
        if (!mp3->loop()) mp3->stop();
    } else {
        Serial.printf("MP3 done\n");
        delay(1000);
    }
}
