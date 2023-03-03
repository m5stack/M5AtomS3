#include <Arduino.h>
#include <driver/rmt.h>
#include <WiFi.h>
#include <Wire.h>
#include <BLEClient.h>
#include <I2C_MPU6886.h>
#include <ir_tools.h>
#include <led_strip.h>
#include <MahonyAHRS.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "M5GFX.h"
#include "M5UnitOLED.h"
#include "atoms3.c"

#define VERSION "V0.3"

#define DEVICE_NAME         "ATOM-S3"
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define NE0_RMT_TX_CHANNEL RMT_CHANNEL_0
#define NEO_GPIO           35
#define NUM_LEDS           1
#define IR_RMT_TX_CHANNEL  RMT_CHANNEL_2
#define IR_GPIO            4

#define BL_GPIO 16

#define BTN_GPIO 41
#define IMU_ADDR 0x68

#define I2C_SDA 38
#define I2C_SCL 39

enum { DEV_UNKNOWN, ATOM_S3, ATOM_S3_LCD };

#include "cube.hpp"

extern const unsigned char ATOMS3[];

static void neopixel_init(void);
static void update_neopixel(uint8_t r, uint8_t g, uint8_t b);
static void ir_tx_init(void);
static void ir_tx_test(void);

static void ble_task(void *);
static void wifi_task(void *);
static void io_task(void *);

static uint8_t device_type          = DEV_UNKNOWN;
static uint8_t atom_s3_gpio_list[8] = {
    /*Bottom*/ 5, 6, 7, 8, I2C_SDA, I2C_SCL};
static uint8_t atom_s3_lcd_gpio_list[6] = {
    /*Bottom*/ 5, 6, 7, 8, /*Grove*/ 2, 1};  // I2C_SDA, I2C_SCL
static time_t last_io_reverse_time    = 0;
static time_t last_imu_print_time     = 0;
static time_t last_ir_send_time       = 0;
static time_t last_ble_change_time    = 0;
static time_t last_udp_broadcast_time = 0;
static int32_t last_wifi_scan_time     = -25000;

static bool wifi_scan_done_flag = false;
static bool factory_test_flag = true;
static bool btn_pressd_flag   = false;
static int btn_pressd_count   = 0;

static uint16_t ir_addr       = 0;
static uint8_t ir_cmd         = 0x20;
static rmt_item32_t *ir_items = NULL;
static size_t ir_length       = 0;

const char *udp_ap_broadcast_addr = "192.168.4.255";
const int udp_broadcast_port      = 3333;

static uint8_t neopixel_color_index   = 0;
static char neopixel_color_name[][6]  = {"Red", "Green", "Blue", "White",
                                         "Black"};
static uint32_t neopixel_color_list[] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFFFFFF,
                                         0x000000};
static uint8_t mac_addr[6];
static char name_buffer[24];

static M5GFX lcd;
static M5UnitOLED oled( 2, 1, 400000 ); // SDA, SCL, FREQ
static M5Canvas btn_canvas(&lcd);
static M5Canvas wifi_canvas(&lcd);
static M5Canvas imu_canvas(&lcd);
static M5Canvas ir_canvas(&lcd);
static I2C_MPU6886 imu(I2C_MPU6886_DEFAULT_ADDRESS, Wire);
static led_strip_t *strip       = NULL;
static ir_builder_t *ir_builder = NULL;

