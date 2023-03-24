#include <math.h>
#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <driver/ledc.h>
#include <driver/rmt.h>
#include <ir_tools.h>

#include "M5GFX.h"
#include "M5Unified.h"
#include "I2C_MPU6886.h"
#include "MadgwickAHRS.h"

#include "img_res.c"

#define VERSION         0.1
#define LAYOUT_OFFSET_Y 30

#define IR_RMT_TX_CHANNEL RMT_CHANNEL_2
#define IR_GPIO           4

typedef enum {
    FUNC_WIFI_SCAN,
    FUNC_I2C_SCAN,
    FUNC_UART_MON,
    FUNC_PWM_TEST,
    FUNC_ADC_TEST,
    FUNC_IR_SEND,
    FUNC_IMU_TEST,
    FUNC_MAX
} func_index_t;

static void boot_animation(void);
static void ir_tx_init(void);
static void ir_tx_send(uint32_t ir_cmd);

static char func_name_text[][16] = {
    "func_wifi_scan", "func_i2c_scan", "func_uart_mon", "func_pwm_test",
    "func_adc_test",  "func_ir_test",  "func_imu_test"};

const unsigned char* func_img_list[] = {
    wifi_scan_img, i2c_scan_img, uart_mon_img, io_pwm_img,
    io_adc_img,    ir_send_img,  imu_test_img,
};

I2C_MPU6886 imu(I2C_MPU6886_DEFAULT_ADDRESS, Wire1);
Madgwick filter;

class func_base_t {
   public:
    M5Canvas* _canvas;
    bool _draw_flag;
    bool _btn_clicked;

   public:
    void entry(M5Canvas* canvas_) {
        _canvas = canvas_;
        _canvas->fillRect(0, 0, _canvas->width(), _canvas->height(), TFT_BLACK);
        needDraw();
        start();
    };

    virtual void start();
    virtual void update(bool btn_click);
    virtual void stop();

    void needDraw() {
        _draw_flag = true;
    }

    virtual void draw() {
        if (_draw_flag) {
            _draw_flag = false;
            _canvas->pushSprite(0, LAYOUT_OFFSET_Y);
        }
    };

    void leave(void) {
        stop();
    }
};

class func_wifi_t : public func_base_t {
    time_t last_time_show   = 0;
    int16_t wifi_scan_count = 0;
    int16_t wifi_show_index = 0;
    bool wifi_scan_done     = false;

    void start() {
        wifi_scan_count = 0;
        wifi_show_index = 0;
        wifi_scan_done  = false;
        _btn_clicked    = false;

        WiFi.mode(WIFI_MODE_STA);
        WiFi.scanNetworks(true);

        _canvas->setTextWrap(false);
        _canvas->setTextScroll(true);
        _canvas->clear(TFT_BLACK);
        _canvas->setFont(&fonts::efontCN_16);
        _canvas->drawCenterString("Scaning...", _canvas->width() / 2,
                                  _canvas->height() / 2 - 12);
        _canvas->setFont(&fonts::efontCN_12);
        needDraw();
    }

    void update(bool btn_click) {
        if (!wifi_scan_done) {
            int16_t result = WiFi.scanComplete();
            if (result == WIFI_SCAN_RUNNING || result == WIFI_SCAN_FAILED) {
                return;
            } else if (result > 0) {
                wifi_scan_done  = true;
                wifi_scan_count = result;
                _canvas->setCursor(0, 0);
                _canvas->clear(TFT_BLACK);
            }
        } else {
            if (btn_click) {
                _btn_clicked = !_btn_clicked;
            }

            if (!_btn_clicked) {
                if (millis() - last_time_show > 500) {
                    if (wifi_show_index <
                        (wifi_scan_count < 5 ? wifi_scan_count : 5)) {
                        _canvas->printf("%d. %s %d\r\n", wifi_show_index + 1,
                                        WiFi.SSID(wifi_show_index).c_str(),
                                        WiFi.RSSI(wifi_show_index));
                        wifi_show_index++;
                    } else {
                        _canvas->printf("\r\n\r\n");
                        _canvas->setFont(&fonts::efontCN_14);
                        _canvas->printf("Top 5 list:\r\n");
                        _canvas->setFont(&fonts::efontCN_12);
                        wifi_show_index = 0;
                    }
                    last_time_show = millis();
                    needDraw();
                }
            } else {
                // update pause
            }
        }
    }

