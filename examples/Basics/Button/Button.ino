/*
*******************************************************************************
* Copyright (c) 2022 by M5Stack
*                  Equipped with M5AtomS3 sample source code
*                          配套  M5AtomS3 示例源代码
* Visit for more information: https://docs.m5stack.com/en/core/AtomS3
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/core/AtomS3
*
* Describe: Button example.  按键示例
* Date: 2022/12/18
*******************************************************************************
Press button to display the corresponding output on the screen and
USBserial. 按下按键，在屏幕和串口上显示相应输出
*/
#include <M5AtomS3.h>
/* After M5AtomS3 is started or reset
  the program in the setUp () function will be run, and this part will only be
  run once. 在 M5AtomS3
  启动或者复位后，即会开始执行setup()函数中的程序，该部分只会执行一次。 */
void setup() {
    M5.begin(true, true, false, false);  // Init M5AtomS3.  初始化 M5AtomS3
    M5.Lcd.println("Pls Press Btn");
}

/* After the program in setup() runs, it runs the program in loop()
The loop() function is an infinite loop in which the program runs repeatedly
在setup()函数中的程序执行完后，会接着执行loop()函数中的程序
loop()函数是一个死循环，其中的程序会不断的重复运行 */
void loop() {
    M5.update();  // Read the press state of the key.  读取按键 A, B, C 的状态
    if (M5.Btn.wasReleased() || M5.Btn.pressedFor(1000)) {
        USBSerial.print('A');
        M5.Lcd.print("A");
    }
}
