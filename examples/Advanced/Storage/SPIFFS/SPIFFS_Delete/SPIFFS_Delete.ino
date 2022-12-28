/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                  Equipped with M5AtomS3 sample source code
*                          配套  M5AtomS3 示例源代码
* Visit for more information: https://docs.m5stack.com/en/core/AtomS3
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/core/AtomS3
*
* Describe: SPIFFS Delete
* Date: 2022/12/20
******************************************************************************
*/

#include <M5AtomS3.h>
#include <SPIFFS.h>

String file_name =
    "/M5Stack/notes.txt";  // Sets the location and name of the file to be
                           // operated on.  设置被操作的文件位置和名称
void setup() {
    M5.begin();  // Init M5Atom.  初始化 M5ATOM

    if (SPIFFS.begin()) {  // Start SPIFFS, return 1 on success.
                           // 启动闪存文件系统,若成功返回1
        M5.Lcd.println("\nSPIFFS Started.");  // Serial port output format
                                              // String.  串口输出格式化字符串
    } else {
        M5.Lcd.println("SPIFFS Failed to Start.");
    }

    if (SPIFFS.remove(
            file_name)) {  // Delete file_name file from flash, return 1 on
                           // success.  从闪存中删除file_name文件,如果成功返回1
        M5.Lcd.print(file_name);
        M5.Lcd.println(" Remove sucess");
    } else {
        M5.Lcd.print(file_name);
        M5.Lcd.println(" Remove fail");
    }
}

void loop() {
}