    void stop() {
        WiFi.scanDelete();
        _canvas->setFont(&fonts::Font0);
    }
};

class func_i2c_t : public func_base_t {
    uint8_t addr_list[6] = {};
    uint8_t device_count = 0;

    void start() {
        Wire.begin(2, 1);

        _canvas->setTextWrap(false);
        _canvas->setTextScroll(true);
        _canvas->clear(TFT_BLACK);
        _canvas->setFont(&fonts::efontCN_16);
        _canvas->drawCenterString("Scaning...", _canvas->width() / 2,
                                  _canvas->height() / 2 - 12);
        needDraw();
        _btn_clicked = false;
    }

    void update(bool btn_click) {
        if (btn_click) {
            _btn_clicked = !_btn_clicked;
            _canvas->clear(TFT_BLACK);
            _canvas->setTextSize(1);
            _canvas->setTextColor(TFT_WHITE);
            _canvas->setFont(&fonts::efontCN_16);
            _canvas->drawCenterString("Scaning...", _canvas->width() / 2,
                                      _canvas->height() / 2 - 12);
            needDraw();
            draw();
        }

        if (!_btn_clicked) {
            _btn_clicked = true;
            needDraw();

            uint8_t address = 0;
            device_count    = 0;
            memset(addr_list, sizeof(addr_list), 0);
            for (address = 1; address < 127; address++) {
                Wire.beginTransmission(address);
                uint8_t error = Wire.endTransmission();
                if (error == 0) {
                    addr_list[device_count] = address;
                    device_count++;

                    if (device_count > 5) {
                        break;
                    }
                }
            }
            _canvas->clear();
            if (device_count == 0) {
                _canvas->setFont(&fonts::efontCN_24);
                _canvas->setTextSize(1);
                _canvas->setTextColor(TFT_RED);
                _canvas->drawCenterString("Not found", 64,
                                          _canvas->height() / 2 - 16);
                return;
            }

            char addr_buf[4];
            _canvas->setFont(&fonts::efontCN_16);
            draw_form();
            for (size_t i = 0; i < device_count; i++) {
                sprintf(addr_buf, "%d. 0x%02X", i + 1, addr_list[i]);
                _canvas->setTextColor((random(0, 255) << 16 |
                                       random(0, 255) << 8 | random(0, 255)));
                _canvas->drawCenterString(addr_buf, (32 * i > 2 ? 1 : 0) + 32,
                                          (32 * (i % 3)) + 8);
            }
        }
    }

    void stop() {
        Wire.end();
        _canvas->setTextColor(TFT_WHITE);
    }

    void draw_form() {
        _canvas->drawFastHLine(0, _canvas->height() / 3 * 0 + 1,
                               _canvas->width(), TFT_WHITE);
        _canvas->drawFastHLine(0, _canvas->height() / 3 * 1, _canvas->width(),
                               TFT_WHITE);
        _canvas->drawFastHLine(0, _canvas->height() / 3 * 2, _canvas->width(),
                               TFT_WHITE);
        _canvas->drawFastHLine(0, _canvas->height() / 3 * 3, _canvas->width(),
                               TFT_WHITE);
        _canvas->drawFastVLine(_canvas->width() / 2 * 0, 1,
                               _canvas->height() - 2, TFT_WHITE);
        _canvas->drawFastVLine(_canvas->width() / 2 * 1, 1,
                               _canvas->height() - 2, TFT_WHITE);
        _canvas->drawFastVLine(_canvas->width() / 2 * 2 - 1, 1,
                               _canvas->height() - 2, TFT_WHITE);
    }
};