void setup() {
    // 按键
    pinMode(BTN_GPIO, INPUT_PULLUP);

    USBSerial.begin(115200);
    esp_efuse_mac_get_default(mac_addr);
    ir_addr = (mac_addr[4] << 8 | mac_addr[5]);

    sprintf(name_buffer, DEVICE_NAME "-%02X%02X%02X%02X%02X%02X",
            MAC2STR(mac_addr));

    // 判断型号
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.beginTransmission(IMU_ADDR);
    if (Wire.endTransmission() == 0) {
        device_type = ATOM_S3_LCD;
    } else {
        device_type = ATOM_S3;
    }

    // ATOM S3 LCD初始化
    if (device_type == ATOM_S3_LCD) {
        // LCD backlight
        pinMode(BL_GPIO, OUTPUT);
        digitalWrite(BL_GPIO, HIGH);

        lcd.init();
        lcd.setTextWrap(false);
        lcd.setTextColor(TFT_WHITE);

        if (!factory_test_flag) {
            lcd.drawPng(atom_s3_img, ~0u, 0, 0);
            while (true) {
                delay(10);
            }
        }

        lcd.clear(TFT_WHITE);
        delay(500);
        lcd.clear(TFT_RED);
        delay(500);
        lcd.clear(TFT_GREEN);
        delay(500);
        lcd.clear(TFT_BLUE);
        delay(500);
        lcd.clear(TFT_BLACK);
        delay(500);

        lcd.setFont(&fonts::efontCN_24_b);
        lcd.drawCenterString("ATOM S3", 64, 0);
        lcd.drawFastHLine(0, 26, 128);

        btn_canvas.setColorDepth(16);
        btn_canvas.createSprite(128, 18);
        btn_canvas.setTextWrap(false);
        btn_canvas.setTextScroll(false);
        btn_canvas.setFont(&fonts::efontCN_16);
        btn_canvas.setTextColor(TFT_GREEN);
        btn_canvas.printf("Button count: 0");
        btn_canvas.pushSprite(0, 28);

        imu_canvas.setColorDepth(16);
        imu_canvas.createSprite(128, 38);
        imu_canvas.setTextWrap(false);
        imu_canvas.setTextScroll(false);
        imu_canvas.setFont(&fonts::efontCN_12);
        imu_canvas.setTextColor(TFT_ORANGE);

        ir_canvas.setColorDepth(16);
        ir_canvas.createSprite(128, 18);
        ir_canvas.setTextWrap(false);
        ir_canvas.setTextScroll(false);
        ir_canvas.setFont(&fonts::efontCN_16);
        ir_canvas.setTextColor(TFT_RED);

        wifi_canvas.setColorDepth(16);
        wifi_canvas.createSprite(128, 40);
        wifi_canvas.setFont(&fonts::efontCN_12);
        wifi_canvas.printf("WiFi scaning...");
        wifi_canvas.pushSprite(0, 98);

        // IMU 初始化
        imu.begin();

        USBSerial.printf("ATOM-S3-LCD\r\n");
    } else if (device_type == ATOM_S3) {
        // Neo Pixel LED
        oled.begin();
        oled.setRotation(1);
        oled.setFont(&fonts::efontCN_24_b);
        oled.drawCenterString("ATOMS3 Lite", 64, 16);
        neopixel_init();
        update_neopixel(
            (neopixel_color_list[neopixel_color_index] >> 16 & 0xFF),
            (neopixel_color_list[neopixel_color_index] >> 8 & 0xFF),
            (neopixel_color_list[neopixel_color_index] & 0xFF));
    }

    // IR
    ir_tx_init();

    // BLE
    xTaskCreatePinnedToCore(ble_task, "ble_task", 4096 * 8, NULL, 1, NULL,
                            APP_CPU_NUM);

    // WIFI
    xTaskCreatePinnedToCore(wifi_task, "wifi_task", 4096 * 8, NULL, 1, NULL,
                            APP_CPU_NUM);

    xTaskCreatePinnedToCore(io_task, "io_task", 4096 * 2, NULL, 1, NULL,
                            APP_CPU_NUM);
}

