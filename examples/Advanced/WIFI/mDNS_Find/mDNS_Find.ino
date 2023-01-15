/*
*******************************************************************************
* Copyright (c) 2022 by M5Stack
*                  Equipped with M5AtomS3 sample source code
*                          配套  M5AtomS3 示例源代码
* Visit for more information: https://docs.m5stack.com/en/core/AtomS3
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/core/AtomS3
*
* Describe: announcing & finding services.  广播&查找服务
* Date: 2023/1/15
*******************************************************************************
*/

#include <ESPmDNS.h>
#include <M5AtomS3.h>
#include <WiFi.h>

// Set the name and password of the wifi to be connected.
// 配置所连接wifi的名称和密码
const char* ssid     = "your-ap";
const char* password = "your-password";

void browseService(const char* service,
                   const char* proto) {  // find devices.  查找设备
    M5.Lcd.printf("Browsing for _%s._%s.local ", service, proto);
    int n =
        MDNS.queryService(service, proto);  // Store the number of devices found
                                            // in n.  将找到的设备数存储在n中
    if (n == 0) {  // if don't have any devices.  如果没有任何设备
        M5.Lcd.println("no services found");
    } else {
        M5.Lcd.printf("%d service(s) found\n", n);
        for (int i = 0; i < n;
             ++i) {  // Print details for each service found. 打印每个找到的设备
            M5.Lcd.printf(" %d: ", i + 1);
            M5.Lcd.print(
                MDNS.hostname(i));  // output the devies name.  输出设备名称
            M5.Lcd.print("  ");
            M5.Lcd.print(
                MDNS.IP(i));  // Output the devices IP Address. 输出设备的IP地址
            M5.Lcd.printf(
                ":%d\n",
                MDNS.port(i));  // output the devices port.  输出设备的端口号
        }
    }
    M5.Lcd.println();
}

void setup() {
    M5.begin();                  // Init M5AtomS3.  初始化 M5AtomS3
    WiFi.begin(ssid, password);  // Connect wifi and return connection status.
                                 // 连接wifi并返回连接状态
    M5.Lcd.print("Connected to ");
    M5.Lcd.print(ssid);  // Output Network name.  输出网络名称
    while (WiFi.status() !=
           WL_CONNECTED) {  // If the wifi connection fails.  若wifi未连接成功
        delay(250);         // delay 0.5s.  延迟0.5s
        M5.Lcd.print(".");
    }
    M5.Lcd.print("Success!\nIP address: ");
    M5.Lcd.println(WiFi.localIP());  // Output IP Address.  输出IP地址

    if (!MDNS.begin("ESP32_Browser")) {  // if init error.  如果初始化错误
        M5.Lcd.println("Error setting up MDNS responder!");
        while (1) {
            delay(1000);
        }
    }
}

void loop() {
    browseService("http", "tcp");
    delay(1000);
    browseService("arduino", "tcp");
    delay(1000);
    browseService("workstation", "tcp");
    delay(3000);
    M5.Lcd.clear();
    M5.Lcd.setCursor(0, 0);
    browseService("smb", "tcp");
    delay(1000);
    browseService("afpovertcp", "tcp");
    delay(1000);
    browseService("ftp", "tcp");
    delay(1000);
    browseService("ipp", "tcp");
    delay(1000);
    browseService("printer", "tcp");
    delay(10000);
}