class func_uart_t : public func_base_t {
    const unsigned char* uart_mon_img_list[4] = {
        uart_mon_01_img, uart_mon_02_img, uart_mon_03_img, uart_mon_04_img};
    const uint8_t uart_io_list[4][2] = {{2, 1}, {2, 1}, {1, 2}, {1, 2}};
    const uint32_t uart_baud_list[4] = {9600, 115200, 9600, 115200};
    uint8_t uart_mon_mode_index      = 0;

    void start() {
        Serial.begin(uart_baud_list[uart_mon_mode_index], SERIAL_8N1,
                     uart_io_list[uart_mon_mode_index][0],
                     uart_io_list[uart_mon_mode_index][1]);
        M5.Display.drawPng(uart_mon_img_list[uart_mon_mode_index], ~0u, 0, 0);
        M5.Display.setFont(&fonts::Font0);
    }

    void update(bool btn_click) {
        if (btn_click) {
            uart_mon_mode_index++;
            if (uart_mon_mode_index > 3) {
                uart_mon_mode_index = 0;
            }

            Serial.end();
            Serial.begin(uart_baud_list[uart_mon_mode_index], SERIAL_8N1,
                         uart_io_list[uart_mon_mode_index][0],
                         uart_io_list[uart_mon_mode_index][1]);
            M5.Display.drawPng(uart_mon_img_list[uart_mon_mode_index], ~0u, 0,
                               0);
        }

        // Grove => USB
        size_t rx_len = Serial.available();
        if (rx_len) {
            for (size_t i = 0; i < rx_len; i++) {
                uint8_t c = Serial.read();
                USBSerial.write(c);
                M5.Display.fillRect(86, 31, 128 - 86, 9);
                M5.Display.setCursor(93, 33);
                M5.Display.setTextColor((random(0, 255) << 16 |
                                         random(0, 255) << 8 | random(0, 255)),
                                        TFT_BLACK);
                M5.Display.printf("0x%02X", c);
            }
        }

        // USB => Grove
        size_t tx_len = USBSerial.available();
        if (tx_len) {
            for (size_t i = 0; i < tx_len; i++) {
                uint8_t c = USBSerial.read();
                Serial.write(c);
                M5.Display.fillRect(86, 41, 128 - 86, 9, TFT_BLACK);
                M5.Display.setCursor(93, 43);
                M5.Display.setTextColor((random(0, 255) << 16 |
                                         random(0, 255) << 8 | random(0, 255)),
                                        TFT_BLACK);
                M5.Display.printf("0x%02X", c);
            }
        }
    }

    void stop() {
        Serial.end();
        M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
        gpio_reset_pin((gpio_num_t)1);
        gpio_reset_pin((gpio_num_t)2);
    }

    // not use canvas :)
    void draw() {
    }
};

class func_pwm_t : public func_base_t {
    const unsigned char* pwm_img_list[4] = {io_pwm_01_img, io_pwm_02_img,
                                            io_pwm_03_img, io_pwm_04_img};
    uint8_t pwm_mode_index               = 0;
    uint8_t pwm_duty                     = 128;

    void start() {
        M5.Display.drawPng(pwm_img_list[pwm_mode_index], ~0u, 0, 0);
        M5.Display.setFont(&fonts::efontCN_16_b);
        M5.Display.drawCenterString("F: 1Khz", 90, 52);

        ledcSetup(0, 1000, 8);
        ledcSetup(1, 1000, 8);
        ledcAttachPin(1, 0);
        ledcAttachPin(2, 1);
        ledcWrite(0, pwm_duty);
        ledcWrite(1, pwm_duty);

        drawDuty(pwm_duty);
    }