void loop() {
    // 按键
    if (digitalRead(BTN_GPIO) == LOW) {
        delay(5);
        if (digitalRead(BTN_GPIO) == LOW) {
            btn_pressd_flag = true;
            btn_pressd_count++;
            neopixel_color_index += 1;
            if (neopixel_color_index > 4) {
                neopixel_color_index = 0;
            }
            while (digitalRead(BTN_GPIO) == LOW)
            {
                delay(300);
            }
        }
    }

    if (btn_pressd_flag) {
        if (device_type == ATOM_S3_LCD) {
            btn_canvas.setTextColor(TFT_GREEN);
            btn_canvas.setCursor(0, 0);
            btn_canvas.clear();
            // btn_canvas.printf("BTN OK %d\r\n", btn_pressd_count);
            btn_canvas.printf("Button count: %d", btn_pressd_count);
            btn_canvas.pushSprite(0, 28);
        } else if (device_type == ATOM_S3) {
            update_neopixel(
                (neopixel_color_list[neopixel_color_index] >> 16 & 0xFF),
                (neopixel_color_list[neopixel_color_index] >> 8 & 0xFF),
                (neopixel_color_list[neopixel_color_index] & 0xFF));
        }
        USBSerial.printf("BTN Pressed %d\r\n", btn_pressd_count);
        btn_pressd_flag = false;
    }

    // IMU
    if ((device_type == ATOM_S3_LCD) &&
        ((millis() - last_imu_print_time) > 300)) {
        float ax;
        float ay;
        float az;
        float gx;
        float gy;
        float gz;
        float t;
        float roll;
        float pitch;
        float yaw;

        imu.getAccel(&ax, &ay, &az);
        imu.getGyro(&gx, &gy, &gz);
        imu.getTemp(&t);

        imu_canvas.setCursor(0, 0);
        imu_canvas.clear();
        imu_canvas.setFont(&fonts::efontCN_16);
        imu_canvas.printf("IMU:\r\n");
        imu_canvas.setFont(&fonts::efontCN_12);
        imu_canvas.printf("%0.2f %0.2f %0.2f\r\n", ax, ay, az);
        imu_canvas.printf("%0.2f %0.2f %0.2f\r\n", gx, gy, gz);
        imu_canvas.pushSprite(0, 44);
        USBSerial.printf(
            "IMU\r\nax:%f ay:%f az:%f gx:%f gy:%f gz:%f temp:%f\r\n", ax, ay,
            az, gx, gy, gz, t);
        last_imu_print_time = millis();
    }

    if (millis() - last_ir_send_time > 1000) {
        ir_tx_test();
        if (device_type == ATOM_S3_LCD) {
            ir_canvas.setCursor(0, 0);
            ir_canvas.clear();
            ir_canvas.printf("IR: %02X %02X\r\n", ir_addr, ir_cmd);
            ir_canvas.pushSprite(0, 82);
        } else {

        }
        USBSerial.printf("IR Send >>> addr:%02X cmd:%02X\r\n", ir_addr, ir_cmd);
        last_ir_send_time = millis();
    }

    if (wifi_scan_done_flag) {
        if (device_type == ATOM_S3_LCD) {
            wifi_canvas.clear();
            wifi_canvas.setCursor(0, 0);
            wifi_canvas.printf("WiFi scan total: %d\r\n", WiFi.scanComplete());
            wifi_canvas.printf("1. %s %d", WiFi.SSID(0).c_str(), WiFi.RSSI(0));
            wifi_canvas.pushSprite(0, 98);
        } else {
            oled.clear();
            oled.setCursor(0, 0);
            oled.setFont(&fonts::efontCN_10);
            oled.printf("MAC: %02X%02X%02X%02X%02X%02X\r\n", MAC2STR(mac_addr));
            oled.setFont(&fonts::efontCN_12);
            oled.printf("Wi-Fi扫描个数: %d\r\n", WiFi.scanComplete());
            oled.printf("信号最好: %s\r\n信号值: %d", WiFi.SSID(0).c_str(), WiFi.RSSI(0));
        }
        USBSerial.printf("WiFi scan total: %d\r\n", WiFi.scanComplete());
        USBSerial.printf("1. %s %d", WiFi.SSID(0).c_str(), WiFi.RSSI(0));
        wifi_scan_done_flag = false;
    }

    delay(10);
}

