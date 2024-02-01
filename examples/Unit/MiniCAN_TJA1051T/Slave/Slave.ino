/**
 * @file Slave.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief Unit Mini CAN Test Slave
 * @version 0.1
 * @date 2024-02-01
 *
 *
 * @Hardwares: M5AtomS3 + Unit Mini CAN
 * @Platform Version: Arduino M5Stack Board Manager v2.0.9
 * @Dependent Library:
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5AtomS3: https://github.com/m5stack/M5AtomS3
 */

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/twai.h"
#include "esp_err.h"

#include "M5AtomS3.h"

/* --------------------- Definitions and static variables ------------------ */

#define RX_TASK_PRIO 8  // Receiving task priority
#define TX_GPIO_NUM  gpio_num_t(2)
#define RX_GPIO_NUM  gpio_num_t(1)

static const twai_general_config_t g_config =
    TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL);
static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_25KBITS();
static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

static void twai_receive_task(void *arg) {
    twai_message_t rx_msg;
    while (1) {
        twai_receive(&rx_msg, portMAX_DELAY);
        Serial.printf("received identifier: 0x%02X\r\n", rx_msg.identifier);
        // rx_msg.data
        Serial.print("received data: ");
        for (int i = 0; i < rx_msg.data_length_code; i++) {
            Serial.printf("0x%02X ", rx_msg.data[i]);
        }
        Serial.println();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    vTaskDelete(NULL);
}

void setup() {
    auto cfg = M5.config();
    AtomS3.begin(cfg);

    AtomS3.Display.setTextColor(GREEN);
    AtomS3.Display.setTextDatum(middle_center);
    AtomS3.Display.setFont(&fonts::FreeSansBold9pt7b);
    AtomS3.Display.setTextSize(1);

    AtomS3.Display.drawString("CAN Slave", AtomS3.Display.width() / 2,
                              AtomS3.Display.height() / 2);

    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    Serial.println("Driver installed");
    ESP_ERROR_CHECK(twai_start());
    xTaskCreatePinnedToCore(twai_receive_task, "twai_receive_task", 4096, NULL,
                            RX_TASK_PRIO, NULL, tskNO_AFFINITY);
}

void loop() {
}