    void update(bool btn_click) {
        if (btn_click) {
            pwm_mode_index++;
            if (pwm_mode_index > 3) {
                pwm_mode_index = 0;
            }
            M5.Display.drawPng(pwm_img_list[pwm_mode_index], ~0u, 0, 0);
            M5.Display.drawCenterString("F: 1Khz", 90, 52);
            if (pwm_mode_index != 0) {
                pwm_duty = 0;
            } else {
                pwm_duty = 0xF;
            }
            ledcWrite(0, pwm_duty);
            ledcWrite(1, pwm_duty);
            drawDuty(pwm_duty);
        }

        if (USBSerial.available()) {
            pwm_duty = (uint8_t)USBSerial.read();
            if (pwm_mode_index <= 1) {
                ledcWrite(0, pwm_duty);
                ledcWrite(1, pwm_duty);
            }

            if (pwm_mode_index == 2) {
                ledcWrite(1, 0);
                ledcWrite(0, pwm_duty);
            }

            if (pwm_mode_index == 3) {
                ledcWrite(1, pwm_duty);
                ledcWrite(0, 0);
            }
            drawDuty(pwm_duty);
        }
    }

    void stop() {
        ledcDetachPin(1);
        ledcDetachPin(2);
        gpio_reset_pin((gpio_num_t)1);
        gpio_reset_pin((gpio_num_t)2);
    }

    void drawDuty(int duty) {
        M5.Display.fillRect(80, 30, 45, 18, TFT_BLACK);
        M5.Display.setCursor(85, 31);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.printf("0x%02X", duty);
    }

    // not use canvas :)
    void draw() {
    }
};

class func_adc_t : public func_base_t {
    time_t last_update_time = 0;

    uint32_t ch1_vol = 0;
    uint32_t ch2_vol = 0;

    void start() {
        M5.Display.drawPng(io_adc_01_img, ~0u, 0, 0);
        M5.Display.setFont(&fonts::efontCN_16_b);
    }

    void update(bool btn_click) {
        if (millis() - last_update_time < 90) {
            return;
        }

        ch1_vol = 0;
        ch2_vol = 0;
        for (size_t i = 0; i < 32; i++) {
            ch1_vol += analogRead(1);
            ch2_vol += analogRead(2);
        }
        drawVolValue((uint32_t)(ch1_vol / 32), (uint32_t)(ch2_vol / 32));
        last_update_time = millis();
    }

    void stop() {
    }

    void drawVolValue(uint32_t ch1, uint32_t ch2) {
        M5.Display.fillRect(0, 30, 50, 16, TFT_BLACK);
        M5.Display.fillRect(28, 50, 50, 16, TFT_BLACK);

        char buf[16] = {0};
        sprintf(buf, "%.01fV", ((ch1 / 4095.0f) * 3.3f));
        M5.Display.drawCenterString(buf, 30, 31);

        sprintf(buf, "%.01fV", ((ch2 / 4095.0f) * 3.3));
        M5.Display.drawCenterString(buf, 55, 51);
    }

    // not use canvas :)
    void draw() {
    }
};

class func_ir_t : public func_base_t {
    const unsigned char* ir_img_list[2] = {ir_send_01_img, ir_send_02_img};
    uint8_t ir_send_mode_index          = 0;

    uint32_t ir_cmd          = 0;
    time_t ir_last_send_time = 0;
    ;
    void start() {
        M5.Display.drawPng(ir_img_list[ir_send_mode_index], ~0u, 0, 0);
        M5.Display.setFont(&fonts::efontCN_16_b);
        M5.Display.setTextColor(TFT_PURPLE);
    }

    void update(bool btn_click) {
        if (btn_click) {
            ir_send_mode_index++;
            if (ir_send_mode_index > 1) {
                ir_send_mode_index = 0;
            }
            M5.Display.drawPng(ir_img_list[ir_send_mode_index], ~0u, 0, 0);
        }

        // USB => IR
        if (ir_send_mode_index == 0) {
            if (USBSerial.available()) {
                uint8_t c = USBSerial.read();
                ir_tx_send((uint32_t)c);
                drawIrData((uint32_t)c);
            }
        } else {
            if (millis() - ir_last_send_time > 1000) {
                ir_cmd++;
                ir_tx_send(ir_cmd);
                drawIrData(ir_cmd);
                ir_last_send_time = millis();
            }
        }
    }

