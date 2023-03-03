/*
*******************************************************************************
* Copyright (c) 2022 by M5Stack
*                  Equipped with M5AtomS3Lite sample source code
*                          配套  M5AtomS3Lite 示例源代码
* Visit for more information: https://docs.m5stack.com/en/core/AtomS3%20Lite
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/core/AtomS3%20Lite
*
* Describe: LED Show example.  LED展示示例
* Date: 2023/1/3
*******************************************************************************
LED colour switching after burning program.
烧录程序后LED彩灯颜色不断切换.
*/
#include <M5AtomS3.h>
/* After M5AtomS3 is started or reset
  the program in the setUp () function will be run, and this part will only be
  run once. 在 M5AtomS3
  启动或者复位后，即会开始执行setup()函数中的程序，该部分只会执行一次。 */
void setup() {
    M5.begin(false, true, false,
             true);  // Init M5AtomS3Lite.  初始化 M5AtomS3Lite
    USBSerial.println("Pls Press Btn change color");
}

/* After the program in setup() runs, it runs the program in loop()
The loop() function is an infinite loop in which the program runs repeatedly
在setup()函数中的程序执行完后，会接着执行loop()函数中的程序
loop()函数是一个死循环，其中的程序会不断的重复运行 */
void loop() {
    M5.dis.drawpix(0xff0000);
    M5.dis.show();
    delay(500);
    M5.dis.drawpix(0x00ff00);
    M5.dis.show();
    delay(500);
    M5.dis.drawpix(0x0000ff);
    M5.dis.show();
    delay(500);
}
