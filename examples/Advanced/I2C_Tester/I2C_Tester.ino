/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                  Equipped with M5AtomS3 sample source code
*                          配套  M5AtomS3 示例源代码
* Visit for more information: https://docs.m5stack.com/en/core/AtomS3
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/core/AtomS3
*
* Describe: I2C Scanner.  I2C探测
* Date: 2023/1/15
*******************************************************************************
This program scans the addresses 1-127 continuosly and shows the devices found
on the TFT. 该程序连续扫描地址 1-127 并显示在外部(内部)I2C发现的设备。
*/
#include <M5AtomS3.h>

void setup() {
    M5.begin(true, true, true,
             false);  // Init M5AtomS3(Initialization of internal I2C is also
                      // included).  M5AtomS3(初始化内部I2C也包含在内)
    Wire.begin(2,
               1);  // Detect external I2C, if this sentence is not added,
                    // detect internal I2C.  检测外部I2C,若不加此句为检测内部I2C
    M5.Lcd.println("M5AtomS3 I2C Tester");  // Print a string on the screen.
                                            // 在屏幕上打印字符串
    delay(3000);
}

void loop() {
    int address;
    int error;
    M5.Lcd.printf("\nscanning Address [HEX]\n");
    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(
            address);  // Data transmission to the specified device address
                       // starts.   开始向指定的设备地址进行传输数据
        error = Wire.endTransmission(); /*Stop data transmission with the slave.
                  停止与从机的数据传输 0: success.  成功 1: The amount of data
                  exceeds the transmission buffer capacity limit.
                  数据量超过传送缓存容纳限制 return value:              2:
                  Received NACK when sending address.  传送地址时收到 NACK 3:
                  Received NACK when transmitting data.  传送数据时收到 NACK
                                             4: Other errors.  其它错误 */
        if (error == 0) {
            M5.Lcd.print(address, HEX);
            M5.Lcd.print(" ");
        } else
            M5.Lcd.print(".");

        delay(10);
    }
    delay(1000);
    M5.Lcd.setCursor(1, 12);
    M5.Lcd.fillRect(1, 15, 128, 128, BLACK);
}
