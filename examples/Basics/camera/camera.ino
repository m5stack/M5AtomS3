/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#include "M5AtomS3.h"
#include "camera_pins.h"
#include <WiFi.h>
#include "esp_camera.h"

#define STA_MODE
// #define AP_MODE

const char* ssid     = "yourssid";
const char* password = "yourpasswd";

WiFiServer server(80);
camera_fb_t* fb    = NULL;
uint8_t* out_jpg   = NULL;
size_t out_jpg_len = 0;

static void jpegStream(WiFiClient* client);

static camera_config_t camera_config = {
    .pin_pwdn     = PWDN_GPIO_NUM,
    .pin_reset    = RESET_GPIO_NUM,
    .pin_xclk     = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,
    .pin_d7       = Y9_GPIO_NUM,
    .pin_d6       = Y8_GPIO_NUM,
    .pin_d5       = Y7_GPIO_NUM,
    .pin_d4       = Y6_GPIO_NUM,
    .pin_d3       = Y5_GPIO_NUM,
    .pin_d2       = Y4_GPIO_NUM,
    .pin_d1       = Y3_GPIO_NUM,
    .pin_d0       = Y2_GPIO_NUM,

    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href  = HREF_GPIO_NUM,
    .pin_pclk  = PCLK_GPIO_NUM,

    .xclk_freq_hz = 20000000,
    .ledc_timer   = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format  = PIXFORMAT_RGB565,
    .frame_size    = FRAMESIZE_QVGA,
    .jpeg_quality  = 0,
    .fb_count      = 2,
    .fb_location   = CAMERA_FB_IN_PSRAM,
    .grab_mode     = CAMERA_GRAB_LATEST,
    .sccb_i2c_port = 0,
};

void setup() {
    Serial.begin(115200);
    pinMode(POWER_GPIO_NUM, OUTPUT);
    digitalWrite(POWER_GPIO_NUM, LOW);

    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        Serial.println("Camera Init Fail");
        delay(1000);
        esp_restart();
    } else {
        Serial.println("Camera Init Success");
    }

#ifdef STA_MODE

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.setSleep(false);
    Serial.println("");

    Serial.print("Connecting to ");
    Serial.println(ssid);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
#endif

#ifdef AP_MODE
    if (!WiFi.softAP(ssid, password)) {
        log_e("Soft AP creation failed.");
        while (1)
            ;
    }

    Serial.println("AP SSID:");
    Serial.println(ssid);
    Serial.println("AP PASSWORD:");
    Serial.println(password);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
#endif

    server.begin();
}

void loop() {
    WiFiClient client = server.available();  // listen for incoming clients
    if (client) {                            // if you get a client,
        while (client.connected()) {         // loop while the client's connected
            if (client.available()) {        // if there's bytes to read from the
                jpegStream(&client);
            }
        }
        // close the connection:
        client.stop();
        Serial.println("Client Disconnected.");
    }
}

// used to image stream
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY     = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART         = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

static void jpegStream(WiFiClient* client) {
    Serial.println("Image stream satrt");
    client->println("HTTP/1.1 200 OK");
    client->printf("Content-Type: %s\r\n", _STREAM_CONTENT_TYPE);
    client->println("Content-Disposition: inline; filename=capture.jpg");
    client->println("Access-Control-Allow-Origin: *");
    client->println();
    static int64_t last_frame = 0;
    if (!last_frame) {
        last_frame = esp_timer_get_time();
    }

    for (;;) {
        fb = esp_camera_fb_get();
        if (fb) {
            frame2jpg(fb, 255, &out_jpg, &out_jpg_len);

            Serial.printf("pic size: %d\n", out_jpg_len);
            client->print(_STREAM_BOUNDARY);
            client->printf(_STREAM_PART, out_jpg_len);
            int32_t to_sends    = out_jpg_len;
            int32_t now_sends   = 0;
            uint8_t* out_buf    = out_jpg;
            uint32_t packet_len = 8 * 1024;
            while (to_sends > 0) {
                now_sends = to_sends > packet_len ? packet_len : to_sends;
                if (client->write(out_buf, now_sends) == 0) {
                    goto client_exit;
                }
                out_buf += now_sends;
                to_sends -= packet_len;
            }

            int64_t fr_end     = esp_timer_get_time();
            int64_t frame_time = fr_end - last_frame;
            last_frame         = fr_end;
            frame_time /= 1000;
            Serial.printf("MJPG: %luKB %lums (%.1ffps)\r\n", (long unsigned int)(out_jpg_len / 1024),
                          (long unsigned int)frame_time, 1000.0 / (long unsigned int)frame_time);

            if (fb) {
                esp_camera_fb_return(fb);
                fb = NULL;
            }
            if (out_jpg) {
                free(out_jpg);
                out_jpg     = NULL;
                out_jpg_len = 0;
            }
        } else {
            Serial.println("Camera capture failed");
        }
    }

client_exit:
    if (fb) {
        esp_camera_fb_return(fb);
        fb = NULL;
    }
    client->stop();
    Serial.printf("Image stream end\r\n");
}