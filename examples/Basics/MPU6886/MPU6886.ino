/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                  Equipped with AtomS3 sample source code
*                          配套  AtomS3 示例源代码
* Visit for more information: https://docs.m5stack.com/en/core/AtomS3
* 获取更多资料请访问：https://docs.m5stack.com/zh_CN/core/AtomS3
*
* Describe: MPU6886.  姿态传感器示例
* Date: 2022/12/19
*******************************************************************************
*/
#include "M5AtomS3.h"

/* After AtomS3 is started or reset the program in the setUp ()
function will be run, and this part will only be run once.
在 AtomS3 启动或者复位后，即会开始执行setup()函数中的程序，该部分只会执行一次。
*/
void setup() {
    M5.begin(true, true, true,
             false);  // Init AtomS3(Initialize LCD, serial port).
                      // 初始化 AtomS3(初始化LCD、串口)
    M5.IMU.begin();   // Init IMU sensor.  初始化姿态传感器
    USBSerial.printf("whoAmI() = 0x%02x\n", M5.IMU.whoAmI());
}

/* After the program in setup() runs, it runs the program in loop()
The loop() function is an infinite loop in which the program runs repeatedly
在setup()函数中的程序执行完后，会接着执行loop()函数中的程序
loop()函数是一个死循环，其中的程序会不断的重复运行 */
float ax, ay, az, gx, gy, gz, t;
void loop() {
    M5.Lcd.setCursor(0, 40);
    M5.Lcd.clear();                  // Delay 100ms 延迟100ms
    M5.IMU.getAccel(&ax, &ay, &az);  // Read tri-axial accel 读取三轴加速度
    M5.IMU.getGyro(&gx, &gy, &gz);  // Read gyroscope data 读取陀螺仪数据
    M5.IMU.getTemp(&t);             // Read temperature data 读取温度数据
    USBSerial.printf("%f,%f,%f,%f,%f,%f,%f\n", ax, ay, az, gx, gy, gz,
                     t);  // serial port output the formatted string.  串口输出
    M5.Lcd.printf("IMU:\r\n");
    M5.Lcd.printf("%0.2f %0.2f %0.2f\r\n", ax, ay, az);
    M5.Lcd.printf("%0.2f %0.2f %0.2f\r\n", gx, gy, gz);
    delay(500);
}
