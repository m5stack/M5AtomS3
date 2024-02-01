/**
 * @file WebServer.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5AtomS3 Atomic PoE Base WebServer Test
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
 */

#include "M5AtomS3.h"
#include <SPI.h>
#include <M5_Ethernet.h>

#define SCK  5
#define MISO 7
#define MOSI 8
#define CS   6

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x99};
IPAddress ip(192, 168, 1, 177);

EthernetServer server(80);

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

    // start the server
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
    AtomS3.Display.setTextSize(0.5);
    AtomS3.Display.clear();
    AtomS3.Display.drawString(Ethernet.localIP().toString().c_str(),
                              AtomS3.Display.width() / 2, 60);
}

long lastTime;

void loop() {
    // listen for incoming clients
    EthernetClient client = server.available();
    if (client) {
        Serial.println("new client");
        // an http request ends with a blank line
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                Serial.write(c);
                // if you've gotten to the end of the line (received a newline
                // character) and the line is blank, the http request has ended,
                // so you can send a reply
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println(
                        "Connection: close");  // the connection will be closed
                                               // after completion of the
                                               // response
                    client.println("Refresh: 5");  // refresh the page
                                                   // automatically every 5 sec
                    client.println();
                    client.println("<!DOCTYPE HTML>");
                    client.println("<html>");
                    client.print("<h2>Hello M5Stack LAN Module!</h2>");
                    client.println("</html>");
                    break;
                }
                if (c == '\n') {
                    // you're starting a new line
                    currentLineIsBlank = true;
                } else if (c != '\r') {
                    // you've gotten a character on the current line
                    currentLineIsBlank = false;
                }
            }
        }
        // give the web browser time to receive the data
        delay(1);
        // close the connection:
        client.stop();
        Serial.println("client disconnected");
    }
}
