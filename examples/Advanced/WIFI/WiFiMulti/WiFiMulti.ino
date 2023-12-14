/*
*******************************************************************************
* Copyright (c) 2022 by M5Stack
*                  Equipped with M5AtomS3 sample source code
*                          配套  M5AtomS3 示例源代码
* Visit for more information: https://docs.m5stack.com/en/core/AtomS3
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/core/AtomS3
*
* Describe: WIFI Multi.  多wifi择优
* Date: 2023/1/15
*******************************************************************************
*  Connect to the best AP based on a given wifi list
*  根据给定wifi的列表连接到最好的AP
*/

#include <M5AtomS3.h>
#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

void setup() {
    AtomS3.begin();                      // Init M5AtomS3.  初始化 M5AtomS3
    wifiMulti.addAP("wifi1", "123456");  // Storage wifi configuration
                                         // information 1.  存储wifi配置信息1
    wifiMulti.addAP("wifi2", "123456123456");
    wifiMulti.addAP("aaa", "sadf");
    AtomS3.Lcd.print("Connecting Wifi...");  // Serial port format output
                                             // string. 串口格式化输出字符串
}

void loop() {
    if (wifiMulti.run() ==
        WL_CONNECTED) {  // If the connection to wifi is established
                         // successfully.  如果与wifi成功建立连接
        AtomS3.Lcd.setCursor(0, 20);
        AtomS3.Lcd.print("WiFi connected\n\nSSID:");
        AtomS3.Lcd.println(WiFi.SSID());  // Output Network name.  输出网络名称
        AtomS3.Lcd.print("RSSI: ");
        AtomS3.Lcd.println(
            WiFi.RSSI());  // Output signal strength.  输出信号强度
        AtomS3.Lcd.print("IP address: ");
        AtomS3.Lcd.println(WiFi.localIP());  // Output IP Address.  输出IP地址
        delay(1000);
        AtomS3.Lcd.fillRect(0, 20, 180, 300,
                            BLACK);  // It's equivalent to partial screen
                                     // clearance. 相当于部分清屏
    } else {
        // If the connection to wifi is not established successfully.
        // 如果没有与wifi成功建立连接
        AtomS3.Lcd.print(".");
        delay(1000);
    }
}