static void neopixel_init(void) {
    // Init RMT
    rmt_config_t config = {.rmt_mode      = RMT_MODE_TX,
                           .channel       = RMT_CHANNEL_0,
                           .gpio_num      = (gpio_num_t)NEO_GPIO,
                           .clk_div       = 80,
                           .mem_block_num = 1,
                           .flags         = 0,
                           .tx_config     = {
                                   .carrier_freq_hz      = 38000,
                                   .carrier_level        = RMT_CARRIER_LEVEL_HIGH,
                                   .idle_level           = RMT_IDLE_LEVEL_LOW,
                                   .carrier_duty_percent = 33,
                                   .carrier_en           = false,
                                   .loop_en              = false,
                                   .idle_output_en       = true,
                           }};
    // set counter clock to 40MHz
    config.clk_div = 2;
    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
    // install ws2812 driver
    led_strip_config_t strip_config =
        LED_STRIP_DEFAULT_CONFIG(64, (led_strip_dev_t)config.channel);
    strip = led_strip_new_rmt_ws2812(&strip_config);
    if (!strip) {
        USBSerial.printf("Install WS2812 driver failed");
    }
}

static void update_neopixel(uint8_t r, uint8_t g, uint8_t b) {
    ESP_ERROR_CHECK(strip->set_pixel(strip, 0, r, g, b));
    ESP_ERROR_CHECK(strip->refresh(strip, 0));
}

static void ir_tx_init(void) {
    rmt_config_t rmt_tx_config = {.rmt_mode      = RMT_MODE_TX,
                                  .channel       = RMT_CHANNEL_2,
                                  .gpio_num      = (gpio_num_t)IR_GPIO,
                                  .clk_div       = 80,
                                  .mem_block_num = 1,
                                  .flags         = 0,
                                  .tx_config     = {
                                          .carrier_freq_hz = 38000,
                                          .carrier_level   = RMT_CARRIER_LEVEL_HIGH,
                                          .idle_level      = RMT_IDLE_LEVEL_LOW,
                                          .carrier_duty_percent = 33,
                                          .carrier_en           = false,
                                          .loop_en              = false,
                                          .idle_output_en       = true,
                                  }};

    rmt_tx_config.tx_config.carrier_en = true;
    rmt_config(&rmt_tx_config);
    rmt_driver_install(IR_RMT_TX_CHANNEL, 0, 0);
    ir_builder_config_t ir_builder_config =
        IR_BUILDER_DEFAULT_CONFIG((ir_dev_t)IR_RMT_TX_CHANNEL);
    ir_builder_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT;
    ir_builder = ir_builder_rmt_new_nec(&ir_builder_config);
}

