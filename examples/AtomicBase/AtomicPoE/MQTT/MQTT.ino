/**
 * @file MQTT.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5AtomS3 Atomic PoE Base MQTT Test
 * @version 0.1
 * @date 6023-12-13
 *
 *
 * @Hardwares: M5AtomS3 + Atomic PoE Base
 * @Platform Version: Arduino M5Stack Board Manager v2.0.9
 * @Dependent Library:
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5AtomS3: https://github.com/m5stack/M5AtomS3
 * M5_Ethernet: https://github.com/m5stack/M5-Ethernet
 * PubSubClient: https://github.com/knolleary/pubsubclient
 */

#include "M5AtomS3.h"
#include <SPI.h>
#include <M5_Ethernet.h>
#include <PubSubClient.h>

#define PUB_INTERVAL 3000

#define PUB_TOPIC "LAN_UPLINK"
#define SUB_TOPIC "LAN_DOWNLINK"

#define SCK  5
#define MISO 7
#define MOSI 8
#define CS   6

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x89};

const char* mqtt_server = "broker.emqx.io";

EthernetClient ethClient;
PubSubClient client(ethClient);

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("arduinoClient")) {
            Serial.println("connected");
            AtomS3.Display.clear();
            AtomS3.Display.drawString("Connected!", AtomS3.Display.width() / 2,
                                      60);

            // Once connected, publish an announcement...
            client.publish(PUB_TOPIC, "hello world");
            // ... and resubscribe
            client.subscribe(SUB_TOPIC);
        } else {
            AtomS3.Display.clear();
            AtomS3.Display.drawString("Failed!", AtomS3.Display.width() / 2,
                                      60);
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

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
        delay(1000);
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

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

long lastTime;

void loop() {
    if (!client.connected()) {
        reconnect();
    } else {
        client.loop();
        if (millis() - lastTime > PUB_INTERVAL) {
            lastTime = millis();
            AtomS3.Display.clear();
            AtomS3.Display.drawString("Publish!", AtomS3.Display.width() / 2,
                                      60);
            String data = "hello world: " + String(millis());
            client.publish(PUB_TOPIC, data.c_str());
        }
    }
}