    void stop() {
        M5.Display.setTextColor(TFT_WHITE);
    }

    void drawIrData(uint32_t _ir_cmd) {
        M5.Display.fillRect(25, 105, 75, 20, TFT_WHITE);
        char buf[8] = {0};
        sprintf(buf, "0x%02X", _ir_cmd);
        M5.Display.drawCenterString(buf, 64, 107);
    }

    // not use canvas :)
    void draw() {
    }
};

class func_imu_t : public func_base_t {
    struct ball_t {
        uint16_t x;
        uint16_t y;
        uint16_t r;
    };

    time_t last_imu_sample_time = 0;
    uint16_t center_x, center_y, c1_r, c2_r;
    float roll, pitch, yaw;
    ball_t ball;

    void start() {
        Wire1.begin(38, 39);
        imu.begin();
        filter.begin(20);  // 20hz

        center_x = (uint16_t)(_canvas->width() / 2);
        center_y = (uint16_t)(_canvas->height() / 2);
        c1_r     = (uint16_t)(_canvas->height() / 2 - 4);
        c2_r     = (uint16_t)(_canvas->height() / 4 - 4);

        ball.x = (uint16_t)(_canvas->width() / 2);
        ball.y = (uint16_t)(_canvas->height() / 2);
        ball.r = 5;
    }

    void update(bool btn_click) {
        if ((millis() - last_imu_sample_time) > (1000 / 25)) {
            float ax, ay, az, gx, gy, gz;

            imu.getAccel(&ax, &ay, &az);
            imu.getGyro(&gx, &gy, &gz);

            // update the filter, which computes orientation
            filter.updateIMU(gx, gy, gz, ax, ay, az);

            roll  = filter.getRoll();
            pitch = filter.getPitch();
            yaw   = filter.getYaw();

            // USBSerial.printf("[%ld] roll:%.01f pitch:%.01f yaw:%.01f\r\n",
            //                  millis(), roll, pitch, yaw);
            calculateBall(roll, pitch);
            drawCircle();
            needDraw();
            last_imu_sample_time = millis();
        }
    }

    void stop() {
        Wire1.end();
    }

    void drawCircle() {
        _canvas->drawCircle(center_x, center_y, c1_r,
                            _canvas->color24to16(0xCCCC33));
        _canvas->drawCircle(center_x, center_y, c2_r,
                            _canvas->color24to16(0xCCCC33));
        _canvas->drawFastHLine((_canvas->width() - _canvas->height()) / 2 + 4,
                               center_y, _canvas->height() - 8,
                               _canvas->color24to16(0xCCCC33));
        _canvas->drawFastVLine(center_x, 4, _canvas->height() - 8,
                               _canvas->color24to16(0xCCCC33));
    }

    void calculateBall(float _roll, float _pitch) {
        if (abs(_roll) > 90) {
            if (_roll < -90) {
                _roll = -90;
            } else {
                _roll = 90;
            }
        }
        if (abs(_pitch) > 90) {
            if (_pitch < -90) {
                _pitch = -90;
            } else {
                _pitch = 90;
            }
        }
        uint16_t x = (uint16_t)map(_pitch, -90.0f, 90.0f, center_x - c1_r,
                                   center_x + c1_r);
        uint16_t y = (uint16_t)map(_roll, -90.0f, 90.0f, center_y - c1_r,
                                   center_y + c1_r);
        drawBall(x, y);
    }

    void drawBall(int16_t x, float y) {
        // erase old
        _canvas->fillCircle(ball.x, ball.y, ball.r, TFT_BLACK);
        ball.x = x;
        ball.y = y;
        // draw new
        _canvas->fillCircle(ball.x, ball.y, ball.r,
                            _canvas->color24to16(0xCCCC33));
    }
};

func_wifi_t func_wifi_scan;
func_i2c_t func_i2c_scan;
func_uart_t func_uart_mon;
func_pwm_t func_pwm_test;
func_adc_t func_adc_test;
func_ir_t func_ir_send;
func_imu_t func_imu_test;

