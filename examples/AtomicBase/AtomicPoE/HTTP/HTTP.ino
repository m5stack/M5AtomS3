/**
 * @file HTTP.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5AtomS3 Atomic PoE Base HTTP Test
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
 * ArduinoHttpClient: https://github.com/arduino-libraries/ArduinoHttpClient
 */

#include "M5AtomS3.h"
#include <SPI.h>
#include <M5_Ethernet.h>
#include <ArduinoHttpClient.h>

#define SERVER "httpbin.org"

#define SCK  5
#define MISO 7
#define MOSI 8
#define CS   6

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x99};

EthernetClient ethClient;

HttpClient client = HttpClient(ethClient, SERVER);

void setup() {
    auto cfg = M5.config();
    AtomS3.begin(cfg);

    AtomS3.Display.setTextColor(GREEN);
    AtomS3.Display.setTextDatum(middle_center);
    AtomS3.Display.setFont(&fonts::Orbitron_Light_24);
    AtomS3.Display.setTextSize(1);

    SPI.begin(SCK, MISO, MOSI, -1);
    Ethernet.init(CS);

    AtomS3.Display.drawString("Init...", AtomS3.Display.width() / 2, 60);

    while (Ethernet.begin(mac) != 1) {
        Serial.println("Error getting IP address via DHCP, trying again...");
        delay(2000);
    }

    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println(
            "Ethernet shield was not found.  Sorry, can't run without "
            "hardware. :(");
        while (true) {
            delay(1);  // do nothing, no point running without Ethernet hardware
        }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is not connected.");
    }
}

void loop() {
    AtomS3.Display.clear();
    AtomS3.Display.drawString("GET", AtomS3.Display.width() / 2, 20);
    Serial.println("making GET request");

    client.get("/get");
    // read the status code and body of the response
    int statusCode  = client.responseStatusCode();
    String response = client.responseBody();

    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);
    Serial.println("Wait five seconds");

    AtomS3.Display.drawString("STATUS:", AtomS3.Display.width() / 2, 60);
    AtomS3.Display.drawString(String(statusCode), AtomS3.Display.width() / 2,
                              100);

    delay(5000);

    AtomS3.Display.clear();

    AtomS3.Display.drawString("POST", AtomS3.Display.width() / 2, 20);

    Serial.println("making POST request");

    String contentType = "application/x-www-form-urlencoded";
    String postData    = "name=Alice&age=12";

    client.post("/post", contentType, postData);

    // read the status code and body of the response
    statusCode = client.responseStatusCode();
    response   = client.responseBody();

    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);
    Serial.println("Wait five seconds");

    AtomS3.Display.drawString("STATUS:", AtomS3.Display.width() / 2, 60);
    AtomS3.Display.drawString(String(statusCode), AtomS3.Display.width() / 2,
                              100);

    delay(5000);
}