static void ir_tx_test(void) {
    ir_cmd++;
    USBSerial.printf("Send command 0x%x to address 0x%x\r\n", ir_cmd, ir_addr);
    // Send new key code
    ESP_ERROR_CHECK(ir_builder->build_frame(ir_builder, ir_addr, ir_cmd));
    ESP_ERROR_CHECK(ir_builder->get_result(ir_builder, &ir_items, &ir_length));
    // To send data according to the waveform items.
    rmt_write_items(IR_RMT_TX_CHANNEL, ir_items, ir_length, false);
    // Send repeat code
    vTaskDelay(ir_builder->repeat_period_ms / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(ir_builder->build_repeat_frame(ir_builder));
    ESP_ERROR_CHECK(ir_builder->get_result(ir_builder, &ir_items, &ir_length));
    rmt_write_items(IR_RMT_TX_CHANNEL, ir_items, ir_length, false);
}

static void ble_task(void *) {
    BLEDevice::init(name_buffer);
    BLEServer *pServer                 = BLEDevice::createServer();
    BLEService *pService               = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

    pCharacteristic->setValue("Hello world from " DEVICE_NAME "!");
    pService->start();
    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still
    // is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(
        0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    static char buffer[64] = {0};

    size_t ble_count = 0;
    while (1) {
        if (millis() - last_ble_change_time > 1000) {
            ble_count++;
            // sprintf(buffer, "Hello world from %s! %d", DEVICE_NAME, millis()
            // / 1000);
            sprintf(buffer, "Hello world from %s! %d", DEVICE_NAME, ble_count);
            pCharacteristic->setValue(buffer);
            USBSerial.println("BLE update ok");
            last_ble_change_time = millis();
        }
        delay(10);
    }
}

static void wifi_task(void *) {
    WiFi.mode(WIFI_MODE_APSTA);

    WiFi.softAP(name_buffer, "88888888");
    IPAddress myIP = WiFi.softAPIP();
    USBSerial.printf("AP:\r\nSSID: %s\r\nPSWD: %s\r\nIP address: ", name_buffer,
                     "88888888");
    USBSerial.println(myIP);
    WiFi.disconnect();

    WiFiUDP udp_ap;
    udp_ap.begin(WiFi.softAPIP(), udp_broadcast_port);

    size_t udp_count = 0;
    while (1) {
        if (millis() - last_wifi_scan_time > 20000) {
            USBSerial.println("\r\nWiFi scan start");

            // WiFi.scanNetworks will return the number of networks found
            int n = WiFi.scanNetworks();
            USBSerial.println("WiFi scan done");
            if (n == 0) {
                USBSerial.println("no networks found");
            } else {
                USBSerial.print(n);
                USBSerial.println(" networks found");
                for (int i = 0; i < n; ++i) {
                    // Print SSID and RSSI for each network found
                    USBSerial.print(i + 1);
                    USBSerial.print(": ");
                    USBSerial.print(WiFi.SSID(i));
                    USBSerial.print(" (");
                    USBSerial.print(WiFi.RSSI(i));
                    USBSerial.print(")");
                    USBSerial.println(
                        (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
                    delay(10);
                }
            }
            USBSerial.println("");
            last_wifi_scan_time = millis();
            wifi_scan_done_flag = true;
        }

        if (millis() - last_udp_broadcast_time > 1000) {
            udp_count++;
            udp_ap.beginPacket(udp_ap_broadcast_addr, udp_broadcast_port);
            udp_ap.printf("UDP broadcast count: %lu", udp_count);
            USBSerial.printf("UDP broadcast %s\r\n",
                             udp_ap.endPacket() == 1 ? "OK" : "ERROR");
            last_udp_broadcast_time = millis();
        }
        delay(10);
    }
}

static void io_task(void *) {
    if (device_type == ATOM_S3) {
        for (size_t i = 0; i < sizeof(atom_s3_gpio_list); i++) {
            pinMode(atom_s3_gpio_list[i], OUTPUT);
            digitalWrite(atom_s3_gpio_list[i], LOW);
        }
        Wire.end();
    } else if (device_type == ATOM_S3_LCD) {
        for (size_t i = 0; i < sizeof(atom_s3_lcd_gpio_list); i++) {
            pinMode(atom_s3_lcd_gpio_list[i], OUTPUT);
            digitalWrite(atom_s3_lcd_gpio_list[i], LOW);
        }
    }

    while (true) {
        if (device_type == ATOM_S3) {
            for (size_t i = 0; i < sizeof(atom_s3_gpio_list); i++) {
                digitalWrite(atom_s3_gpio_list[i],
                             !digitalRead(atom_s3_gpio_list[i]));
                delay(200);
            }
        } else if (device_type == ATOM_S3_LCD) {
            for (size_t i = 0; i < sizeof(atom_s3_lcd_gpio_list); i++) {
                digitalWrite(atom_s3_lcd_gpio_list[i],
                             !digitalRead(atom_s3_lcd_gpio_list[i]));
                delay(200);
            }
        }
    }
}
