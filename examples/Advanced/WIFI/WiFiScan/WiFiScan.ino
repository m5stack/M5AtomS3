/*
*******************************************************************************
* Copyright (c) 2022 by M5Stack
*                  Equipped with M5AtomS3 sample source code
*                          配套  M5AtomS3 示例源代码
* Visit for more information: https://docs.m5stack.com/en/core/AtomS3
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/core/AtomS3
*
* Describe: Wifi scan.  wifi扫描
* Date: 2023/1/15
*******************************************************************************
*/
#include <M5AtomS3.h>
#include "WiFi.h"

void setup() {
    M5.begin();           // Init M5AtomS3.  初始化M5AtomS3
    WiFi.mode(WIFI_STA);  // Set WiFi to station mode and disconnect from an AP
                          // if it was previously connected.
                          // 将WiFi设置为站模式，如果之前连接过AP，则断开连接
    WiFi.disconnect();  // Turn off all wifi connections.  关闭所有wifi连接
    delay(100);         // 100 ms delay.  延迟100ms
    M5.Lcd.print("WIFI SCAN");  // Screen print string.  屏幕打印字符串
}

void loop() {
    M5.Lcd.setCursor(2, 2);  // Set the cursor at (2,2).  将光标设置在(2,2)处
    M5.Lcd.println("Please press Btn to (re)scan");
    M5.update();  // Check the status of the key.  检测按键的状态
    if (M5.Btn.isPressed()) {  // If button is pressed.  如果按键按下
        M5.Lcd.clear();        // Clear the screen.  清空屏幕
        M5.Lcd.println("scan start");
        int n = WiFi.scanNetworks();  // return the number of networks found.
                                      // 返回发现的网络数
        if (n == 0) {  // If no network is found.  如果没有找到网络
            M5.Lcd.println("no networks found");
        } else {  // If have network is found.  找到网络
            M5.Lcd.printf("networks found:%d\n\n", n);
            for (int i = 0; i < n;
                 ++i) {  // Print SSID and RSSI for each network found.
                         // 打印每个找到的网络的SSID和信号强度
                M5.Lcd.printf("%d:", i + 1);
                M5.Lcd.print(WiFi.SSID(i));
                M5.Lcd.printf("(%d)", WiFi.RSSI(i));
                M5.Lcd.println(
                    (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
                delay(10);
            }
        }
        delay(1000);
    }
}