func_base_t* func_list[7] = {&func_wifi_scan, &func_i2c_scan, &func_uart_mon,
                             &func_pwm_test,  &func_adc_test, &func_ir_send,
                             &func_imu_test};

static char btn_state_text[][16] = {"", "wasHold", "wasClicked"};

static func_index_t func_index = FUNC_WIFI_SCAN;
static bool is_entry_func      = false;

static uint16_t ir_addr         = 0;
static rmt_item32_t* ir_items   = NULL;
static size_t ir_length         = 0;
static ir_builder_t* ir_builder = NULL;

static uint8_t mac_addr[6];

M5Canvas canvas(&M5.Display);

void setup() {
    M5.begin();
    USBSerial.begin(115200);

    esp_efuse_mac_get_default(mac_addr);
    ir_addr = (mac_addr[2] << 24) | (mac_addr[3] << 16) | (mac_addr[4] << 8) |
              mac_addr[5];

    M5.Display.begin();

    canvas.setColorDepth(16);
    canvas.createSprite(M5.Display.width(),
                        M5.Display.height() - LAYOUT_OFFSET_Y);

    boot_animation();
    delay(500);

    // Start entry funciton list
    M5.Display.drawPng(func_img_list[func_index], ~0u, 0, 0);

    ir_tx_init();
}

void loop() {
    M5.update();

    int btn_state = M5.BtnA.wasHold() ? 1 : M5.BtnA.wasClicked() ? 2 : 0;

    if (btn_state != 0) {
        // USBSerial.printf("BTN %s\r\n", btn_state_text[btn_state]);
    }

    if ((btn_state == 2) && !is_entry_func) {
        // next function
        func_index = (func_index_t)(func_index + 1);
        if (func_index == FUNC_MAX) {
            func_index = FUNC_WIFI_SCAN;
        }
        M5.Display.drawPng(func_img_list[func_index], ~0u, 0, 0);
    }

    if (btn_state == 1) {
        if (!is_entry_func) {
            is_entry_func = true;
            // USBSerial.printf("Entry function <%s>\r\n",
            //                  func_name_text[func_index]);
            // entry function
            func_list[func_index]->entry(&canvas);
        } else {
            is_entry_func = false;
            // USBSerial.printf("Leave function <%s>\r\n",
            //                  func_name_text[func_index]);
            // leave function
            func_list[func_index]->leave();
            M5.Display.drawPng(func_img_list[func_index], ~0u, 0, 0);
        }
    }

    if (is_entry_func) {
        func_list[func_index]->update(btn_state == 2 ? true : false);
        func_list[func_index]->draw();
    }

    delay(10);
}

uint32_t circle_color_list[8] = {0xcc3300, 0xff6633, 0xffff66, 0x33cc33,
                                 0x00ffff, 0x0000ff, 0xff3399, 0x990099};
static void boot_animation(void) {
    for (size_t c = 0; c < 8; c++) {
        M5.Display.fillArc(0, M5.Display.height(), c * 23, (c + 1) * 23, 270, 0,
                           circle_color_list[c]);
        delay(200);
    }
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

static void ir_tx_send(uint32_t _ir_cmd) {
    // Send new key code
    ESP_ERROR_CHECK(ir_builder->build_frame(ir_builder, ir_addr, _ir_cmd));
    ESP_ERROR_CHECK(ir_builder->get_result(ir_builder, &ir_items, &ir_length));
    // To send data according to the waveform items.
    rmt_write_items(IR_RMT_TX_CHANNEL, ir_items, ir_length, false);
    // Send repeat code
    vTaskDelay(ir_builder->repeat_period_ms / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(ir_builder->build_repeat_frame(ir_builder));
    ESP_ERROR_CHECK(ir_builder->get_result(ir_builder, &ir_items, &ir_length));
    rmt_write_items(IR_RMT_TX_CHANNEL, ir_items, ir_length, false);
}
